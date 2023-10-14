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
#ifndef _SYSTEM_HEADER_
#define _SYSTEM_HEADER_

/* LVGL configuration */
#define SCREEN_HOR_SIZE (1024)
#define SCREEN_VER_SIZE (800)
#define LVGL_REFRESH_TICK_RATE 30 /*milliseconds*/

/* System configuration file */
#define SYSTEM_CONF_FILE_PATH "/etc/openbikecomputer/system.conf"
#define RIDER_CONF_FILE_PATH "/etc/openbikecomputer/rider.conf"
#define BIKE_CONF_FILE_PATH "/etc/openbikecomputer/bike.conf"

#endif //_SYSTEM_HEADER_
