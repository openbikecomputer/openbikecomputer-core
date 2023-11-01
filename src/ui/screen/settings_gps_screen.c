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

#include "log.h"
#include "lvgl_helper.h"
#include "settings_gps_screen.h"

static T_lv_btn back_btn;

int settings_gps_screen_enter(void)
{
	int ret = 0;
	ret = lvgl_helper_create_button(&back_btn, BACK_BUTTON_SIZE_X, BACK_BUTTON_SIZE_Y, BACK_BUTTON_ALIGN, BACK_BUTTON_POS_X, BACK_BUTTON_POS_Y, BACK_BUTTON_TEXT, &lvgl_helper_back_button_event_handler);
	fail_if_negative(ret, -1, "Error: lvgl_helper_create_button failed, return %d\n");
	return 0;
}
int settings_gps_screen_exit(void)
{
	return 0;
}
