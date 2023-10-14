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

#include "system.h"

static void * tick_thread_handler(void *data)
{
	while(1)
	{
		lv_tick_inc(LVGL_REFRESH_TICK_RATE);
		usleep(LVGL_REFRESH_TICK_RATE * 1000);
	}

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
		time_till_next = lv_wayland_timer_handler();

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
	printf("LVGL draw thread exit, kill the application\n");
	exit(-1);

	return NULL;
}

static int create_initial_ui(void)
{
	lv_obj_t * tv = lv_tileview_create(lv_scr_act());

	/*Tile1: Configuration menu*/
	lv_obj_t * tile1 = lv_tileview_add_tile(tv, 0, 0, LV_DIR_BOTTOM);
	lv_obj_t * label = lv_label_create(tile1);
	lv_label_set_text(label, "Scroll down");
	lv_obj_center(label);


	/*Tile2: Main screen (speed/hour/power/other) */
	lv_obj_t * tile2 = lv_tileview_add_tile(tv, 0, 1, LV_DIR_TOP | LV_DIR_RIGHT);

	lv_obj_t * btn = lv_btn_create(tile2);

	label = lv_label_create(btn);
	lv_label_set_text(label, "Scroll up or right");

	lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_center(btn);

	/*Tile3: Navigation (MAP) */
	lv_obj_t * tile3 = lv_tileview_add_tile(tv, 1, 1, LV_DIR_LEFT);
	lv_obj_t * list = lv_list_create(tile3);
	lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));

	lv_list_add_btn(list, NULL, "One");

	return 0;
}

int ui_init(void)
{
	int ret;
	pthread_t tick_thread, draw_thread;

	/* Init lvgl lib and wayland driver */
	lv_init();
	lv_wayland_init();

	/* Create a display */
	lv_disp_t * disp = lv_wayland_create_window(SCREEN_HOR_SIZE, SCREEN_VER_SIZE, "openbikecomputer", NULL /*close_cb*/);
	lv_disp_set_rotation(disp, LV_DISP_ROT_270);
	lv_wayland_window_set_fullscreen(disp, true);

	/* Create a thread to tell lvgl the elapsed time */
	ret = pthread_create(&tick_thread, NULL, &tick_thread_handler, NULL);
	if(ret != 0)
	{
		printf("Create tick thread failed, return: %d\n", ret);
		return -1;
	}

	/* Create a thread to handle lvgl drawing */
	ret = pthread_create(&draw_thread, NULL, &draw_thread_handler, NULL);
	if(ret != 0)
	{
		printf("Create draw thread failed, return: %d\n", ret);
		return -2;
	}

	ret = create_initial_ui();
	if(ret < 0)
	{
		printf("Create initial ui failed, return: %d\n", ret);
		return -3;
	}

	return 0;
}
