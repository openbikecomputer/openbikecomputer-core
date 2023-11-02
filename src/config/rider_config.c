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

#include <string.h>
#include "log.h"
#include "system.h"
#include "common_config.h"
#include "libconfig_helper.h"
#include "rider_config.h"

static struct {
	bool is_initialized;
	char name[CONFIG_VALUE_STRING_LENGHT];
	char first_name[CONFIG_VALUE_STRING_LENGHT];
	int age;
	int weight;
	int height;
} rider_conf = {
	.is_initialized = false,
};

int rider_config_init(void)
{
	fail_if_true(rider_conf.is_initialized, -1, "rider_conf is already initialized\n");
	int ret = 0;

	/* Get all the config from the file */
	ret = libconfig_helper_get_string(RIDER_CONF_FILE_PATH, "name", rider_conf.name, sizeof(rider_conf.name));
	fail_if_negative(ret, -2, "getting rider name conf failed\n");
	ret = libconfig_helper_get_string(RIDER_CONF_FILE_PATH, "first_name", rider_conf.first_name, sizeof(rider_conf.first_name));
	fail_if_negative(ret, -3, "getting rider first_name conf failed\n");
	ret = libconfig_helper_get_int(RIDER_CONF_FILE_PATH, "weight", &rider_conf.weight);
	fail_if_negative(ret, -4, "getting rider weight conf failed\n");
	ret = libconfig_helper_get_int(RIDER_CONF_FILE_PATH, "age", &rider_conf.age);
	fail_if_negative(ret, -5, "getting rider age conf failed\n");
	ret = libconfig_helper_get_int(RIDER_CONF_FILE_PATH, "height", &rider_conf.height);
	fail_if_negative(ret, -6, "getting rider height conf failed\n");

	rider_conf.is_initialized = true;
	return 0;
}

int rider_config_get_name(char *buff, int size)
{
	fail_if_false(rider_conf.is_initialized, -1, "rider_conf is not initialized\n");
	fail_if_null(buff, -2, "buff is null\n");
	fail_if_negative_or_zero(size, -3, "size is negative or zero\n");

	if(size < CONFIG_VALUE_STRING_LENGHT)
	{
		log_warn("strncpy: truncating character sequence\n");
	}

	strncpy(buff, rider_conf.name, size);
	buff[size-1]='\0';

	return 0;
}

int rider_config_get_first_name(char *buff, int size)
{
	fail_if_false(rider_conf.is_initialized, -1, "rider_conf is not initialized\n");
	fail_if_null(buff, -2, "buff is null\n");
	fail_if_negative_or_zero(size, -3, "size is negative or zero\n");

	if(size < CONFIG_VALUE_STRING_LENGHT)
	{
		log_warn("strncpy: truncating character sequence\n");
	}

	strncpy(buff, rider_conf.first_name, size);
	buff[size-1]='\0';

	return 0;
}

int rider_config_get_age(void)
{
	fail_if_false(rider_conf.is_initialized, -1, "rider_conf is not initialized\n");

	return rider_conf.age;
}

int rider_config_get_weight(void)
{
	fail_if_false(rider_conf.is_initialized, -1, "rider_conf is not initialized\n");

	return rider_conf.weight;
}

int rider_config_get_height(void)
{
	fail_if_false(rider_conf.is_initialized, -1, "rider_conf is not initialized\n");

	return rider_conf.height;
}

int rider_config_set_name(const char *name)
{
	fail_if_false(rider_conf.is_initialized, -1, "rider_conf is not initialized\n");
	fail_if_null(name, -2, "name is null\n");

	int ret = 0;

	ret = libconfig_helper_set_string(RIDER_CONF_FILE_PATH, "name", name);
	fail_if_negative(ret, -3, "libconfig_helper_set_string failed, return: %d\n", ret);

	strncpy(rider_conf.name, name, sizeof(rider_conf.name)-1);

	return 0;
}

int rider_config_set_first_name(const char *first_name)
{
	fail_if_false(rider_conf.is_initialized, -1, "rider_conf is not initialized\n");
	fail_if_null(first_name, -2, "first_name is null\n");

	int ret = 0;

	ret = libconfig_helper_set_string(RIDER_CONF_FILE_PATH, "first_name", first_name);
	fail_if_negative(ret, -3, "libconfig_helper_set_string failed, return: %d\n", ret);

	strncpy(rider_conf.first_name, first_name, sizeof(rider_conf.first_name)-1);

	return 0;
}

int rider_config_set_age(const int age)
{
	fail_if_false(rider_conf.is_initialized, -1, "rider_conf is not initialized\n");

	int ret = 0;

	ret = libconfig_helper_set_int(RIDER_CONF_FILE_PATH, "age", age);
	fail_if_negative(ret, -3, "libconfig_helper_set_string failed, return: %d\n", ret);

	rider_conf.age = age;

	return 0;
}

int rider_config_set_weight(const int weight)
{
	fail_if_false(rider_conf.is_initialized, -1, "rider_conf is not initialized\n");

	int ret = 0;

	ret = libconfig_helper_set_int(RIDER_CONF_FILE_PATH, "weight", weight);
	fail_if_negative(ret, -3, "libconfig_helper_set_string failed, return: %d\n", ret);

	rider_conf.weight = weight;

	return 0;
}

int rider_config_set_height(const int height)
{
	fail_if_false(rider_conf.is_initialized, -1, "rider_conf is not initialized\n");

	int ret = 0;

	ret = libconfig_helper_set_int(RIDER_CONF_FILE_PATH, "height", height);
	fail_if_negative(ret, -3, "libconfig_helper_set_string failed, return: %d\n", ret);

	rider_conf.height = height;

	return 0;
}
