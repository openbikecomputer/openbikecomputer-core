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


#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include "log.h"
#include "simulator.h"

static struct {
	bool is_initialized;
	pthread_t simu_thread;
} simulator = {
	.is_initialized = false,
};

static void * simu_thread_handler(void *data)
{
	/* Cast the data void data into char* */
	char *file = (char*)data;
	fail_if_null(file, NULL, "file is NULL\n");

	FILE * fd;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
	int line_counter = 0;

	if(access(file, F_OK) != 0)
	{
		fail(NULL, "file %s doesn't exist\n", file);
	}

	/* Open file */
	fd = fopen(file, "r");
	fail_if_null(fd, NULL, "fopen failed\n");

	while ((read = getline(&line, &len, fd)) != -1) {
		/* Count line read in file */
		line_counter++;

		/* Ignore line starting with # */
		if(line[0] == '#')
		{
			continue;
		}

		/* Use sscanf to get the value for each of the following parameter */
		/* latitude;longitude;speed;altitude;temperature;heart rate;power;cadence; */

		/* Value to get from each line */
		float latitude = 0;
		float longitude = 0;
		int speed = 0;
		int altitude = 0;
		int temperature = 0;
		int heart_rate = 0;
		int power = 0;
		int cadence = 0;

		/* extract the value from the string readed */
		errno = 0;
		int ret = sscanf(line, "%f;%f;%d;%d;%d;%d;%d;%d;", &latitude, &longitude, &speed, &altitude, &temperature, &heart_rate, &power, &cadence);
		if (ret != 8 || errno != 0) {
			log_error("simulation file %s, line %d is malformed, ignoring it\n", file, line_counter);
			continue;
		}

		/* push value to the data manager */
		printf("Value read:%f;%f;%d;%d;%d;%d;%d;%d\n", latitude, longitude, speed, altitude,temperature, heart_rate, power, cadence);
	}

	/* Close the file */
	fclose(fd);

	/* Cleanup the line buffer if necessary */
	if (line)
	{
		free(line);
	}

	return NULL;
}


int simulator_init(char *file_path)
{
	int ret = 0;

	fail_if_null(file_path, -1, "file_path is NULL\n");

	log_info("simulator is in simulation mode with file %s\n", file_path);

	/* Create a thread to read the file line by line and push fake data */
	ret = pthread_create(&simulator.simu_thread, NULL, &simu_thread_handler, file_path);
	fail_if_negative(ret, -1, "Create simulation thread failed, return: %d\n", ret);

	// TODO
	return 0;
}
