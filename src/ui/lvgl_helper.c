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
#include "lvgl_helper.h"

void lvgl_helper_back_button_event_handler(lv_event_t *event)
{
	lv_event_code_t code = lv_event_get_code(event);

    if(code == LV_EVENT_CLICKED) {
		ui_change_screen(E_MAIN_SCREEN);
    }
}

int lvgl_helper_create_button(T_lv_btn *obj, int size_x, int size_y, lv_align_t align, int pos_x, int pos_y, char *text, btn_handler handler)
{
	fail_if_null(text, -1, "Error: text is null\n");
	fail_if_null(handler, -2, "Error: handler is null\n");
	fail_if_null(obj, -3, "Error: btn is null\n");

	int ret = 0;

	ret = ui_style_get_default_style(&obj->style);
	fail_if_negative(ret, -4, "Error: ui_style_get_default_style failed, return %d\n");

	/* Create button */
	obj->btn = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(obj->btn, handler, LV_EVENT_ALL, NULL);
	lv_obj_set_size(obj->btn, size_x, size_y);
	lv_obj_align(obj->btn, align, pos_x, pos_y);
	lv_obj_add_style(obj->btn, &obj->style, 0);

	/* Put the text in the label */
	obj->label = lv_label_create(obj->btn);
	lv_label_set_text(obj->label, text);
    lv_obj_center(obj->label);

	return 0;
}
