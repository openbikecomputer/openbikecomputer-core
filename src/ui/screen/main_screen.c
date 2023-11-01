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
#include "ui_style.h"
#include "lvgl_helper.h"
#include "main_screen.h"
#include "ui.h"

static void button_event_handler(lv_event_t *event)
{
	int *user_data = lv_event_get_user_data(event);
	E_screen_id next_screen = *user_data;

	ui_change_screen(next_screen);
}

typedef struct {
	lv_obj_t *obj;
	char *name;
	char *image;
	E_screen_id next_screen;
} T_button;

#define NB_BUTTON 6
#define MARGIN 20
#define BUTTON_SIZE ((ui_get_resolution_hor() / 2) - (2 * MARGIN))
static struct {
	lv_style_t style;
	lv_obj_t * cont;
	T_button button_array[NB_BUTTON];
} main_screen = {
	.button_array[0] = {.name = "Data",       .image = "D:/usr/share/openbikecomputer/images/counter.png", .next_screen = E_DATA_SCREEN},
	.button_array[1] = {.name = "Navigation", .image = "D:/usr/share/openbikecomputer/images/navigation.png", .next_screen = E_NAVIGATION_SCREEN},
	.button_array[2] = {.name = "Results",    .image = "D:/usr/share/openbikecomputer/images/results.png", .next_screen = E_RESULT_SCREEN},
	.button_array[3] = {.name = "Routes",     .image = "D:/usr/share/openbikecomputer/images/routes.png", .next_screen = E_ROUTE_SCREEN},
	.button_array[4] = {.name = "Profiles",   .image = "D:/usr/share/openbikecomputer/images/profiles.png", .next_screen = E_PROFILE_SCREEN},
	.button_array[5] = {.name = "Settings",   .image = "D:/usr/share/openbikecomputer/images/settings.png", .next_screen = E_SETTINGS_SCREEN},
};

int main_screen_enter(void)
{
	/* Load global style */
	ui_style_get_default_style(&main_screen.style);

	main_screen.cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_screen.cont, lv_pct(100), lv_pct(90));
    lv_obj_align(main_screen.cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(main_screen.cont, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_add_style(main_screen.cont, &main_screen.style, 0);

	for(int i = 0; i < NB_BUTTON; i++)
	{
		T_button *btn = &main_screen.button_array[i];

		/* Create the button */
		btn->obj = lv_imgbtn_create(main_screen.cont);
		lv_imgbtn_set_src(btn->obj, LV_IMGBTN_STATE_RELEASED, btn->image, NULL, NULL);

		/* Change button size */
		lv_obj_set_size(btn->obj, BUTTON_SIZE, BUTTON_SIZE);

		/* Link the button click to the event callback */
		lv_obj_add_event_cb(btn->obj, &button_event_handler, LV_EVENT_CLICKED, (void*)&btn->next_screen);
	}

	return 0;
}

int main_screen_exit(void)
{
	return 0;
}
