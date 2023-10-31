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
#include "bike_conf_screen.h"
#include "rider_conf_screen.h"
#include "system_conf_screen.h"
#include "data_screen.h"
#include "user_conf_screen.h"
#include "navigation_screen.h"
#include "profile_screen.h"
#include "result_screen.h"
#include "route_screen.h"
#include "settings_ant_screen.h"
#include "settings_bluetooth_screen.h"
#include "settings_gps_screen.h"
#include "settings_screen.h"
#include "settings_wifi_screen.h"

#include "fifo.h"
#include "log.h"
#include "system.h"
#include "ui_style.h"
#include "ui.h"

#define SCREEN_FIFO_DEPTH 8
static struct {
	int (*enter)(void);
	int (*exit)(void);
} screen_table[E_SCREEN_ID_MAX] = {
	[E_MAIN_SCREEN]        = {.enter = &main_screen_enter, .exit = &main_screen_exit},
	[E_RIDER_CONF_SCREEN]  = {.enter = &rider_conf_screen_enter, .exit = &rider_conf_screen_exit},
	[E_BIKE_CONF_SCREEN]   = {.enter = &bike_conf_screen_enter, .exit = &bike_conf_screen_exit},
	[E_SYSTEM_CONF_SCREEN] = {.enter = &system_screen_enter, .exit = &system_screen_exit},
	[E_USER_CONF_SCREEN]   = {.enter = &user_conf_screen_enter, .exit = &user_conf_screen_exit},
	[E_DATA_SCREEN]        = {.enter = &data_screen_enter, .exit = &data_screen_exit},
	[E_NAVIGATION_SCREEN]         = {.enter = &navigation_screen_enter, .exit = &navigation_screen_exit},
	[E_RESULT_SCREEN]             = {.enter = &result_screen_enter, .exit = &result_screen_exit},
	[E_ROUTE_SCREEN]              = {.enter = &route_screen_enter, .exit = &route_screen_exit},
	[E_PROFILE_SCREEN]            = {.enter = &profile_screen_enter, .exit = &profile_screen_exit},
	[E_SETTINGS_SCREEN]           = {.enter = &settings_screen_enter, .exit = &settings_screen_exit},
	[E_SETTINGS_GPS_SCREEN]       = {.enter = &settings_gps_screen_enter, .exit = &settings_gps_screen_exit},
	[E_SETTINGS_ANT_SCREEN]       = {.enter = &settings_ant_screen_enter, .exit = &settings_ant_screen_exit},
	[E_SETTINGS_BLUETOOTH_SCREEN] = {.enter = &settings_bluetooth_screen_enter, .exit = &settings_bluetooth_screen_exit},
	[E_SETTINGS_WIFI_SCREEN]      = {.enter = &settings_wifi_screen_enter, .exit = &settings_wifi_screen_exit},
};

static struct {
	bool is_initialized;
	/* display settings */
	int resolution_hor;
	int resolution_ver;
	lv_disp_rot_t rotation;

	/* lvgl display */
	lv_disp_t *display;

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
} ui = {
	.is_initialized = false,
	.lvgl_mutex = PTHREAD_MUTEX_INITIALIZER,
	.actual_screen = E_MAIN_SCREEN,
	.previous_screen = E_MAIN_SCREEN,
};

static int _push_next_screen_in_fifo(E_screen_id next)
{
	int ret = fifo_push(&ui.screen_fifo, (int*)&next);
	fail_if_negative(ret, -1, "Error: fifo_push next screen (%d) failed, return %d\n", next, ret);

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
			log_error("Error: fifo_pop_wait failed, return: %d\n", ret);
			continue;
		}

		/* Clean the actual_screen screen before continuing */
		log_debug("LVGL: Clean actual screen\n");
		pthread_mutex_lock(&ui.lvgl_mutex);
		lv_obj_clean(lv_scr_act());
		pthread_mutex_unlock(&ui.lvgl_mutex);

		/* Exit the actual_screen screen, exit handler is not mandatory */
		if(screen_table[ui.actual_screen].exit != NULL)
		{
			log_debug("Run actual screen exit handler\n");
			pthread_mutex_lock(&ui.lvgl_mutex);
			ret = screen_table[ui.actual_screen].exit();
			pthread_mutex_unlock(&ui.lvgl_mutex);
			if(ret < 0)
			{
				log_error("Error: exiting screen %d failed, returned: %d\n", ui.actual_screen, ret);
			}
		}

		/* Enter the next screen */
		log_debug("Run next screen enter handler\n");
		pthread_mutex_lock(&ui.lvgl_mutex);
		ret = screen_table[next].enter();
		pthread_mutex_unlock(&ui.lvgl_mutex);
		if(ret < 0)
		{
			log_error("Error: entering screen %d failed, returned: %d\n", next, ret);

			/* Go to the previous_screen screen (failsafe mode) */
			pthread_mutex_lock(&ui.lvgl_mutex);
			ret = screen_table[ui.actual_screen].enter();
			pthread_mutex_unlock(&ui.lvgl_mutex);
			if(ret < 0)
			{
				/* At this point nothing more can be done, exit the application */
				log_error("Error: entering previous_screen screen %d failed, returned: %d, exit application\n", ui.actual_screen, ret);
				exit(-1);
			}
			else
			{
				log_error("UI display change failsafe mode did recover an error by loading screen: %d\n", ui.actual_screen);
			}
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

int ui_change_screen(E_screen_id next)
{
	int ret = 0;

	fail_if_false(ui.is_initialized, -1, "Error: ui is not initialized\n");

	if(next < 0 || next > E_SCREEN_ID_MAX)
	{
		fail(-2, "Error: Invalid screen id given\n");
	}

	if(next == ui.actual_screen)
	{
		/* Nothing todo */
		return 0;
	}

	log_info("New screen asked: %d\n", next);

	/* Display the next screen */
	ret = _push_next_screen_in_fifo(next);
	fail_if_negative(ret, -3, "Error: _push_next_screen_in_fifo failed, return %d\n", ret);

	return 0;
}

int ui_go_to_previous_screen(void)
{
	int ret = 0;

	fail_if_false(ui.is_initialized, -1, "Error: ui is not initialized\n");

	/* Display the next screen */
	// TODO unsafe, need mutex
	ret = _push_next_screen_in_fifo(ui.previous_screen);
	fail_if_negative(ret, -3, "Error: _push_next_screen_in_fifo failed, return %d\n", ret);

	return 0;
}

int ui_get_resolution_hor(void)
{
	fail_if_false(ui.is_initialized, -1, "Error: ui is not initialized\n");
	
	return ui.resolution_hor;
}

int ui_get_resolution_ver(void)
{
	fail_if_false(ui.is_initialized, -1, "Error: ui is not initialized\n");
	
	return ui.resolution_ver;
}

int ui_init(int resolution_hor, int resolution_ver, int screen_rotation)
{
	int ret;

	fail_if_true(ui.is_initialized, -1, "Error: ui is already initialized\n");

	/* Init lvgl lib and wayland driver */
	lv_init();
	lv_wayland_init();

	ret = fifo_create(&ui.screen_fifo, sizeof(int), SCREEN_FIFO_DEPTH);
	fail_if_negative(ret, -2, "Error: fifo_create fail, return: %d\n", ret);

	printf("Init display %dx%d, rotation: %d\n", resolution_hor, resolution_ver, screen_rotation);

	/* Create a display */
	ui.resolution_hor = resolution_hor;
	ui.resolution_ver = resolution_ver;
	ui.display = lv_wayland_create_window(ui.resolution_hor, ui.resolution_ver, "openbikecomputer", NULL /*close_cb*/);
	fail_if_null(ui.display, -3, "Error: lv_wayland_create_window return NULL\n");

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
	ret = ui_style_init();
	fail_if_negative(ret, -4, "Error: ui_style_init failed, return: %d\n", ret);

	/* Create a thread to tell lvgl the elapsed time */
	ret = pthread_create(&ui.tick_thread, NULL, &tick_thread_handler, NULL);
	fail_if_negative(ret, -5, "Error: Create lvgl tick thread failed, return: %d\n", ret);

	/* Create a thread to handle lvgl drawing */
	ret = pthread_create(&ui.draw_thread, NULL, &draw_thread_handler, NULL);
	fail_if_negative(ret, -6, "Error: Create lvgl draw thread failed, return: %d\n", ret);

	/* Create a thread to handle lvgl drawing */
	ret = pthread_create(&ui.screen_thread, NULL, &screen_thread_handler, NULL);
	fail_if_negative(ret, -7, "Error: Create screen manager thread failed, return: %d\n", ret);

	/* Display the main screen */
	ret = _push_next_screen_in_fifo(E_MAIN_SCREEN);
	fail_if_negative(ret, -8, "Error: _push_next_screen_in_fifo failed, return %d\n", ret);

	/* Mark the ui module as initialized */
	ui.is_initialized = true;

	return 0;
}
