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
#ifndef _UI_HEADER_
#define _UI_HEADER_

typedef enum {
	E_MAIN_SCREEN = 0,
	E_RIDER_CONF_SCREEN,
	E_BIKE_CONF_SCREEN,
	E_SYSTEM_CONF_SCREEN,
	E_USER_CONF_SCREEN,
	E_DATA_SCREEN,
	E_SCREEN_ID_MAX /*must be last*/
} E_screen_id;


int ui_init(int resolution_hor, int resolution_ver, int screen_rotation);
int ui_change_screen(E_screen_id next);
int ui_go_to_previous_screen(void);
int ui_get_resolution_hor(void);
int ui_get_resolution_ver(void);

#endif //_UI_HEADER_
