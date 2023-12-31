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

#include "log.h"
#include "data_manager.h"
#include "data_recorder.h"
#include "data.h"

int data_init(void)
{
	int ret = 0;

	ret = data_manager_init();
	fail_if_negative(ret, -1, "data_manager_init failed, return: %d\n", ret);

	ret = data_recorder_init();
	fail_if_negative(ret, -2, "data_manager_init failed, return: %d\n", ret);

	return 0;
}
