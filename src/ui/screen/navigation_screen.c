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
#include "navigation_screen.h"
#include "assets.h"
#include "system.h"

static T_lv_btn back_btn;
static lv_obj_t *fake_map_img;

int navigation_screen_enter(lv_obj_t *screen)
{
	int ret = 0;

	/* Temporary fake map */
	fake_map_img = lv_image_create(screen);
	lv_image_set_src(fake_map_img, IMAGE_MAP);
	lv_obj_set_size(fake_map_img, SCREEN_HOR_SIZE, SCREEN_VER_SIZE);
	lv_obj_align(fake_map_img, LV_ALIGN_CENTER, 0, 0);

	/* Create the label */
	lv_obj_t* label = lv_label_create(fake_map_img);
	lv_label_set_text(label, "Comming soon...");
	lv_obj_center(label);

	/* Change style of text */
	lv_obj_set_style_text_font(label, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);


	/* Remove padding and border */
	lv_obj_set_style_pad_all(fake_map_img, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(fake_map_img, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_outline_width(fake_map_img, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

	lv_obj_update_layout(fake_map_img);

	/* Add back button to go in main screen */
	ret = lvgl_helper_create_button(&back_btn, screen, BACK_BUTTON_SIZE_X, BACK_BUTTON_SIZE_Y, BACK_BUTTON_ALIGN, BACK_BUTTON_POS_X, BACK_BUTTON_POS_Y, BACK_BUTTON_TEXT, &lvgl_helper_back_button_event_handler);
	fail_if_negative(ret, -1, "lvgl_helper_create_button failed, return %d\n");

	return 0;
}

int navigation_screen_exit(void)
{
	return 0;
}
