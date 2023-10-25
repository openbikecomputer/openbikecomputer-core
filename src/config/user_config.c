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
#include "user_config.h"

static struct {
	bool is_initialized;
	int brightness;
	int gps_on;
	int ant_on;
	int bluetooth_on;
	int wifi_on;
} user_conf = {
	.is_initialized = false,
};

int user_config_init(void)
{
	fail_if_true(user_conf.is_initialized, -1, "Error: user_conf is already initialized\n");
	int ret = 0;

	/* Get all the config from the file */
	ret = libconfig_helper_get_int(USER_CONF_FILE_PATH, "brightness", &user_conf.brightness);
	fail_if_negative(ret, -2, "Error: getting user brightness conf failed\n");
	ret = libconfig_helper_get_int(USER_CONF_FILE_PATH, "gps_on", &user_conf.gps_on);
	fail_if_negative(ret, -3, "Error: getting user gps_on conf failed\n");
	ret = libconfig_helper_get_int(USER_CONF_FILE_PATH, "ant_on", &user_conf.ant_on);
	fail_if_negative(ret, -4, "Error: getting user ant_on conf failed\n");
	ret = libconfig_helper_get_int(USER_CONF_FILE_PATH, "bluetooth_on", &user_conf.bluetooth_on);
	fail_if_negative(ret, -5, "Error: getting user bluetooth_on conf failed\n");
	ret = libconfig_helper_get_int(USER_CONF_FILE_PATH, "wifi_on", &user_conf.wifi_on);
	fail_if_negative(ret, -6, "Error: getting user wifi_on conf failed\n");

	user_conf.is_initialized = true;
	return 0;
}

int user_config_get_brightness(void)
{
	fail_if_false(user_conf.is_initialized, -1, "Error: user_conf is not initialized\n");

	return user_conf.brightness;
}

int user_config_get_gps_on(void)
{
	fail_if_false(user_conf.is_initialized, -1, "Error: user_conf is not initialized\n");

	return user_conf.gps_on;
}

int user_config_get_ant_on(void)
{
	fail_if_false(user_conf.is_initialized, -1, "Error: user_conf is not initialized\n");

	return user_conf.ant_on;
}

int user_config_get_bluetooth_on(void)
{
	fail_if_false(user_conf.is_initialized, -1, "Error: user_conf is not initialized\n");

	return user_conf.bluetooth_on;
}

int user_config_get_wifi_on(void)
{
	fail_if_false(user_conf.is_initialized, -1, "Error: user_conf is not initialized\n");

	return user_conf.wifi_on;
}

int user_config_set_brightness(int value)
{
	fail_if_false(user_conf.is_initialized, -1, "Error: user_conf is not initialized\n");

	int ret = 0;

	ret = libconfig_helper_set_int(USER_CONF_FILE_PATH, "brightness", value);
	fail_if_negative(ret, -2, "Error: set brightness failed, return: %d\n", ret);

	user_conf.brightness = value;

	return 0;
}

int user_config_set_gps_on(int value)
{
	fail_if_false(user_conf.is_initialized, -1, "Error: user_conf is not initialized\n");

	int ret = 0;

	ret = libconfig_helper_set_int(USER_CONF_FILE_PATH, "gps_on", value);
	fail_if_negative(ret, -2, "Error: set gps_on failed, return: %d\n", ret);

	user_conf.gps_on = value;

	return 0;
}

int user_config_set_ant_on(int value)
{
	fail_if_false(user_conf.is_initialized, -1, "Error: user_conf is not initialized\n");

	int ret = 0;

	ret = libconfig_helper_set_int(USER_CONF_FILE_PATH, "ant_on", value);
	fail_if_negative(ret, -2, "Error: set ant_on failed, return: %d\n", ret);

	user_conf.ant_on = value;

	return 0;
}

int user_config_set_bluetooth_on(int value)
{
	fail_if_false(user_conf.is_initialized, -1, "Error: user_conf is not initialized\n");

	int ret = 0;

	ret = libconfig_helper_set_int(USER_CONF_FILE_PATH, "bluetooth_on", value);
	fail_if_negative(ret, -2, "Error: set bluetooth_on failed, return: %d\n", ret);

	user_conf.bluetooth_on = value;

	return 0;
}

int user_config_set_wifi_on(int value)
{
	fail_if_false(user_conf.is_initialized, -1, "Error: user_conf is not initialized\n");

	int ret = 0;

	ret = libconfig_helper_set_int(USER_CONF_FILE_PATH, "wifi_on", value);
	fail_if_negative(ret, -2, "Error: set wifi_on failed, return: %d\n", ret);

	user_conf.wifi_on = value;

	return 0;
}
