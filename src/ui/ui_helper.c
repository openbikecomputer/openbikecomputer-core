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
#include "ui_style.h"
#include "ui_helper.h"

int ui_helper_create_button(int size_x, int size_y, lv_align_t align, int pos_x, int pos_y, char *text, btn_handler handler)
{
	fail_if_null(text, -1, "Error: text is null\n");
	fail_if_null(handler, -2, "Error: handler is null\n");

	int ret = 0;

	lv_style_t default_style;
	ret = ui_style_get_default_style(&default_style);
	fail_if_negative(ret, -3, "Error: ui_style_get_default_style failed, return %d\n");

	/* Create button */
	lv_obj_t * button = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(button, handler, LV_EVENT_ALL, NULL);
	lv_obj_set_size(button, size_x, size_y);
	lv_obj_align(button, align, pos_x, pos_y);
	lv_obj_add_style(button, &default_style, 0);

	lv_obj_t * label_button;
	label_button = lv_label_create(button);
	lv_label_set_text(label_button, text);
    lv_obj_center(label_button);

	return 0;
}
