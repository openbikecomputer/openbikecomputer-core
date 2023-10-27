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

#ifndef _RIDER_CONFIG_
#define _RIDER_CONFIG_

int rider_config_init(void);

int rider_config_get_name(char *buff, int size);
int rider_config_get_first_name(char *buff, int size);
int rider_config_get_age(void);
int rider_config_get_weight(void);
int rider_config_get_height(void);

int rider_config_set_name(const char *name);
int rider_config_set_first_name(const char *first_name);
int rider_config_set_age(const int age);
int rider_config_set_weight(const int weight);
int rider_config_set_height(const int height);


#endif //_RIDER_CONFIG_
