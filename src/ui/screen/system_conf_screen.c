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
#include <lvgl.h>

#include "log.h"
#include "system.h"
#include "system_conf_screen.h"
#include "screen_manager.h"

static void back_button_event_handler(lv_event_t *event)
{ 
	lv_event_code_t code = lv_event_get_code(event); 

    if(code == LV_EVENT_CLICKED) {
		screen_manager_change_screen(E_MAIN_SCREEN);
    }
}

int system_screen_enter(void)
{
	lv_obj_t * label_back_button;

	/* Create a style to increase the font size */
	static lv_style_t style_btn;
	lv_style_init(&style_btn);
	lv_style_set_text_font(&style_btn, &lv_font_montserrat_48);

	/* Create back button */
	lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn1, back_button_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_set_size(btn1, 600, 200);
	lv_obj_align(btn1, LV_ALIGN_TOP_MID, 0, 50);
	lv_obj_add_style(btn1, &style_btn, 0);

	label_back_button = lv_label_create(btn1);
	lv_label_set_text(label_back_button, "Back");
    lv_obj_center(label_back_button);

	return 0;
}

int system_screen_exit(void)
{
	return 0;
}
