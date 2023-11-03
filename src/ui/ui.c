/*
	OpenBikeComputer core application
    Copyright (C) 2023  LAMBS Pierre-Antoine

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <lvgl.h>
#include <wayland.h>
#include <semaphore.h>

#include "main_screen.h"
#include "data_screen.h"
#include "navigation_screen.h"
#include "profiles_screen.h"
#include "profiles_riders_screen.h"
#include "profiles_bikes_screen.h"
#include "results_screen.h"
#include "routes_screen.h"
#include "settings_screen.h"

#include "fifo.h"
#include "log.h"
#include "system.h"
#include "ui.h"

#define STATUS_BAR_SIZE ((ui_get_resolution_ver() / 10))
#define STATUS_BAR_TIMER_DELAY (1000) //ms
#define SCREEN_FIFO_DEPTH 8
#define STR_TIME_SIZE (8)

static struct {
	int (*enter)(lv_obj_t * screen);
	int (*exit)(void);
	bool bar_visible; /* display or not the status bar */
	bool bar_back_button; /* display or not the back button in the status bar */
	E_screen_id bar_back_button_screen; /* screen id where the back button lead */
} screen_table[E_SCREEN_ID_MAX] = {
	[E_MAIN_SCREEN]           = {.enter = &main_screen_enter,            .exit = &main_screen_exit,           .bar_visible = true,  .bar_back_button = false, .bar_back_button_screen = 0},
	[E_DATA_SCREEN]           = {.enter = &data_screen_enter,            .exit = &data_screen_exit,           .bar_visible = true,  .bar_back_button = true,  .bar_back_button_screen = E_MAIN_SCREEN},
	[E_NAVIGATION_SCREEN]     = {.enter = &navigation_screen_enter,      .exit = &navigation_screen_exit,     .bar_visible = false, .bar_back_button = false, .bar_back_button_screen = 0},
	[E_RESULTS_SCREEN]        = {.enter = &results_screen_enter,         .exit = &results_screen_exit,        .bar_visible = true,  .bar_back_button = true,  .bar_back_button_screen = E_MAIN_SCREEN},
	[E_ROUTES_SCREEN]         = {.enter = &routes_screen_enter,          .exit = &routes_screen_exit,         .bar_visible = true,  .bar_back_button = true,  .bar_back_button_screen = E_MAIN_SCREEN},
	[E_PROFILES_SCREEN]       = {.enter = &profiles_screen_enter,        .exit = &profiles_screen_exit,       .bar_visible = true,  .bar_back_button = true,  .bar_back_button_screen = E_MAIN_SCREEN},
	[E_PROFILES_RIDER_SCREEN] = {.enter = &profiles_riders_screen_enter, .exit = &profiles_riders_screen_exit,.bar_visible = true,  .bar_back_button = true,  .bar_back_button_screen = E_PROFILES_SCREEN},
	[E_PROFILES_BIKE_SCREEN]  = {.enter = &profiles_bikes_screen_enter,  .exit = &profiles_bikes_screen_exit, .bar_visible = true,  .bar_back_button = true,  .bar_back_button_screen = E_PROFILES_SCREEN},
	[E_SETTINGS_SCREEN]       = {.enter = &settings_screen_enter,        .exit = &settings_screen_exit,       .bar_visible = true,  .bar_back_button = true,  .bar_back_button_screen = E_MAIN_SCREEN},
};

typedef struct {
	lv_obj_t *bar;
	lv_obj_t *back_btn;
	lv_obj_t *back_btn_label;
	lv_obj_t *label;
	lv_timer_t *timer;
} T_status_bar;

static struct {
	bool is_initialized;
	/* display settings */
	int resolution_hor;
	int resolution_ver;
	lv_disp_rot_t rotation;

	/* lvgl display */
	lv_disp_t *display;
	lv_obj_t *virt_screen;
	lv_style_t virt_screen_style;
	lv_style_t default_style;

	/* threads */
	pthread_t tick_thread;
	pthread_t draw_thread;
	pthread_t screen_thread;

	/* mutex */
	pthread_mutex_t lvgl_mutex;

	/* screen information*/
	E_screen_id previous_screen;
	E_screen_id actual_screen;
	T_fifo screen_fifo;

	/* status bar */
	T_status_bar status_bar;
} ui = {
	.is_initialized = false,
	.lvgl_mutex = PTHREAD_MUTEX_INITIALIZER,
	.actual_screen = E_MAIN_SCREEN,
	.previous_screen = E_MAIN_SCREEN,
	.status_bar = {0},
};

static int _push_next_screen_in_fifo(E_screen_id next)
{
	int ret = fifo_push(&ui.screen_fifo, (int*)&next);
	fail_if_negative(ret, -1, "fifo_push next screen (%d) failed, return %d\n", next, ret);

	return 0;
}

static int _get_local_time(char *str, int size)
{
	fail_if_null(str, -1, "str is null\n");

	time_t t;
	struct tm * tmp;

	t = time(NULL);
	tmp = localtime(&t);
	fail_if_null(tmp, -2, "tmp is null\n");

	strftime(str, size, "%H:%M", tmp);

	return 0;
}

static void _statusbar_timer_handler(lv_timer_t * timer)
{
	int ret = 0;

	lv_obj_t *label = timer->user_data;

	char str[STR_TIME_SIZE];
	ret = _get_local_time(str, sizeof(str));
	if(ret < 0)
	{
		log_error("_get_local_time failed, return %d\n");
		return;
	}
	lv_label_set_text_fmt(label, LV_SYMBOL_GPS " " LV_SYMBOL_WIFI " " LV_SYMBOL_BATTERY_FULL " %s", str);
}

static void _status_bar_back_btn_handler(lv_event_t *event)
{
	int *user_data = lv_event_get_user_data(event);
	E_screen_id next_screen = *user_data;

	ui_change_screen(next_screen);
}

static int _create_status_bar(bool back_button_visible, E_screen_id *back_screen)
{
	int ret = 0;

	/* Create the status bar */
	ui.status_bar.bar = lv_obj_create(lv_scr_act());
	lv_obj_set_size(ui.status_bar.bar, ui_get_resolution_hor(), STATUS_BAR_SIZE);
	lv_obj_align(ui.status_bar.bar, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_add_style(ui.status_bar.bar, &ui.default_style, 0);

	/* Create the status bar back button if needed */
	if(back_button_visible)
	{
		/* Create back button */
		ui.status_bar.back_btn = lv_btn_create(ui.status_bar.bar);
		lv_obj_add_event_cb(ui.status_bar.back_btn, &_status_bar_back_btn_handler, LV_EVENT_CLICKED, (void*)back_screen);
		lv_obj_set_size(ui.status_bar.back_btn, lv_pct(25), lv_pct(90));
		lv_obj_align(ui.status_bar.back_btn, LV_ALIGN_LEFT_MID, 0, 0);

		/* Put the text in the back button label */
		ui.status_bar.back_btn_label = lv_label_create(ui.status_bar.back_btn);
		lv_label_set_text(ui.status_bar.back_btn_label, "< Back");
		lv_obj_center(ui.status_bar.back_btn_label);
	}

	/* Fill the status bar with widgets */
	ui.status_bar.label = lv_label_create(ui.status_bar.bar);
	char str[STR_TIME_SIZE];
	ret = _get_local_time(str, sizeof(str));
	fail_if_negative(ret, -2, "_get_local_time failed, return %d\n", ret);
	lv_label_set_text_fmt(ui.status_bar.label, LV_SYMBOL_GPS " " LV_SYMBOL_WIFI " " LV_SYMBOL_BATTERY_FULL " %s", str);
    lv_obj_align(ui.status_bar.label, LV_ALIGN_RIGHT_MID, 0, 0);

	/* Create lvgl timer that update the bar each secondes */
	ui.status_bar.timer = lv_timer_create(&_statusbar_timer_handler, STATUS_BAR_TIMER_DELAY, ui.status_bar.label);
	lv_timer_set_repeat_count(ui.status_bar.timer, -1); // repeat indefinitly

	return 0;
}

static int _destroy_status_bar(void)
{
	if(ui.status_bar.timer != NULL)
	{
		lv_timer_del(ui.status_bar.timer);
		ui.status_bar.timer = NULL;
	}
	return 0;
}

static void * screen_thread_handler(void *data)
{
	int ret = 0;
	int next = 0;

	while(1)
	{
		ret = fifo_pop_wait(&ui.screen_fifo, &next);
		if(ret < 0)
		{
			log_error("fifo_pop_wait failed, return: %d\n", ret);
			continue;
		}

		/* Check enter and exit handler, if null print log and skip the screen change */
		if(screen_table[next].enter == NULL)
		{
			log_error("next screen (%d) enter handler is NULL\n", next);
			continue;
		}
		if(screen_table[ui.actual_screen].exit == NULL)
		{
			log_error("actual screen (%d) exit handler is NULL\n", ui.actual_screen);
			continue;
		}

		/* Clean the actual_screen screen before continuing */
		log_debug("LVGL: Clean actual screen\n");
		pthread_mutex_lock(&ui.lvgl_mutex);
		/* Destroy the status bar before cleaning the display */
		_destroy_status_bar();
		/* Clean the display */
		lv_obj_clean(lv_scr_act());
		pthread_mutex_unlock(&ui.lvgl_mutex);

		log_debug("Run actual screen exit handler\n");
		pthread_mutex_lock(&ui.lvgl_mutex);
		ret = screen_table[ui.actual_screen].exit();
		pthread_mutex_unlock(&ui.lvgl_mutex);
		if(ret < 0)
		{
			log_error("exiting screen %d failed, returned: %d\n", ui.actual_screen, ret);
			/* If it fail we take the shot and continue */
		}

		/* Create an LVGL objet that is the size of the screen minus the size
		 * of the statusbar if the status bar is needed, if not the size of the
		 * fullscreen.
		 * Each screen enter handler will use that to create the children objects.
		 * */
		pthread_mutex_lock(&ui.lvgl_mutex);

		/* Create the virtual screen object */
		ui.virt_screen = lv_obj_create(lv_scr_act());

		/* Apply global style on the virt screen */
		lv_obj_add_style(ui.virt_screen, &ui.virt_screen_style, 0);

		if(screen_table[next].bar_visible)
		{
			/* Bar is visible, resize the virtual screen object to the correct dimensions */
			lv_obj_set_size(ui.virt_screen, ui_get_resolution_hor(), ui_get_resolution_ver() - STATUS_BAR_SIZE);
			lv_obj_align(ui.virt_screen, LV_ALIGN_TOP_MID, 0, STATUS_BAR_SIZE);

			/* Create the status bar in the same display */
			ret = _create_status_bar(screen_table[next].bar_back_button, &screen_table[next].bar_back_button_screen);
			if(ret < 0)
			{
				log_error("_create_status_bar failed, return: %d\n", ret);
				exit(-1);
			}
		}
		else
		{
			/* screen were the statusbar is not displayed */
			lv_obj_set_size(ui.virt_screen, ui_get_resolution_hor(), ui_get_resolution_ver());
			lv_obj_align(ui.virt_screen, LV_ALIGN_TOP_MID, 0, 0);
		}
		pthread_mutex_unlock(&ui.lvgl_mutex);

		/* Enter the next screen */
		log_debug("Run next screen enter handler\n");
		pthread_mutex_lock(&ui.lvgl_mutex);
		ret = screen_table[next].enter(ui.virt_screen);
		pthread_mutex_unlock(&ui.lvgl_mutex);
		if(ret < 0)
		{
			log_error("entering screen %d failed, returned: %d\n", next, ret);
			exit(-1);
		}

		/* Actualize the screen manager state */
		ui.previous_screen = ui.actual_screen;
		ui.actual_screen = next;

		log_info("Info: changed screen from %d to %d\n", ui.previous_screen, ui.actual_screen);
	}

	/* If we are here something wrong happen, kill the application */
	log_error("LVGL tick thread exit, kill the application\n");
	exit(-1);

	return NULL;
}

static void * tick_thread_handler(void *data)
{
	/* The tick thread tell the LVGL lib the time elapsed */
	while(1)
	{
		pthread_mutex_lock(&ui.lvgl_mutex);
		lv_tick_inc(LVGL_REFRESH_TICK_RATE);
		pthread_mutex_unlock(&ui.lvgl_mutex);
		usleep(LVGL_REFRESH_TICK_RATE * 1000);
	}

	/* If we are here something wrong happen, kill the application */
	log_error("LVGL tick thread exit, kill the application\n");
	exit(-1);

	return NULL;
}

static void * draw_thread_handler(void *data)
{
	int sleep;
	struct pollfd pfd;
	uint32_t time_till_next;

	/* Get wayland events fd (touch/keyboard/mouse/others) */
	pfd.fd = lv_wayland_get_fd();
	pfd.events = POLLIN;

	while (1) {
		/* Handle any Wayland/LVGL timers/events */
		pthread_mutex_lock(&ui.lvgl_mutex);
		time_till_next = lv_wayland_timer_handler();
		pthread_mutex_unlock(&ui.lvgl_mutex);

		/* Run until the last window closes */
		if (!lv_wayland_window_is_open(NULL)) {
			break;
		}

		/* Wait for something interesting to happen */
		if (time_till_next == LV_NO_TIMER_READY) {
			sleep = -1;
		} else if (time_till_next > INT_MAX) {
			sleep = INT_MAX;
		} else {
			sleep = time_till_next;
		}

		while ((poll(&pfd, 1, sleep) < 0) && (errno == EINTR));
	}

	/* If we are here something wrong happen, kill the application */
	log_error("LVGL draw thread exit, kill the application\n");
	exit(-1);

	return NULL;
}

int _init_defaults_style(void)
{
	/* Screen widget default style */
	lv_style_init(&ui.default_style);
	lv_style_set_text_font(&ui.default_style, &DEFAULT_FONT);

	/* Virtual screen default style */
	lv_style_init(&ui.virt_screen_style);
	lv_style_set_text_font(&ui.virt_screen_style, &DEFAULT_FONT);
	lv_style_set_border_width(&ui.virt_screen_style, 0);
	lv_style_set_outline_width(&ui.virt_screen_style, 0);
	lv_style_set_pad_all(&ui.virt_screen_style, 0);

	return 0;
}

int ui_apply_default_style_to_obj(lv_obj_t *obj)
{
	fail_if_false(ui.is_initialized, -1, "ui is not initialized\n");

	lv_obj_add_style(obj, &ui.default_style, 0);

	return 0;
}

int ui_change_screen(E_screen_id next)
{
	int ret = 0;

	fail_if_false(ui.is_initialized, -1, "ui is not initialized\n");

	if(next < 0 || next > E_SCREEN_ID_MAX)
	{
		fail(-2, "Invalid screen id given\n");
	}

	if(next == ui.actual_screen)
	{
		/* Nothing todo */
		return 0;
	}

	log_info("New screen asked: %d\n", next);

	/* Display the next screen */
	ret = _push_next_screen_in_fifo(next);
	fail_if_negative(ret, -3, "_push_next_screen_in_fifo failed, return %d\n", ret);

	return 0;
}

int ui_go_to_previous_screen(void)
{
	int ret = 0;

	fail_if_false(ui.is_initialized, -1, "ui is not initialized\n");

	/* Display the next screen */
	// TODO unsafe, need mutex
	ret = _push_next_screen_in_fifo(ui.previous_screen);
	fail_if_negative(ret, -3, "_push_next_screen_in_fifo failed, return %d\n", ret);

	return 0;
}

int ui_get_resolution_hor(void)
{
	fail_if_false(ui.is_initialized, -1, "ui is not initialized\n");
	
	return ui.resolution_hor;
}

int ui_get_resolution_ver(void)
{
	fail_if_false(ui.is_initialized, -1, "ui is not initialized\n");
	
	return ui.resolution_ver;
}

int ui_init(int resolution_hor, int resolution_ver, int screen_rotation)
{
	int ret;

	fail_if_true(ui.is_initialized, -1, "ui is already initialized\n");

	/* Init lvgl lib and wayland driver */
	lv_init();
	lv_wayland_init();

	ret = fifo_create(&ui.screen_fifo, sizeof(int), SCREEN_FIFO_DEPTH);
	fail_if_negative(ret, -2, "fifo_create fail, return: %d\n", ret);

	log_debug("display %dx%d, rotation: %d\n", resolution_hor, resolution_ver, screen_rotation);

	/* Create a display */
	ui.resolution_hor = resolution_hor;
	ui.resolution_ver = resolution_ver;
	ui.display = lv_wayland_create_window(ui.resolution_hor, ui.resolution_ver, "openbikecomputer", NULL /*close_cb*/);
	fail_if_null(ui.display, -3, "lv_wayland_create_window return NULL\n");

	/* Set window in fullscreen mode */
	lv_wayland_window_set_fullscreen(ui.display, true);

	ui.rotation = LV_DISP_ROT_NONE;
	switch(screen_rotation)
	{
		case 0:
			ui.rotation = LV_DISP_ROT_NONE;
			break;
		case 90:
			ui.rotation = LV_DISP_ROT_90;
			break;
		case 180:
			ui.rotation = LV_DISP_ROT_180;
			break;
		case 270:
			ui.rotation = LV_DISP_ROT_270;
			break;
		default:
			log_error("Invalid screen rotation %d, value 0, 90, 180 or 270\n", screen_rotation);
			exit(-1);
			break;
	};
	lv_disp_set_rotation(ui.display, ui.rotation);

	/* Init ui style */
	ret = _init_defaults_style();
	fail_if_negative(ret, -4, "ui_style_init failed, return: %d\n", ret);

	/* Create a thread to tell lvgl the elapsed time */
	ret = pthread_create(&ui.tick_thread, NULL, &tick_thread_handler, NULL);
	fail_if_negative(ret, -5, "Create lvgl tick thread failed, return: %d\n", ret);

	/* Create a thread to handle lvgl drawing */
	ret = pthread_create(&ui.draw_thread, NULL, &draw_thread_handler, NULL);
	fail_if_negative(ret, -6, "Create lvgl draw thread failed, return: %d\n", ret);

	/* Create a thread to handle lvgl drawing */
	ret = pthread_create(&ui.screen_thread, NULL, &screen_thread_handler, NULL);
	fail_if_negative(ret, -7, "Create screen manager thread failed, return: %d\n", ret);

	/* Wait 150ms to let thread settle */
	usleep(150*1000);

	/* Display the main screen */
	ret = _push_next_screen_in_fifo(E_MAIN_SCREEN);
	fail_if_negative(ret, -8, "_push_next_screen_in_fifo failed, return %d\n", ret);

	/* Mark the ui module as initialized */
	ui.is_initialized = true;

	return 0;
}
