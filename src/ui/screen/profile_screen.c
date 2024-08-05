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
#include "lvgl_helper.h"
#include "profile_screen.h"


static void button_event_handler(lv_event_t *event)
{
	int *user_data = lv_event_get_user_data(event);
	E_screen_id next_screen = *user_data;

	ui_change_screen(next_screen);
}

typedef struct {
	lv_obj_t *obj;
	char *image;
	E_screen_id next_screen;
} T_button;

#define NB_BUTTON 2
#define MARGIN 20
#define BUTTON_SIZE ((ui_get_resolution_hor() / 2) - (2 * MARGIN))

static struct {
	T_button button[NB_BUTTON];
} profiles_screen = {
	.button[0] = {.image = "D:/usr/share/openbikecomputer/images/profiles_riders.png", .next_screen = E_RIDER_SCREEN},
	.button[1] = {.image = "D:/usr/share/openbikecomputer/images/profiles_bikes.png", .next_screen = E_BIKE_SCREEN},
};

int profile_screen_enter(lv_obj_t *screen)
{
	for(int i = 0; i < NB_BUTTON; i++)
	{
		T_button *btn = &profiles_screen.button[i];

		/* Create the button */
		btn->obj = lv_imagebutton_create(screen);
		lv_imagebutton_set_src(btn->obj, LV_IMAGEBUTTON_STATE_RELEASED, btn->image, NULL, NULL);

		/* Change button size */
		lv_obj_set_size(btn->obj, BUTTON_SIZE, BUTTON_SIZE);

		/* Align the two bouton one over the other in the center of the screen */
		if(i == 0)
		{
			lv_obj_align(btn->obj, LV_ALIGN_CENTER, 0, -((BUTTON_SIZE/2) + (MARGIN / 2)));
		}
		else
		{
			lv_obj_align(btn->obj, LV_ALIGN_CENTER, 0, ((BUTTON_SIZE/2) + (MARGIN / 2)));
		}

		/* Link the button click to the event callback */
		lv_obj_add_event_cb(btn->obj, &button_event_handler, LV_EVENT_CLICKED, (void*)&btn->next_screen);
	}

	return 0;
}
int profile_screen_exit(void)
{
	return 0;
}
