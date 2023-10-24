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
#ifndef _UTILS_HEADER_
#define _UTILS_HEADER_

#include <stdlib.h>
#include <strings.h>
#include "log.h"

/* Create a safe version of strncpy, always terminate the string 
 * by char '\0' even if the string is truncated
 */
#define safe_strncpy(dst,src,size){                           \
	strncpy(dst, src, size);                                  \
	dst[size-1]='\0';                                         \
}                                                             \

#endif //_UTILS_HEADER_
