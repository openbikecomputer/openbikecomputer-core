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
#include "data_manager.h"
#include "utils.h"
#include "simulator.h"
#include "config.h"

static void _print_help(void)
{
	printf("Usage:\n");
	printf("openbikecomputer-core <option>\n");
	printf("\n");
	printf("Options:\n");
	printf("  -h, --help: print this\n");
	printf("  -v, --version: print the version\n");
	printf("  -s, --simulation <file>: launch simulation mode\n");
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

	/* Disable getopt error output */
	opterr = 0;
	while (1)
	{
		static struct option long_options[] =
		{
			{"help",       no_argument,       0, 'h'},
			{"version",    no_argument,       0, 'v'},
			{"simulation", required_argument, 0, 's'},
			{0, 0, 0, 0}
		};

		/* Parse application arguments to get the options */
		c = getopt_long(argc, argv, "hvs:", long_options, NULL);

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

			case '?':
			default:
				/* Option error */
				_print_help();
				exit(-1);
				break;
		}
	}

	ret = config_init();
	fail_if_negative(ret, -1, "Error: config_init failed, return: %d\n", ret);

	ret = data_manager_init();
	fail_if_negative(ret, -2, "Error: data_manager_init failed, return: %d\n", ret);

	ret = ui_init();
	fail_if_negative(ret, -3, "Error: ui initialization failed, return: %d\n", ret);

	if(simulation_mode)
	{
		ret = simulator_init(simulation_file);
		fail_if_negative(ret, -4, "Error: simulator initialization failed, return: %d\n", ret);
	}

	/* Don't exit the application */
	while (1) {
		pause();
	}

	return 0;
}
