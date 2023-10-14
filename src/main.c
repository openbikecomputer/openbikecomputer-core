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
#include "version.h"
#include "ui.h"
#include "log.h"
#include "bike_config.h"
#include "rider_config.h"
#include "system_config.h"

int main(int argc, char **argv)
{
	int ret = 0;

	printf("OpenBikeComputer core V%d.%d.%d\n",\
		OPEN_BIKE_COMPUTER_VERSION_MAJOR,
		OPEN_BIKE_COMPUTER_VERSION_MINOR,
		OPEN_BIKE_COMPUTER_VERSION_FIX
	);

	ret = system_config_init();
	fail_if_negative(ret, -1, "Error: system_config_init failed, return: %d\n", ret);

	ret = bike_config_init();
	fail_if_negative(ret, -2, "Error: bike_config_init failed, return: %d\n", ret);

	ret = rider_config_init();
	fail_if_negative(ret, -3, "Error: rider_config_init failed, return: %d\n", ret);

	ret = ui_init();
	fail_if_negative(ret, -4, "Error: ui initialization failed, return: %d\n", ret);

	/* Don't exit the application */
	while (1) {
		pause();
	}

	return 0;
}
