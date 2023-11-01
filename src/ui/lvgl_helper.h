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

#ifndef _LVGL_HELPER_HEADER_
#define _LVGL_HELPER_HEADER_

#include <lvgl.h>
#include "ui.h"

#define TOP_BAR_SIZE ((ui_get_resolution_ver() / 10))

/* Back button shared by all screen */
#define BACK_BUTTON_SIZE_X ((ui_get_resolution_hor() / 4))
#define BACK_BUTTON_SIZE_Y ((ui_get_resolution_ver() / 10))
#define BACK_BUTTON_ALIGN (LV_ALIGN_TOP_LEFT)
#define BACK_BUTTON_POS_X 0
#define BACK_BUTTON_POS_Y (TOP_BAR_SIZE)
#define BACK_BUTTON_TEXT "< Back"

typedef struct {
	lv_obj_t *btn;
	lv_obj_t *label;
	lv_style_t style;
} T_lv_btn;

typedef void (*btn_handler)(lv_event_t *event);

void lvgl_helper_back_button_event_handler(lv_event_t *event);
int lvgl_helper_create_button(T_lv_btn *obj, int size_x, int size_y, lv_align_t align, int pos_x, int pos_y, char *text, btn_handler handler);

#endif //_LVGL_HELPER_HEADER_
