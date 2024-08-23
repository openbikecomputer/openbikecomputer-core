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

#include "locales.h"

#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <stdlib.h>

#include "log.h"

int locales_init()
{
	char *ret = NULL;

	char *env = NULL;

	/* LANG and LANGUAGE variable can override the language choose by the
	 * the user, display it content if they are set for a more easy troubleshooting
	 */
	env = getenv("LANG");
	if(env)
	{
		log("environment variable LANG is set to %s\n", env);
	}

	env = getenv("LANGUAGE");
	if(env)
	{
		log("environment variable LANGUAGE is set to %s\n", env);
	}

	/* Setting the i18n environment */
	ret = setlocale(LC_ALL, "");
	fail_if_null(ret, -1, "setlocale failed\n");

	ret = bindtextdomain(LOCALES_TEXTDOMAIN, LOCALES_FOLDER);
	fail_if_null(ret, -2, "bindtextdomain failed\n");

	ret = textdomain(LOCALES_TEXTDOMAIN);
	fail_if_null(ret, -3, "textdomain failed\n");

	return 0;
}

int locales_set(char *locale)
{
	fail_if_null(locale, -1, "locale is NULL\n");

	char *ret = NULL;
	ret = setlocale(LC_MESSAGES, locale);
	fail_if_null(ret, -2, "setlocale failed\n");

	return 0;
}
