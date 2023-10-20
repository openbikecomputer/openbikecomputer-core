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

#ifndef _SCREEN_MANAGER_HEADER_
#define _SCREEN_MANAGER_HEADER_

typedef enum {
	E_MAIN_SCREEN = 0,
	E_RIDER_CONF_SCREEN,
	E_BIKE_CONF_SCREEN,
	E_SYSTEM_CONF_SCREEN,
	E_DATA_SCREEN,
	E_SCREEN_ID_MAX /*must be last*/
} E_screen_id;

int screen_manager_init(void);
int screen_manager_change_screen(E_screen_id next);

#endif //_SCREEN_MANAGER_HEADER_
