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
#include "lvgl_helper.h"
#include "main_screen.h"
#include "ui.h"
#include "assets.h"

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
	.button_array[0] = {.name = "Data",       .image = IMAGE_COUNTER,    .next_screen = E_DATA_SCREEN},
	.button_array[1] = {.name = "Navigation", .image = IMAGE_NAVIGATION, .next_screen = E_NAVIGATION_SCREEN},
	.button_array[2] = {.name = "Results",    .image = IMAGE_RESULTS,    .next_screen = E_RESULTS_SCREEN},
	.button_array[3] = {.name = "Routes",     .image = IMAGE_ROUTES,     .next_screen = E_ROUTES_SCREEN},
	.button_array[4] = {.name = "Profiles",   .image = IMAGE_PROFILES,   .next_screen = E_PROFILE_SCREEN},
	.button_array[5] = {.name = "Settings",   .image = IMAGE_SETTINGS,   .next_screen = E_SETTINGS_SCREEN},
};

int main_screen_enter(lv_obj_t *screen)
{
	int ret = 0;

	/* Create flexbox container and apply ui default style */
	main_screen.cont = lv_obj_create(screen);
	ret = ui_apply_default_style_to_obj(main_screen.cont);
	fail_if_negative(ret, -1, "ui_apply_default_style_to_obj failed, return: %d\n", ret);

	lv_obj_set_size(main_screen.cont, lv_pct(100), lv_pct(100));
	lv_obj_align(main_screen.cont, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_flex_flow(main_screen.cont, LV_FLEX_FLOW_ROW_WRAP);

	for(int i = 0; i < NB_BUTTON; i++)
	{
		T_button *btn = &main_screen.button_array[i];

		/* Create the button */
		btn->obj = lv_imagebutton_create(main_screen.cont);
		lv_imagebutton_set_src(btn->obj, LV_IMAGEBUTTON_STATE_RELEASED, btn->image, NULL, NULL);

		/* Change button size */
		lv_obj_set_size(btn->obj, BUTTON_SIZE, BUTTON_SIZE);
		lv_obj_align(btn->obj, LV_ALIGN_CENTER, 0, 0);

		/* Link the button click to the event callback */
		lv_obj_add_event_cb(btn->obj, &button_event_handler, LV_EVENT_CLICKED, (void*)&btn->next_screen);
	}

	return 0;
}

int main_screen_exit(void)
{
	return 0;
}
