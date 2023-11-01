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
#include <time.h>
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

#define STR_TIME_SIZE (8)
static T_lv_statusbar status_bar = {0};

static int _get_local_time(char *str, int size)
{
	fail_if_null(str, -1, "Error: str is null\n");

	time_t t;
	struct tm * tmp;

	t = time(NULL);
	tmp = localtime(&t);
	fail_if_null(tmp, -2, "Error: tmp is null\n");

	strftime(str, size, "%H:%M", tmp);

	return 0;
}

static void _statusbar_timer_handler(lv_timer_t * timer)
{
	int ret = 0;

	lv_obj_t *label = timer->user_data;

	char str[STR_TIME_SIZE];
	ret = _get_local_time(str, sizeof(str));
	if(ret < 0)
	{
		log_error("Error: _get_local_time failed, return %d\n");
		return;
	}
	lv_label_set_text_fmt(label, LV_SYMBOL_GPS " " LV_SYMBOL_WIFI " " LV_SYMBOL_BATTERY_FULL " %s", str);
}

int lvgl_helper_create_status_bar(void)
{
	int ret = 0;
	T_lv_statusbar *obj = &status_bar;

	ret = ui_style_get_default_style(&obj->style);
	fail_if_negative(ret, -2, "Error: ui_style_get_default_style failed, return %d\n");

	/* Create the status bar */
	obj->bar = lv_obj_create(lv_layer_top());
	lv_obj_set_size(obj->bar, ui_get_resolution_hor(), STATUS_BAR_SIZE);
	lv_obj_align(obj->bar, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_add_style(obj->bar, &obj->style, 0);

	/* Fill the status bar with widgets */
	obj->label = lv_label_create(obj->bar);
	char str[STR_TIME_SIZE];
	ret = _get_local_time(str, sizeof(str));
	lv_label_set_text_fmt(obj->label, LV_SYMBOL_GPS " " LV_SYMBOL_WIFI " " LV_SYMBOL_BATTERY_FULL " %s", str);
    lv_obj_align(obj->label, LV_ALIGN_RIGHT_MID, 0, 0);

	/* Create lvgl timer that update the bar each secondes */
	obj->timer = lv_timer_create(&_statusbar_timer_handler, STATUS_BAR_TIMER_DELAY, obj->label);
	lv_timer_set_repeat_count(obj->timer, -1); // repeat indefinitly

	return 0;
}

int lvgl_helper_destroy_status_bar(void)
{
	if(status_bar.timer != NULL)
	{
		lv_timer_del(status_bar.timer);
	}
	lv_obj_clean(lv_layer_top());
	return 0;
}
