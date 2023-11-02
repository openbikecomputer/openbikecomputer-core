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
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "log.h"

#define _GNU_SOURCE
#include <errno.h>
extern char * program_invocation_short_name;

#define MAX_LOG_LENGTH 256

//Log color
#define NORMAL    "\x1B[0m"
#define GREEN     "\x1B[32m"
#define BLACKBOLD "\033[1m"
#define REDBOLD   "\033[31m"

void _write_log(char* log_level_txt, const char * filename, const char * function, int line, const char * format, ...){
	int ret;
	struct timeval tv;
	struct tm * ptm;
	va_list args;

	char tmp_time_string[32];
	char time_string[32];
	char log_va_list_buffer[MAX_LOG_LENGTH];

	/*
	 * Get va_args into log_va_list_buffer
	 */
	va_start(args,format);
	vsnprintf(log_va_list_buffer, sizeof(log_va_list_buffer), format, args);
	va_end(args);

	/*
	 * make string from time
	 */
	ret = gettimeofday( &tv, NULL);
	if( ret < 0) {
		printf( "Error gettimeofday return %d\n", ret);
	}

	ptm = localtime( &tv.tv_sec);
	ret = strftime( tmp_time_string, sizeof(tmp_time_string), "%Y-%m-%d %H:%M:%S", ptm);
	if( ret == 0) {
		printf( "Error strftime return %d\n", (int) ret);
	}

	ret = snprintf( time_string, sizeof( time_string), "%s.%03ld", tmp_time_string, (tv.tv_usec/1000));
	if( ret < 0) {
		printf( "Error snprintf return %d\n", ret);
	}

	/*
	 * Print log on console
	 */
	printf( "%s %s [%d:%d][%s:%d:%s]%s %s", time_string, program_invocation_short_name,getpid(),(int)syscall( SYS_gettid), filename, line, function, log_level_txt, log_va_list_buffer);

	return;
}
