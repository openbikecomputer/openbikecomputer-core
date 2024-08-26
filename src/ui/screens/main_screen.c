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
#include "locales.h"
#include "styles.h"

#define GAP_BETWEEN_BUTTON_LABEL_PX 10

static void button_event_handler(lv_event_t *event)
{
	int *user_data = lv_event_get_user_data(event);
	E_screen_id next_screen = *user_data;

	ui_change_screen(next_screen);
}

typedef struct {
	/* Elements set during screen creation */
	lv_obj_t *container;
	lv_obj_t *button;
	lv_obj_t *image;
	lv_obj_t *label;

	/* Elements set statically, DO NOT CHANGE */
	int bg_color;
	char *image_path;
	E_screen_id next_screen;
} T_button;

typedef enum {
	E_MAIN_BUTTON_DATA = 0,
	E_MAIN_BUTTON_NAVIGATION,
	E_MAIN_BUTTON_RESULTS,
	E_MAIN_BUTTON_ROUTES,
	E_MAIN_BUTTON_PROFILES,
	E_MAIN_BUTTON_SETTINGS,
	E_MAIN_BUTTON_NUMBER, // must be last
} E_main_button_id;

static struct {
	lv_style_t style;
	T_button button_array[E_MAIN_BUTTON_NUMBER];
} main_screen = {
	.button_array[E_MAIN_BUTTON_DATA]       = {.image_path = IMAGE_COUNTER,    .bg_color = 0xcf6c1b, .next_screen = E_DATA_SCREEN},
	.button_array[E_MAIN_BUTTON_NAVIGATION] = {.image_path = IMAGE_NAVIGATION, .bg_color = 0x53bf45, .next_screen = E_NAVIGATION_SCREEN},
	.button_array[E_MAIN_BUTTON_RESULTS]    = {.image_path = IMAGE_RESULTS,    .bg_color = 0xb16ccc, .next_screen = E_RESULTS_SCREEN},
	.button_array[E_MAIN_BUTTON_ROUTES]     = {.image_path = IMAGE_ROUTES,     .bg_color = 0x368aad, .next_screen = E_ROUTES_SCREEN},
	.button_array[E_MAIN_BUTTON_PROFILES]   = {.image_path = IMAGE_PROFILES,   .bg_color = 0xe69ae0, .next_screen = E_PROFILE_SCREEN},
	.button_array[E_MAIN_BUTTON_SETTINGS]   = {.image_path = IMAGE_SETTINGS,   .bg_color = 0x624791, .next_screen = E_SETTINGS_SCREEN},
};

static const char *_get_button_name(E_main_button_id button_id)
{
	switch(button_id)
	{
		case E_MAIN_BUTTON_DATA:
			return _("Data");
			break;
		case E_MAIN_BUTTON_NAVIGATION:
			return _("Navigation");
			break;
		case E_MAIN_BUTTON_RESULTS:
			return _("Results");
			break;
		case E_MAIN_BUTTON_ROUTES:
			return _("Routes");
			break;
		case E_MAIN_BUTTON_PROFILES:
			return _("Profiles");
			break;
		case E_MAIN_BUTTON_SETTINGS:
			return _("Settings");
			break;
		default:
			/* No translation */
			return "id_invalid";
			break;
	};
}

int main_screen_enter(lv_obj_t *screen)
{
	for(int i = 0; i < E_MAIN_BUTTON_NUMBER; i++)
	{
		/* Get the pointer on the actual button data we need */
		T_button *element = &main_screen.button_array[i];

		/* Create the container for the button and the label */
		element->container = lv_obj_create(screen);
		lv_obj_set_size(element->container, lv_pct(50), lv_pct(33));
		lv_obj_add_style(element->container, styles_get_main_button_style(), LV_PART_MAIN | LV_STATE_DEFAULT);

		/* Create the button */
		element->button = lv_button_create(element->container);
		lv_obj_set_size(element->button, lv_pct(100), lv_pct(85));
		lv_obj_align(element->button, LV_ALIGN_TOP_MID, 0, 0);
		lv_obj_add_style(element->button, styles_get_main_button_style(), LV_PART_MAIN | LV_STATE_DEFAULT);

		/* Set button background color */
		lv_obj_set_style_bg_color(element->button, lv_color_hex(element->bg_color), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_opa(element->button, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

		/* Create the image inside the button */
		element->image = lv_image_create(element->button);
		lv_image_set_src(element->image, element->image_path);
		lv_obj_center(element->image);
		lv_obj_add_style(element->image, styles_get_main_button_style(), LV_PART_MAIN | LV_STATE_DEFAULT);

		/* Create the label under the button */
		element->label = lv_label_create(element->container);
		lv_label_set_text(element->label, _get_button_name(i));
		lv_obj_align_to(element->label, element->button, LV_ALIGN_OUT_BOTTOM_MID, 0, GAP_BETWEEN_BUTTON_LABEL_PX);
		lv_obj_add_style(element->label, styles_get_main_button_style(), LV_PART_MAIN | LV_STATE_DEFAULT);

		/* Link the button click to the event callback */
		lv_obj_add_event_cb(element->button, &button_event_handler, LV_EVENT_CLICKED, (void*)&element->next_screen);
	}

	/* Align each container in the screen */
	lv_obj_align(main_screen.button_array[E_MAIN_BUTTON_DATA].container, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_align(main_screen.button_array[E_MAIN_BUTTON_NAVIGATION].container, LV_ALIGN_TOP_RIGHT, 0, 0);
	lv_obj_align_to(main_screen.button_array[E_MAIN_BUTTON_RESULTS].container, main_screen.button_array[E_MAIN_BUTTON_DATA].container, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
	lv_obj_align_to(main_screen.button_array[E_MAIN_BUTTON_ROUTES].container, main_screen.button_array[E_MAIN_BUTTON_NAVIGATION].container, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
	lv_obj_align_to(main_screen.button_array[E_MAIN_BUTTON_PROFILES].container, main_screen.button_array[E_MAIN_BUTTON_RESULTS].container, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
	lv_obj_align_to(main_screen.button_array[E_MAIN_BUTTON_SETTINGS].container, main_screen.button_array[E_MAIN_BUTTON_ROUTES].container, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);

	return 0;
}

int main_screen_exit(void)
{
	return 0;
}
