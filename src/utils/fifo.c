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
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>

#include "log.h"
#include "fifo.h"

int fifo_create(T_fifo *fifo, int nb_element, size_t element_size)
{
	fail_if_null(fifo, -1, "fifo is null\n");
	fail_if_true(fifo->is_initialized, -2, "fifo is already initialized\n");

	int ret = 0;
	errno = 0;

	ret = sem_init(&fifo->sem, 0, 0);
	fail_if_not_zero(ret, -1, "Error: sem_init failed, errno: %d\n", errno);

	ret = pthread_mutex_init(&fifo->mutex, NULL);
	fail_if_not_zero(ret, -2, "Error: pthread_mutex_init failed, return %d\n", ret);

	/* alloc buffer */
	fifo->buffer = malloc(nb_element * element_size);
	fail_if_null(fifo->buffer, -3, "Error: malloc fifo buffer failed\n");

	/* Init all struct variables */
	fifo->nb_element = nb_element;
	fifo->element_size = element_size;
	fifo->element_count = 0;
	fifo->read_index = 0;
	fifo->write_index = 0;

	/* Mark the fifo as initialized */
	fifo->is_initialized = true;

	return 0;
}

int fifo_destroy(T_fifo *fifo)
{
	fail_if_null(fifo, -1, "fifo is null\n");
	fail_if_false(fifo->is_initialized, -2, "fifo is not initialized\n");

	if(fifo->buffer)
	{
		free(fifo->buffer);
	}

	fifo->is_initialized = false;

	return 0;
}

int fifo_push(T_fifo *fifo, void *element)
{
	fail_if_null(fifo, -1, "fifo is null\n");
	fail_if_false(fifo->is_initialized, -2, "fifo is not initialized\n");

	int ret = 0;

	pthread_mutex_lock(&fifo->mutex);

	if(fifo->element_count == fifo->nb_element)
	{
		log_error("Error: fifo is full\n");
		ret = -3;
		goto push_cleanup;
	}

	char *dest = &fifo->buffer[fifo->element_size * fifo->write_index];
	if(!memcpy(dest, element, fifo->element_size))
	{
		log_error("Error: memcpy %p in %p failed\n", element, dest);
		ret = -4;
		goto push_cleanup;
	}

	/* Update variables */
	fifo->write_index++;
	fifo->element_count++;

	if(fifo->write_index > fifo->nb_element)
	{
		fifo->write_index = 0;
	}

	if(sem_post(&fifo->sem) != 0)
	{
		log_error("Error: sem_post failed, errno: %d\n", errno);
		ret = -5;
		goto push_cleanup;
	}

push_cleanup:
	pthread_mutex_unlock(&fifo->mutex);

	return ret;
}

static int _pop(T_fifo *fifo, void *element)
{
	pthread_mutex_lock(&fifo->mutex);
	int ret = 0;

	if(fifo->element_count == 0)
	{
		log_error("Error: fifo is empty\n");
		ret = -1;
		goto pop_cleanup;
	}

	char *src = &fifo->buffer[fifo->element_size * fifo->read_index];
	if(!memcpy(element, src, fifo->element_size))
	{
		log_error("Error: memcpy %p in %p failed\n", src, element);
		ret = -4;
		goto pop_cleanup;
	}

	/* Update variables */
	fifo->read_index++;
	fifo->element_count--;

	if(fifo->read_index > fifo->nb_element)
	{
		fifo->read_index = 0;
	}

pop_cleanup:
	pthread_mutex_unlock(&fifo->mutex);

	return ret;
}

int fifo_pop(T_fifo *fifo, void *element)
{
	fail_if_null(fifo, -1, "fifo is null\n");
	fail_if_false(fifo->is_initialized, -2, "fifo is not initialized\n");

	/* Use trywait to decrease in case it was increase */
	sem_trywait(&fifo->sem);

	return _pop(fifo, element);
}

int fifo_pop_wait(T_fifo *fifo, void *element)
{
	fail_if_null(fifo, -1, "fifo is null\n");
	fail_if_false(fifo->is_initialized, -2, "fifo is not initialized\n");

	int ret = 0;

	ret = sem_wait(&fifo->sem);
	fail_if_not_zero(ret, -3, "Error: sem_wait failed, errno: %d\n", errno);

	return _pop(fifo, element);
}

int fifo_get_element_count(T_fifo *fifo)
{
	int element_count = 0;

	pthread_mutex_lock(&fifo->mutex);
	element_count = fifo->element_count;
	pthread_mutex_unlock(&fifo->mutex);

	return element_count;
}
