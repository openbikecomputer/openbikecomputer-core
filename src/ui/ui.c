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

#include "ui.h"

#if DISPLAY_BACKEND == WAYLAND
#include <wayland.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <lvgl.h>
#include <semaphore.h>

#include "main_screen.h"
#include "data_screen.h"
#include "navigation_screen.h"
#include "profile_screen.h"
#include "rider_screen.h"
#include "bike_screen.h"
#include "results_screen.h"
#include "routes_screen.h"
#include "settings_screen.h"

#include "fifo.h"
#include "log.h"
#include "system.h"

#define TOPBAR_SIZE ((ui_get_resolution_ver() / 10))
#define TOPBAR_TIMER_DELAY (1000) //ms
#define SCREEN_FIFO_DEPTH 8
#define STR_TIME_SIZE (8)

LV_IMAGE_DECLARE(mouse_img);

static struct {
	int (*enter)(lv_obj_t * screen);
	int (*exit)(void);
	bool top_bar_visible; /* display or not the top bar */
	bool top_bar_has_back_button; /* display or not the back button in the top bar */
	E_screen_id top_bar_has_back_button_screen; /* screen id where the back button lead */
} screen_table[E_SCREEN_ID_MAX] = {
	[E_MAIN_SCREEN]           = {.enter = &main_screen_enter,            .exit = &main_screen_exit,           .top_bar_visible = true,  .top_bar_has_back_button = false, .top_bar_has_back_button_screen = 0},
	[E_DATA_SCREEN]           = {.enter = &data_screen_enter,            .exit = &data_screen_exit,           .top_bar_visible = true,  .top_bar_has_back_button = true,  .top_bar_has_back_button_screen = E_MAIN_SCREEN},
	[E_NAVIGATION_SCREEN]     = {.enter = &navigation_screen_enter,      .exit = &navigation_screen_exit,     .top_bar_visible = false, .top_bar_has_back_button = false, .top_bar_has_back_button_screen = 0},
	[E_RESULTS_SCREEN]        = {.enter = &results_screen_enter,         .exit = &results_screen_exit,        .top_bar_visible = true,  .top_bar_has_back_button = true,  .top_bar_has_back_button_screen = E_MAIN_SCREEN},
	[E_ROUTES_SCREEN]         = {.enter = &routes_screen_enter,          .exit = &routes_screen_exit,         .top_bar_visible = true,  .top_bar_has_back_button = true,  .top_bar_has_back_button_screen = E_MAIN_SCREEN},
	[E_PROFILE_SCREEN]        = {.enter = &profile_screen_enter,         .exit = &profile_screen_exit,        .top_bar_visible = true,  .top_bar_has_back_button = true,  .top_bar_has_back_button_screen = E_MAIN_SCREEN},
	[E_RIDER_SCREEN]          = {.enter = &rider_screen_enter,           .exit = &rider_screen_exit,          .top_bar_visible = false, .top_bar_has_back_button = false, .top_bar_has_back_button_screen = 0},
	[E_BIKE_SCREEN]           = {.enter = &bike_screen_enter,            .exit = &bike_screen_exit,           .top_bar_visible = false, .top_bar_has_back_button = false, .top_bar_has_back_button_screen = 0},
	[E_SETTINGS_SCREEN]       = {.enter = &settings_screen_enter,        .exit = &settings_screen_exit,       .top_bar_visible = true,  .top_bar_has_back_button = true,  .top_bar_has_back_button_screen = E_MAIN_SCREEN},
};

typedef struct {
	lv_obj_t *bar;
	lv_obj_t *back_btn;
	lv_obj_t *back_btn_label;
	lv_obj_t *label;
	lv_timer_t *timer;
} T_topbar;

static struct {
	bool is_initialized;
	/* display settings */
	int resolution_hor;
	int resolution_ver;
	lv_display_rotation_t rotation;

	/* lvgl display */
	lv_display_t *display;
	lv_obj_t *virt_screen;
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

	/* top bar */
	T_topbar topbar;
} ui = {
	.is_initialized = false,
	.lvgl_mutex = PTHREAD_MUTEX_INITIALIZER,
	.actual_screen = E_MAIN_SCREEN,
	.previous_screen = E_MAIN_SCREEN,
	.topbar = {0},
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

static void _topbar_timer_handler(lv_timer_t * timer)
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
	//lv_label_set_text_fmt(label, LV_SYMBOL_GPS " " LV_SYMBOL_WIFI " " LV_SYMBOL_BATTERY_FULL " %s", str);
	lv_label_set_text_fmt(label, LV_SYMBOL_BATTERY_FULL " %s", str);
}

static void _topbar_back_btn_handler(lv_event_t *event)
{
	int *user_data = lv_event_get_user_data(event);
	E_screen_id next_screen = *user_data;

	ui_change_screen(next_screen);
}

static int _create_topbar(bool back_button_visible, E_screen_id *back_screen)
{
	int ret = 0;

	/* Create the top bar */
	ui.topbar.bar = lv_obj_create(lv_scr_act());
	lv_obj_set_size(ui.topbar.bar, ui_get_resolution_hor(), TOPBAR_SIZE);
	lv_obj_align(ui.topbar.bar, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_add_style(ui.topbar.bar, &ui.default_style, 0);

	/* Create the top bar back button if needed */
	if(back_button_visible)
	{
		/* Create back button */
		ui.topbar.back_btn = lv_btn_create(ui.topbar.bar);
		lv_obj_add_event_cb(ui.topbar.back_btn, &_topbar_back_btn_handler, LV_EVENT_CLICKED, (void*)back_screen);
		lv_obj_set_size(ui.topbar.back_btn, lv_pct(35), lv_pct(90));
		lv_obj_align(ui.topbar.back_btn, LV_ALIGN_LEFT_MID, 0, 0);

		/* Put the text in the back button label */
		ui.topbar.back_btn_label = lv_label_create(ui.topbar.back_btn);
		lv_label_set_text(ui.topbar.back_btn_label, "< Back");
		lv_obj_center(ui.topbar.back_btn_label);

		/* Change style of text */
		lv_obj_set_style_text_font(ui.topbar.back_btn_label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
	}

	/* Fill the top bar with widgets */
	ui.topbar.label = lv_label_create(ui.topbar.bar);
	char str[STR_TIME_SIZE];
	ret = _get_local_time(str, sizeof(str));
	fail_if_negative(ret, -2, "_get_local_time failed, return %d\n", ret);
	//lv_label_set_text_fmt(ui.topbar.label, LV_SYMBOL_GPS " " LV_SYMBOL_WIFI " " LV_SYMBOL_BATTERY_FULL " %s", str);
	lv_label_set_text_fmt(ui.topbar.label, LV_SYMBOL_BATTERY_FULL " %s", str);
    lv_obj_align(ui.topbar.label, LV_ALIGN_RIGHT_MID, 0, 0);

	/* Create lvgl timer that update the bar each secondes */
	ui.topbar.timer = lv_timer_create(&_topbar_timer_handler, TOPBAR_TIMER_DELAY, ui.topbar.label);
	lv_timer_set_repeat_count(ui.topbar.timer, -1); // repeat indefinitly

	/* Disable scrollbar in topbar */
	lv_obj_set_scrollbar_mode(ui.topbar.bar, LV_SCROLLBAR_MODE_OFF);

	return 0;
}

static int _destroy_topbar(void)
{
	if(ui.topbar.timer != NULL)
	{
		lv_timer_del(ui.topbar.timer);
		ui.topbar.timer = NULL;
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
		/* Destroy the top bar before cleaning the display */
		_destroy_topbar();
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
		 * of the topbar if the top bar is needed, if not the size of the
		 * fullscreen.
		 * Each screen enter handler will use that to create the children objects.
		 * */
		pthread_mutex_lock(&ui.lvgl_mutex);

		/* Create the virtual screen object */
		ui.virt_screen = lv_obj_create(lv_scr_act());

		/* Remove all style from virt screen */
		lv_obj_remove_style_all(ui.virt_screen);

		if(screen_table[next].top_bar_visible)
		{
			/* Bar is visible, resize the virtual screen object to the correct dimensions */
			lv_obj_set_size(ui.virt_screen, ui_get_resolution_hor(), ui_get_resolution_ver() - TOPBAR_SIZE);
			lv_obj_align(ui.virt_screen, LV_ALIGN_TOP_MID, 0, TOPBAR_SIZE);

			/* Create the top bar in the same display */
			ret = _create_topbar(screen_table[next].top_bar_has_back_button, &screen_table[next].top_bar_has_back_button_screen);
			if(ret < 0)
			{
				log_error("_create_topbar failed, return: %d\n", ret);
				exit(-1);
			}
		}
		else
		{
			/* screen were the topbar is not displayed */
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
	log_error("LVGL draw thread exit, kill the application\n");
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
#if DISPLAY_BACKEND == WAYLAND
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
#else
	while(1)
	{
		pthread_mutex_lock(&ui.lvgl_mutex);
		lv_timer_handler();
		pthread_mutex_unlock(&ui.lvgl_mutex);
		usleep(LVGL_TIMER_HANDLER_RATE * 1000);
	}
#endif

	/* If we are here something wrong happen, kill the application */
	log_error("LVGL draw thread exit, kill the application\n");
	exit(-1);

	return NULL;
}

static int _init_defaults_style(void)
{
	/* Screen widget default style */
	lv_style_init(&ui.default_style);
	lv_style_set_text_font(&ui.default_style, &DEFAULT_FONT);

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

	/* Init lvgl lib */
	lv_init();

	ret = fifo_create(&ui.screen_fifo, sizeof(int), SCREEN_FIFO_DEPTH);
	fail_if_negative(ret, -2, "fifo_create fail, return: %d\n", ret);

	log_debug("display %dx%d, rotation: %d\n", resolution_hor, resolution_ver, screen_rotation);

	/* Create a display */
	ui.resolution_hor = resolution_hor;
	ui.resolution_ver = resolution_ver;

#if DISPLAY_BACKEND == WAYLAND
	lv_wayland_init();
	ui.display = lv_wayland_create_window(ui.resolution_hor, ui.resolution_ver, "openbikecomputer", NULL /*close_cb*/);
	lv_wayland_window_set_fullscreen(ui.display, true);
#elif DISPLAY_BACKEND == SDL_X11
	ui.display = lv_sdl_window_create(ui.resolution_hor, ui.resolution_ver);
#elif DISPLAY_BACKEND == X11
	ui.display = lv_x11_window_create("openbikecomputer", ui.resolution_hor, ui.resolution_ver);
	lv_x11_inputs_create(ui.display, &mouse_img);
#elif DISPLAY_BACKEND == FRAMEBUFFER
	ui.display = lv_linux_fbdev_create();
	lv_linux_fbdev_set_file(ui.display, "/dev/fb0");
#else
	#error "DISPLAY_BACKEND is not set (choice: WAYLAND, SDL_X11, X11 or FRAMEBUFFER)"
#endif

	fail_if_null(ui.display, -3, "lv_wayland_create_window return NULL\n");

	ui.rotation = LV_DISPLAY_ROTATION_0;
	switch(screen_rotation)
	{
		case 0:
			ui.rotation = LV_DISPLAY_ROTATION_0;
			break;
		case 90:
			ui.rotation = LV_DISPLAY_ROTATION_90;
			break;
		case 180:
			ui.rotation = LV_DISPLAY_ROTATION_180;
			break;
		case 270:
			ui.rotation = LV_DISPLAY_ROTATION_270;
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

	/* Display the main screen */
	ret = _push_next_screen_in_fifo(E_MAIN_SCREEN);
	fail_if_negative(ret, -8, "_push_next_screen_in_fifo failed, return %d\n", ret);

	/* Mark the ui module as initialized */
	ui.is_initialized = true;

	return 0;
}
