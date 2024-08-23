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

#ifndef _LOCALES_HEADER_
#define _LOCALES_HEADER_

#include <libintl.h>
#include <locale.h>

#include "assets.h"

/* Shorter version of the gettext macro */
#define _(STRING) gettext(STRING)

/* App locales configuration */
#define LOCALES_TEXTDOMAIN   "openbikecomputer"
#define LOCALES_FOLDER       RESOURCES_FOLDER "locales"

int locales_init();
int locales_set(char *locale);

#endif /* _LOCALES_HEADER_ */
