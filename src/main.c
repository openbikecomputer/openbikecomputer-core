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

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"
#include "ui.h"
#include "log.h"
#include "data.h"
#include "utils.h"
#include "simulator.h"
#include "obc_config.h"
#include "system.h"
#include "locales.h"

static void _print_help(void)
{
	printf("Usage:\n");
	printf("openbikecomputer-core <option>\n");
	printf("\n");
	printf("Options:\n");
	printf("  -h, --help: print this\n");
	printf("  -v, --version: print the version\n");
	printf("  -s, --simulation <file>: launch simulation mode\n");
	printf("  -w, --screen_w <resolution X>: Set screen horizontal resolution\n");
	printf("  -h, --screen_h <resolution Y>: Set screen vertical resolution\n");
	printf("  -r, --rotation <angle>: rotation angle of the screen, possible value 0, 90, 180 or 270\n");
}

static void _print_version(void)
{
	printf("%d.%d.%d\n",\
		OPEN_BIKE_COMPUTER_VERSION_MAJOR,
		OPEN_BIKE_COMPUTER_VERSION_MINOR,
		OPEN_BIKE_COMPUTER_VERSION_FIX
	);
}

#define SIM_STRING_SIZE 64
int main(int argc, char **argv)
{
	int ret = 0;
	int c = 0;
	bool simulation_mode = false;
	char simulation_file[SIM_STRING_SIZE];
	int resolution_hor = SCREEN_HOR_SIZE;
	int resolution_ver = SCREEN_VER_SIZE;
	int screen_rotation = SCREEN_ROTATION;

	/* Disable getopt error output */
	opterr = 0;
	while (1)
	{
		static struct option long_options[] =
		{
			{"help",       no_argument,       0, 'h'},
			{"version",    no_argument,       0, 'v'},
			{"simulation", required_argument, 0, 's'},
			{"screen_w",   required_argument, 0, 'a'},
			{"screen_h",   required_argument, 0, 'b'},
			{"rotation",   required_argument, 0, 'c'},
			{0, 0, 0, 0}
		};

		/* Parse application arguments to get the options */
		c = getopt_long(argc, argv, "hvs:a:b:c:", long_options, NULL);

		/* Detect the end of the options. */
		if(c == -1)
			break;

		switch(c)
		{
			case 'h':
				/* Show help and exit */
				_print_help();
				exit(0);
				break;

			case 'v':
				/* Show version and exit */
				_print_version();
				exit(0);
				break;

			case 's':
				simulation_mode = true;
				safe_strncpy(simulation_file, optarg, sizeof(simulation_file));
				break;

			case 'a':
				resolution_hor = atoi(optarg);
				break;
			case 'b':
				resolution_ver = atoi(optarg);
				break;
			case 'c':
				screen_rotation = atoi(optarg);
				break;

			case '?':
			default:
				/* Option error */
				_print_help();
				exit(-1);
				break;
		}
	}

	/* Init all configuration system, bike, rider and user */
	ret = obc_config_init();
	fail_if_negative(ret, -1, "obc_config_init failed, return: %d\n", ret);

	/* TODO */
	locales_init();

	/* Init the data manager and recorder subsystem */
	ret = data_init();
	fail_if_negative(ret, -2, "data_init failed, return: %d\n", ret);

	/* Init the ui and display the main screen */
	ret = ui_init(resolution_hor, resolution_ver, screen_rotation);
	fail_if_negative(ret, -3, "ui initialization failed, return: %d\n", ret);

	/* If simulation mode is set, play the simulation file */
	if(simulation_mode)
	{
		ret = simulator_init(simulation_file);
		fail_if_negative(ret, -4, "simulator initialization failed, return: %d\n", ret);
	}

	/* Don't exit the application */
	while (1) {
		pause();
	}

	return 0;
}
