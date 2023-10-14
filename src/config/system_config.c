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
#include "system_config.h"

static struct {
	bool is_initialized;
	char devicename[CONFIG_VALUE_STRING_LENGHT];
	int brightness;
} system_conf = {
	.is_initialized = false,
};

int system_config_init(void)
{
	fail_if_true(system_conf.is_initialized, -1, "Error: system_conf is already initialized\n");
	int ret = 0;

	/* Get all the config from the file */
	ret = libconfig_helper_get_string(SYSTEM_CONF_FILE_PATH, "devicename", system_conf.devicename, sizeof(system_conf.devicename));
	fail_if_negative(ret, -1, "Error: getting system devicename conf failed\n");
	ret = libconfig_helper_get_int(SYSTEM_CONF_FILE_PATH, "brightness", &system_conf.brightness);
	fail_if_negative(ret, -2, "Error: getting system brightness conf failed\n");

	system_conf.is_initialized = true;
	return 0;
}

int system_config_get_brightness(void)
{
	fail_if_false(system_conf.is_initialized, -1, "Error: system_conf is not initialized\n");

	return system_conf.brightness;
}

int system_config_get_device_name(char *name, int size)
{
	fail_if_false(system_conf.is_initialized, -1, "Error: system_conf is not initialized\n");
	fail_if_null(name, -2, "Error: name is null\n");
	fail_if_negative_or_zero(size, -3, "Error: size is negative or zero\n");

	if(size < CONFIG_VALUE_STRING_LENGHT)
	{
		log_warn("strncpy: truncating character sequence\n");
	}

	strncpy(name, system_conf.devicename, size);
	name[size-1]='\0';

	return 0;
}
