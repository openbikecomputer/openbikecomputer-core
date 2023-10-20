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

#include <unistd.h>
#include <stdlib.h>
#include <lvgl.h>

#include "log.h"
#include "system.h"
#include "screen_manager.h"
#include "main_screen.h"
#include "bike_conf_screen.h"
#include "rider_conf_screen.h"
#include "system_conf_screen.h"
#include "data_screen.h"

static struct {
	int (*enter)(void);
	int (*exit)(void);
} screen_table[E_SCREEN_ID_MAX] = {
	[E_MAIN_SCREEN]        = {.enter = &main_screen_enter, .exit = &main_screen_exit},
	[E_RIDER_CONF_SCREEN]  = {.enter = &rider_conf_screen_enter, .exit = &rider_conf_screen_exit},
	[E_BIKE_CONF_SCREEN]   = {.enter = &bike_conf_screen_enter, .exit = &bike_conf_screen_exit},
	[E_SYSTEM_CONF_SCREEN] = {.enter = &system_screen_enter, .exit = &system_screen_exit},
	[E_DATA_SCREEN]        = {.enter = &data_screen_enter, .exit = &data_screen_exit},
};

static struct {
	bool is_initialized;
	E_screen_id previous;
	E_screen_id actual;
} screen_manager = {
	.is_initialized = false,
	.previous = E_MAIN_SCREEN,
	.actual = E_MAIN_SCREEN,
};


static int _display_next_screen(E_screen_id next)
{
	int ret = 0;

	/* Check if the next screen enter handler is null */
	fail_if_null(screen_table[next].enter, -1, "Error: next screen %d enter handler is null\n", next);

	/* Clean the actual screen before continuing */
	lv_obj_clean(lv_scr_act());

	/* Apply style to screen */

	/* Exit the actual screen, exit handler is not mandatory */
	if(screen_table[screen_manager.actual].exit != NULL)
	{
		ret = screen_table[screen_manager.actual].exit();
		if(ret < 0)
		{
			log_error("Error: exiting screen %d failed, returned: %d\n", screen_manager.actual, ret);
		}
	}

	/* Enter the next screen */
	ret = screen_table[next].enter();
	if(ret < 0)
	{
		log_error("Error: entering screen %d failed, returned: %d\n", next, ret);

		/* Go to the previous screen (failsafe mode) */
		ret = screen_table[screen_manager.actual].enter();
		if(ret < 0)
		{
			/* At this point nothing more can be done, exit the application */
			log_error("Error: entering previous screen %d failed, returned: %d, exit application\n", screen_manager.actual, ret);
			exit(-1);
		}
	}

	/* Actualize the screen manager state */
	screen_manager.previous = screen_manager.actual;
	screen_manager.actual = next;

	log_info("Info: changed screen from %d to %d\n", screen_manager.previous, screen_manager.actual);

	return 0;
}

int screen_manager_init(void)
{
	int ret = 0;

	fail_if_true(screen_manager.is_initialized, -1, "Error: screen_manager is already initialized\n");

	/* Display the main screen */
	ret = _display_next_screen(E_MAIN_SCREEN);
	fail_if_negative(ret, -3, "Error: _display_next_screen failed, return %d\n", ret);


	screen_manager.is_initialized = true;
	return 0;
}

int screen_manager_change_screen(E_screen_id next)
{
	int ret = 0;

	fail_if_false(screen_manager.is_initialized, -1, "Error: screen_manager is not initialized\n");

	if(next < 0 || next > E_SCREEN_ID_MAX)
	{
		fail(-2, "Error: Invalid screen id given\n");
	}

	if(next == screen_manager.actual)
	{
		/* Nothing todo */
		return 0;
	}

	ret = _display_next_screen(next);
	fail_if_negative(ret, -3, "Error: _display_next_screen failed, return %d\n", ret);

	return 0;
}
