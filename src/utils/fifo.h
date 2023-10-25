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

#ifndef _FIFO_HEADER_
#define _FIFO_HEADER_

#include <stdbool.h>
#include <stddef.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct {
	bool is_initialized;
	char *buffer;
	int nb_element; /* Total number that can fit in the fifo */
	int element_count; /* Actual number of element in the fifo */
	int element_size; /* size of one element in the fifo */
	int read_index; /* Read index used by pop and wait */
	int write_index; /* Write index used by push */
	pthread_mutex_t mutex; /* mutex to protect the access to the fifo data */
	sem_t sem; /* Semaphore used for the wait */
} T_fifo;

int fifo_create(T_fifo *fifo, int nb_element, size_t element_size);
int fifo_destroy(T_fifo *fifo);
int fifo_push(T_fifo *fifo, void *element);
int fifo_pop(T_fifo *fifo, void *element);
int fifo_pop_wait(T_fifo *fifo, void *element);
int fifo_get_element_count(T_fifo *fifo);

// TODO
//int fifo_push_wait

#endif //_FIFO_HEADER_
