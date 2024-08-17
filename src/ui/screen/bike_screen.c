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
#include "bike_screen.h"
#include "ui.h"

static struct {
	uint32_t btn_cnt;
	lv_obj_t * main_page;
	lv_obj_t * menu;
} bike_screen = {
	.btn_cnt = 1,
};

#define PCT_HOR_SIZE 45
#define PCT_VER_SIZE (100/6)
static void _create_subpage(void)
{
    lv_obj_t *sub_page, *cont, *label, *ta, *dd, *kb;

	printf("Create subpage: %d\n", bike_screen.btn_cnt);

	/* Create the menu subpage */
    sub_page = lv_menu_page_create(bike_screen.menu, NULL);

	/* Populate subpage content */
	/* Name */
    cont = lv_menu_cont_create(sub_page);
    label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "Name:");
    //lv_obj_set_size(label, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);

	/* Name text area writable using the keyboard */
    ta = lv_textarea_create(cont);
    //lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "Jean-Michel");
	lv_obj_set_width(ta, lv_pct(PCT_HOR_SIZE));
    //lv_obj_set_size(ta, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(ta, LV_ALIGN_RIGHT_MID, 0, 0);

	/* Brand */
    cont = lv_menu_cont_create(sub_page);
    label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "Brand:");
    //lv_obj_set_size(label, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);

	/* Name text area writable using the keyboard */
    ta = lv_textarea_create(cont);
    //lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "AvalanceBike");
	lv_obj_set_width(ta, lv_pct(PCT_HOR_SIZE));
    //lv_obj_set_size(ta, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(ta, LV_ALIGN_RIGHT_MID, 0, 0);

	/* Model */
    cont = lv_menu_cont_create(sub_page);
    label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "Model:");
    //lv_obj_set_size(label, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);

	/* Model text area writable using the keyboard */
    ta = lv_textarea_create(cont);
    //lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "Topbike3000");
	lv_obj_set_width(ta, lv_pct(PCT_HOR_SIZE));
    //lv_obj_set_size(ta, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(ta, LV_ALIGN_RIGHT_MID, 0, 0);

	/* Weight */
    cont = lv_menu_cont_create(sub_page);
    label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "Weight:");
    //lv_obj_set_size(label, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);

	/* Weight text area writable using the keyboard */
    ta = lv_textarea_create(cont);
    //lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "15kg");
	lv_obj_set_width(ta, lv_pct(PCT_HOR_SIZE));
    //lv_obj_set_size(ta, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(ta, LV_ALIGN_RIGHT_MID, 0, 0);

	/* Type */
    cont = lv_menu_cont_create(sub_page);
    label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "Type:");
    //lv_obj_set_size(label, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);

	/*Create bike type drop down list*/
    dd = lv_dropdown_create(cont);
    lv_dropdown_set_options(dd, "All terrain bike\n"
                            "Mountain bike\n"
                            "Road\n"
                            "Gravel\n"
                            "Touring bike\n"
                            "Speedbike\n"
                            "Other");

	lv_obj_set_width(dd, lv_pct(PCT_HOR_SIZE));
    //lv_obj_set_size(dd, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(dd, LV_ALIGN_RIGHT_MID, 0, 0);
    //lv_obj_add_event_cb(dd, event_handler, LV_EVENT_ALL, NULL);

	/* Wheel size */
    cont = lv_menu_cont_create(sub_page);
    label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "Whell size:");
    //lv_obj_set_size(label, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);

	/* Wheel size text area writable using the keyboard */
    ta = lv_textarea_create(cont);
    //lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "2298mm");
	lv_obj_set_width(ta, lv_pct(PCT_HOR_SIZE));
    //lv_obj_set_size(ta, lv_pct(PCT_HOR_SIZE), lv_pct(PCT_VER_SIZE));
    lv_obj_align(ta, LV_ALIGN_RIGHT_MID, 0, 0);

	/* Add the keyboard to enter the informations */
	kb = lv_keyboard_create(sub_page);
    lv_obj_set_size(kb, lv_pct(100), lv_pct(50));
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

	/* Update the main page to add the subpage entry */
    cont = lv_menu_cont_create(bike_screen.main_page);
    label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "New bike %"LV_PRIu32, bike_screen.btn_cnt);
    lv_menu_set_load_page_event(bike_screen.menu, cont, sub_page);

	/* Scroll to the new entry on the front page, this make it visible */
    lv_obj_scroll_to_view_recursive(cont, LV_ANIM_ON);
}

static void _float_button_handler(lv_event_t * e)
{
    LV_UNUSED(e);

    bike_screen.btn_cnt++;

	/* Create subpage */
	_create_subpage();
}

static void _back_button_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_t * menu = lv_event_get_user_data(e);

    if(lv_menu_back_button_is_root(menu, obj)) {
		/* Go to the previous screen */
		ui_change_screen(E_PROFILE_SCREEN);
    }
}

int bike_screen_enter(lv_obj_t *screen)
{
    /* Create a menu object */
    bike_screen.menu = lv_menu_create(screen);

	/* Enable root back button to exit the menu */
	lv_menu_set_mode_root_back_button(bike_screen.menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);
    lv_obj_add_event_cb(bike_screen.menu, _back_button_handler, LV_EVENT_CLICKED, bike_screen.menu);

	/* Set size and center the menu */
    lv_obj_set_size(bike_screen.menu, lv_pct(100), lv_pct(100));
    lv_obj_center(bike_screen.menu);

	printf("Create menu main page\n");
    /*Create a main page that will contain the bike list */
    bike_screen.main_page = lv_menu_page_create(bike_screen.menu, NULL);

	/* TODO Populate the menu with existing bike configuration */

	printf("Create menu button\n");
    /* Create floating button to add new bike configuration  */
    lv_obj_t * float_btn = lv_button_create(bike_screen.main_page);
	int btn_size = (ui_get_resolution_hor()/4);
    lv_obj_set_size(float_btn, btn_size, btn_size);
    lv_obj_add_flag(float_btn, LV_OBJ_FLAG_FLOATING);
    lv_obj_align(float_btn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(float_btn, _float_button_handler, LV_EVENT_CLICKED, bike_screen.menu);
    lv_obj_set_style_radius(float_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_img_src(float_btn, LV_SYMBOL_PLUS, 0);

	printf("Create menu button\n");
	/* Go to the main page */
    lv_menu_set_page(bike_screen.menu, bike_screen.main_page);

	printf("done\n");
	return 0;
}

int bike_screen_exit(void)
{
	return 0;
}
