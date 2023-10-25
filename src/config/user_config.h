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

#ifndef _USER_HEADER_
#define _USER_HEADER_

int user_config_init(void);

int user_config_get_brightness(void);
int user_config_get_gps_on(void);
int user_config_get_ant_on(void);
int user_config_get_bluetooth_on(void);
int user_config_get_wifi_on(void);

int user_config_set_brightness(int value);
int user_config_set_gps_on(int value);
int user_config_set_ant_on(int value);
int user_config_set_bluetooth_on(int value);
int user_config_set_wifi_on(int value);

#endif //_USER_HEADER_
