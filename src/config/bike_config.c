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
#include "libconfig_helper.h"
#include "common_config.h"
#include "bike_config.h"

static struct {
	bool is_initialized;
	char brand[CONFIG_VALUE_STRING_LENGHT];
	char model[CONFIG_VALUE_STRING_LENGHT];
	char name[CONFIG_VALUE_STRING_LENGHT];
	int weight;
	E_bike_type type;
	int wheel_size; //mm
} bike_conf = {
	.is_initialized = false,
};

int bike_config_init(void)
{
	fail_if_true(bike_conf.is_initialized, -1, "Error: bike_conf is already initialized\n");
	int ret = 0;

	/* Get all the config from the file */
	ret = libconfig_helper_get_string(BIKE_CONF_FILE_PATH, "brand", bike_conf.brand, sizeof(bike_conf.brand));
	fail_if_negative(ret, -2, "Error: getting bike brand conf failed\n");
	ret = libconfig_helper_get_string(BIKE_CONF_FILE_PATH, "model", bike_conf.model, sizeof(bike_conf.model));
	fail_if_negative(ret, -3, "Error: getting bike model conf failed\n");
	ret = libconfig_helper_get_string(BIKE_CONF_FILE_PATH, "name", bike_conf.name, sizeof(bike_conf.name));
	fail_if_negative(ret, -4, "Error: getting bike name conf failed\n");
	ret = libconfig_helper_get_int(BIKE_CONF_FILE_PATH, "weight", &bike_conf.weight);
	fail_if_negative(ret, -5, "Error: getting bike weight conf failed\n");
	ret = libconfig_helper_get_int(BIKE_CONF_FILE_PATH, "type", (int*)&bike_conf.type);
	fail_if_negative(ret, -6, "Error: getting bike type conf failed\n");
	ret = libconfig_helper_get_int(BIKE_CONF_FILE_PATH, "wheel_size", &bike_conf.wheel_size);
	fail_if_negative(ret, -7, "Error: getting bike wheel_size conf failed\n");

	bike_conf.is_initialized = true;
	return 0;
}

int bike_config_get_brand(char *buff, int size)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");
	fail_if_null(buff, -2, "Error: buff is null\n");
	fail_if_negative_or_zero(size, -3, "Error: size is negative or zero\n");

	if(size < CONFIG_VALUE_STRING_LENGHT)
	{
		log_warn("strncpy: truncating character sequence\n");
	}

	strncpy(buff, bike_conf.brand, size);
	buff[size-1]='\0';

	return 0;
}

int bike_config_get_model(char *buff, int size)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");
	fail_if_null(buff, -2, "Error: buff is null\n");
	fail_if_negative_or_zero(size, -3, "Error: size is negative or zero\n");

	if(size < CONFIG_VALUE_STRING_LENGHT)
	{
		log_warn("strncpy: truncating character sequence\n");
	}

	strncpy(buff, bike_conf.model, size);
	buff[size-1]='\0';

	return 0;
}

int bike_config_get_name(char *buff, int size)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");
	fail_if_null(buff, -2, "Error: buff is null\n");
	fail_if_negative_or_zero(size, -3, "Error: size is negative or zero\n");

	if(size < CONFIG_VALUE_STRING_LENGHT)
	{
		log_warn("strncpy: truncating character sequence\n");
	}

	strncpy(buff, bike_conf.name, size);
	buff[size-1]='\0';

	return 0;
}

int bike_config_get_weight(void)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");

	return bike_conf.weight;
}

E_bike_type bike_config_get_type(void)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");

	return bike_conf.type;
}

int bike_config_get_wheel_size(void)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");

	return bike_conf.wheel_size;
}

int bike_config_set_brand(const char *brand)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");
	fail_if_null(brand, -2, "Error: brand is null\n");

	int ret = 0;

	ret = libconfig_helper_set_string(BIKE_CONF_FILE_PATH, "brand", brand);
	fail_if_negative(ret, -3, "Error: libconfig_helper_set_string failed, return: %d\n", ret);

	strncpy(bike_conf.brand, brand, sizeof(bike_conf.brand)-1);

	return 0;
}

int bike_config_set_model(const char *model)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");
	fail_if_null(model, -2, "Error: model is null\n");

	int ret = 0;

	ret = libconfig_helper_set_string(BIKE_CONF_FILE_PATH, "model", model);
	fail_if_negative(ret, -3, "Error: libconfig_helper_set_string failed, return: %d\n", ret);

	strncpy(bike_conf.model, model, sizeof(bike_conf.model)-1);

	return 0;
}

int bike_config_set_name(const char *name)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");
	fail_if_null(name, -2, "Error: name is null\n");

	int ret = 0;

	ret = libconfig_helper_set_string(BIKE_CONF_FILE_PATH, "name", name);
	fail_if_negative(ret, -3, "Error: libconfig_helper_set_string failed, return: %d\n", ret);

	strncpy(bike_conf.name, name, sizeof(bike_conf.name)-1);

	return 0;
}

int bike_config_set_weight(const int weight)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");

	int ret = 0;

	ret = libconfig_helper_set_int(BIKE_CONF_FILE_PATH, "weight", weight);
	fail_if_negative(ret, -2, "Error: libconfig_helper_set_int failed, return %d\n", ret);

	bike_conf.weight = weight;

	return 0;
}

int bike_config_set_type(const E_bike_type type)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");

	int ret = 0;

	ret = libconfig_helper_set_int(BIKE_CONF_FILE_PATH, "type", type);
	fail_if_negative(ret, -2, "Error: libconfig_helper_set_int failed, return %d\n", ret);

	bike_conf.type = type;

	return 0;
}

int bike_config_set_wheel_size(const int wheel_size)
{
	fail_if_false(bike_conf.is_initialized, -1, "Error: bike_conf is not initialized\n");

	int ret = 0;

	ret = libconfig_helper_set_int(BIKE_CONF_FILE_PATH, "wheel_size", wheel_size);
	fail_if_negative(ret, -2, "Error: libconfig_helper_set_int failed, return %d\n", ret);

	bike_conf.wheel_size = wheel_size;

	return 0;
}
