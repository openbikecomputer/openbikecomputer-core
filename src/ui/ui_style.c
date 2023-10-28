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

#include <lvgl.h>
#include <string.h>
#include "log.h"
#include "ui_style.h"

static struct {
	bool is_initialized;
	lv_style_t default_style;
} ui_style = {
	.is_initialized = false,
};

int ui_style_init(void)
{
	fail_if_true(ui_style.is_initialized, -1, "Error: ui_style is already initialized\n");

	lv_style_init(&ui_style.default_style);
	lv_style_set_text_font(&ui_style.default_style, &lv_font_montserrat_48);

	/* Mark the style module as initialized */
	ui_style.is_initialized = true;
	return 0;
}

int ui_style_get_default_style(lv_style_t *style)
{
	fail_if_false(ui_style.is_initialized, -1, "Error: ui_style is not initialized\n");
	fail_if_null(style, -2, "Error: style is null\n");

	memcpy(style, &ui_style.default_style, sizeof(lv_style_t));

	return 0;
}
