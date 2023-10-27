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

#include <libconfig.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "system.h"
#include "log.h"
#include "libconfig_helper.h"

int libconfig_helper_get_int(const char *file, const char *conf, int *value)
{
	fail_if_null(file, -1, "Error: file is null\n");
	fail_if_null(conf, -2, "Error: conf is null\n");
	fail_if_null(value, -3, "Error: value is null\n");

	int ret = 0;
	config_t cfg;
	config_init(&cfg);

	/* Read the file. If there is an error, report it and exit. */
	if(!config_read_file(&cfg, file))
	{
		config_destroy(&cfg);
		fail(-4 , "Read configuration file %s fail\n", file);
	}

	ret = config_lookup_int(&cfg, conf, value);
	if(ret != CONFIG_TRUE)
	{
		config_destroy(&cfg);
		fail(-5 ,"conf %s not found in configuration file %s\n", conf, file);
	}

	/* Cleanup before exiting*/
	config_destroy(&cfg);

	return 0;
}

int libconfig_helper_get_string(const char *file, const char *conf, char *buff, int size)
{
	fail_if_null(file, -1, "Error: file is null\n");
	fail_if_null(conf, -2, "Error: conf is null\n");
	fail_if_null(buff, -3, "Error: buff is null\n");
	fail_if_negative_or_zero(size, -4, "Error: size is not valid\n");

	int ret = 0;
	config_t cfg;
	config_init(&cfg);

	/* Read the file. If there is an error, report it and exit. */
	if(!config_read_file(&cfg, file))
	{
		config_destroy(&cfg);
		fail(-5, "Error: read configuration file %s fail\n", file);
	}

	const char* str;
	ret = config_lookup_string(&cfg, conf, &str);
	if(ret != CONFIG_TRUE)
	{
		config_destroy(&cfg);
		fail(-7, "conf %s not found in configuration file %s\n", conf, file);
	}

	/* Copy the string in the array provided by the user */
	strncpy(buff, str, size-1);
	buff[size-1] = '\0';

	/* Cleanup before exiting*/
	config_destroy(&cfg);

	return 0;
}

int libconfig_helper_set_int(const char *file, const char *conf, const int value)
{
	fail_if_null(file, -1, "Error: file is null\n");
	fail_if_null(conf, -2, "Error: conf is null\n");

	int ret = 0;
	config_t cfg;
	config_init(&cfg);

	/* Read the file. If there is an error, report it and exit. */
	if(!config_read_file(&cfg, file))
	{
		config_destroy(&cfg);
		fail(-3, "Error: Reading file failed\n");
	}

	/* Get the settings */
	config_setting_t *setting = config_lookup(&cfg, conf);
	if(setting == NULL)
	{
		config_destroy(&cfg);
		fail(-4, "Error: lookup settings failed\n");
	}

	ret = config_setting_set_int(setting, value);
	if(ret != CONFIG_TRUE)
	{
		config_destroy(&cfg);
		fail(-5, "Error: setting int failed\n");
	}

	ret = config_write_file(&cfg, file);
	if(ret != CONFIG_TRUE)
	{
		config_destroy(&cfg);
		fail(-6, "Error: write file failed\n");
	}

	/* Cleanup before exiting*/
	return 0;
}

int libconfig_helper_set_string(const char *file, const char *conf, const char *value)
{
	fail_if_null(file, -1, "Error: file is null\n");
	fail_if_null(conf, -2, "Error: conf is null\n");
	fail_if_null(value, -3, "Error: value is null\n");

	int ret = 0;
	config_t cfg;
	config_init(&cfg);

	/* Read the file. If there is an error, report it and exit. */
	if(!config_read_file(&cfg, file))
	{
		config_destroy(&cfg);
		fail(-4, "Error: Reading file failed\n");
	}

	/* Get the settings */
	config_setting_t *setting = config_lookup(&cfg, conf);
	if(setting == NULL)
	{
		config_destroy(&cfg);
		fail(-5, "Error: lookup settings failed\n");
	}

	ret = config_setting_set_string(setting, value);
	if(ret != CONFIG_TRUE)
	{
		config_destroy(&cfg);
		fail(-6, "Error: setting int failed\n");
	}

	ret = config_write_file(&cfg, file);
	if(ret != CONFIG_TRUE)
	{
		config_destroy(&cfg);
		fail(-7, "Error: write file failed\n");
	}

	/* Cleanup before exiting*/
	return 0;
}
