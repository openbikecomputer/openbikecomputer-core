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
#include "main_screen.h"
#include "screen_manager.h"

static void rider_button_event_handler(lv_event_t *event)
{ 
	lv_event_code_t code = lv_event_get_code(event); 

    if(code == LV_EVENT_CLICKED) {
		screen_manager_change_screen(E_RIDER_CONF_SCREEN);
    }
}
static void bike_button_event_handler(lv_event_t *event)
{
	lv_event_code_t code = lv_event_get_code(event); 

    if(code == LV_EVENT_CLICKED) {
		screen_manager_change_screen(E_BIKE_CONF_SCREEN);
    }
}
static void system_button_event_handler(lv_event_t *event)
{
	lv_event_code_t code = lv_event_get_code(event); 

    if(code == LV_EVENT_CLICKED) {
		screen_manager_change_screen(E_SYSTEM_CONF_SCREEN);
    }
}
static void data_button_event_handler(lv_event_t *event)
{
	lv_event_code_t code = lv_event_get_code(event); 

    if(code == LV_EVENT_CLICKED) {
		screen_manager_change_screen(E_DATA_SCREEN);
    }
}

int main_screen_enter(void)
{
	lv_obj_t * label_bike_conf;
	lv_obj_t * label_rider_conf;
	lv_obj_t * label_system_conf;
	lv_obj_t * label_data_screen;

	/* Create a style to increase the font size */
	static lv_style_t style_btn;
	lv_style_init(&style_btn);
	lv_style_set_text_font(&style_btn, &lv_font_montserrat_48);

	/* Create data button */
	lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn1, data_button_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_set_size(btn1, 600, 200);
	lv_obj_align(btn1, LV_ALIGN_TOP_MID, 0, 50);
	lv_obj_add_style(btn1, &style_btn, 0);

	label_data_screen = lv_label_create(btn1);
	lv_label_set_text(label_data_screen, "Real time data");
    lv_obj_center(label_data_screen);

	/* Create rider button */
	lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn2, rider_button_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_set_size(btn2, 600, 200);
	lv_obj_align(btn2, LV_ALIGN_TOP_MID, 0, 300); //+250
	lv_obj_add_style(btn2, &style_btn, 0);

	label_rider_conf = lv_label_create(btn2);
	lv_label_set_text(label_rider_conf, "Rider configuration");
    lv_obj_center(label_rider_conf);

	/* Create bike button */
	lv_obj_t * btn3 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn3, bike_button_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_set_size(btn3, 600, 200);
	lv_obj_align(btn3, LV_ALIGN_TOP_MID, 0, 550); //+250*2+50
	lv_obj_add_style(btn3, &style_btn, 0);

	label_bike_conf = lv_label_create(btn3);
	lv_label_set_text(label_bike_conf, "Bike configuration");
    lv_obj_center(label_bike_conf);

	/* Create system button */
	lv_obj_t * btn4 = lv_btn_create(lv_scr_act());
	lv_obj_add_event_cb(btn4, system_button_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_set_size(btn4, 600, 200);
	lv_obj_align(btn4, LV_ALIGN_TOP_MID, 0, 800); //+250*3+50
	lv_obj_add_style(btn4, &style_btn, 0);

	label_system_conf = lv_label_create(btn4);
	lv_label_set_text(label_system_conf, "System configuration");
    lv_obj_center(label_system_conf);

	return 0;
}

int main_screen_exit(void)
{
	return 0;
}
