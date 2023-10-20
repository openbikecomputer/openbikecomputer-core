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

#include "log.h"
#include "system.h"
#include "screen_manager.h"

static struct {
	bool is_initialized;
	pthread_t tick_thread;
	pthread_t draw_thread;
	lv_disp_t *display;
} ui = {
	.is_initialized = false,
};

static void * tick_thread_handler(void *data)
{
	/* The tick thread tell the LVGL lib the time elapsed */
	while(1)
	{
		lv_tick_inc(LVGL_REFRESH_TICK_RATE);
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
	log_error("LVGL draw thread exit, kill the application\n");
	exit(-1);

	return NULL;
}

int ui_init(void)
{
	int ret;

	fail_if_true(ui.is_initialized, -1, "Error: ui is already initialized\n");

	/* Init lvgl lib and wayland driver */
	lv_init();
	lv_wayland_init();

	/* Create a display */
	ui.display = lv_wayland_create_window(SCREEN_HOR_SIZE, SCREEN_VER_SIZE, "openbikecomputer", NULL /*close_cb*/);
	fail_if_null(ui.display, -1, "Error: lv_wayland_create_window return NULL\n");
	lv_disp_set_rotation(ui.display, SCREEN_ROTATION);
	lv_wayland_window_set_fullscreen(ui.display, true);

	/* Create a thread to tell lvgl the elapsed time */
	ret = pthread_create(&ui.tick_thread, NULL, &tick_thread_handler, NULL);
	fail_if_negative(ret, -1, "Error: Create lvgl tick thread failed, return: %d\n", ret);

	/* Create a thread to handle lvgl drawing */
	ret = pthread_create(&ui.draw_thread, NULL, &draw_thread_handler, NULL);
	fail_if_negative(ret, -2, "Error: Create lvgl draw thread failed, return: %d\n", ret);

	ret = screen_manager_init();
	fail_if_negative(ret, -3, "Error: screen_manager_init failed, return: %d\n", ret);

	/* Mark the ui module as initialized */
	ui.is_initialized = true;

	return 0;
}
