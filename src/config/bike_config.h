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

#ifndef _BIKE_HEADER_
#define _BIKE_HEADER_

typedef enum {
	E_ATB = 0,
	E_MTB,
	E_ROAD,
	E_GRAVEL,
	E_TOURING,
	E_SPEED,
	E_OTHER,
} E_bike_type;

int bike_config_init(void);
int bike_config_get_brand(char *buff, int size);
int bike_config_get_model(char *buff, int size);
int bike_config_get_name(char *buff, int size);
int bike_config_get_weight(void);
E_bike_type bike_config_get_type(void);
int bike_config_get_wheel_size(void);

int bike_config_set_brand(const char *buff);
int bike_config_set_model(const char *buff);
int bike_config_set_name(const char *buff);
int bike_config_set_weight(const int weight);
int bike_config_set_type(const E_bike_type type);
int bike_config_set_wheel_size(const int wheel_size);

#endif //_BIKE_HEADER_
