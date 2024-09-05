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
#include "locales.h"
#include "styles.h"

static struct {
	uint32_t btn_cnt;
	lv_obj_t * main_page;
	lv_obj_t * menu;
} bike_screen = {
	.btn_cnt = 1,
};

typedef struct {
	lv_obj_t *container;
	lv_obj_t *label;
	lv_obj_t *data;
} T_bike_menu_element;

#define LABEL_GAP_PX 15
#define OFFSET_Y 0
#define PCT_HOR_SIZE 50
#define KEYBOARD_HEIGHT 40
#define NB_ENTRY 6
#define PCT_VER_SIZE ((100-KEYBOARD_HEIGHT)/NB_ENTRY)

static void _ta_event_cb(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * ta = lv_event_get_target(e);
	lv_obj_t * kb = lv_event_get_user_data(e);
	if(code == LV_EVENT_FOCUSED) {
		lv_obj_add_state(ta, LV_STATE_FOCUSED);
		lv_keyboard_set_textarea(kb, ta);
	}

	if(code == LV_EVENT_DEFOCUSED) {
		lv_obj_remove_state(ta, LV_STATE_FOCUSED);
		lv_keyboard_set_textarea(kb, NULL);
	}
}

static int _create_text_entry(T_bike_menu_element *element, lv_obj_t *subpage, char *name, char *placeholder)
{
	fail_if_null(element, -1, "element is NULL\n");
	fail_if_null(subpage, -2, "subpage is NULL\n");
	fail_if_null(name, -3, "name is NULL\n");
	fail_if_null(placeholder, -4, "placeholder is NULL\n");

	/* Create container */
	element->container = lv_menu_cont_create(subpage);
	fail_if_null(element->container, -6, "lv_label_create failed\n");

	/* Apply style */
	lv_obj_add_style(element->container, styles_get_no_border_style(), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(element->container, styles_get_font_inter_regular_18(), LV_PART_MAIN | LV_STATE_DEFAULT);

	/* Set container size */
    lv_obj_set_size(element->container, lv_pct(100), lv_pct(PCT_VER_SIZE));
	lv_obj_update_layout(element->container);

	/* Create label in container */
    element->label = lv_label_create(element->container);
	fail_if_null(element->label, -7, "lv_label_create failed\n");

    lv_label_set_text(element->label, name);
    lv_obj_set_size(element->label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_update_layout(element->label);

	/* Create text area in container */
    element->data = lv_textarea_create(element->container);
	fail_if_null(element->data, -8, "lv_textarea_create failed\n");

    lv_textarea_set_placeholder_text(element->data, placeholder);
    lv_obj_set_size(element->data, lv_pct(PCT_HOR_SIZE), lv_pct(100));
	lv_obj_update_layout(element->data);

	/* Set the text area background color using the style */
	lv_obj_add_style(element->data, styles_get_text_area_bg_color_style(), LV_PART_MAIN | LV_STATE_DEFAULT);

	/* Align the label and the text area */
    lv_obj_align(element->label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align_to(element->data, element->label, LV_ALIGN_OUT_RIGHT_MID, LABEL_GAP_PX, OFFSET_Y);

	return 0;
}

#define DROP_DOWN_STR_LENGTH 256
static int _get_drop_down_translations(char *str, int length)
{
	int ret = 0;
    ret = snprintf(str, length, "%s\n%s\n%s\n%s\n%s\n%s\n%s",
					_("All terrain bike"),
                    _("Mountain bike"),
                    _("Road"),
                    _("Gravel"),
                    _("Touring bike"),
                    _("Speedbike"),
                    _("Other"));
	if(ret >= length)
	{
		log_error("str was truncated\n");
		/* Force the \0 char when the string is truncated */
		str[DROP_DOWN_STR_LENGTH - 1] = '\0';
		return -1;
	}

	return 0;
}

static int _create_drop_down_entry(T_bike_menu_element *element, lv_obj_t *subpage)
{
	char str[DROP_DOWN_STR_LENGTH];
	int ret = 0;

	fail_if_null(subpage, -1, "subpage is NULL\n");
	fail_if_null(element, -2, "element is NULL\n");

	/* Create container */
	element->container = lv_obj_create(subpage);
	fail_if_null(element->container, -3, "container is NULL\n");

	/* Apply style */
	lv_obj_add_style(element->container, styles_get_no_border_style(), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(element->container, styles_get_transp_bg_style(), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(element->container, styles_get_font_inter_regular_18(), LV_PART_MAIN | LV_STATE_DEFAULT | LV_STATE_CHECKED);

	/* Set container size */
    lv_obj_set_size(element->container, lv_pct(100), lv_pct(PCT_VER_SIZE));
	lv_obj_update_layout(element->container);

	/* Create label in container */
    element->label = lv_label_create(element->container);
    lv_label_set_text(element->label, _("Type:"));
    lv_obj_set_size(element->label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_update_layout(element->label);

	/*Create drop down list in container */
    element->data = lv_dropdown_create(element->container);
	ret = _get_drop_down_translations(str, DROP_DOWN_STR_LENGTH);
	if(ret < 0)
	{
		log_error("_get_drop_down_translations failed, returned %d\n", ret);
	}
    lv_dropdown_set_options(element->data, str);
    lv_obj_set_size(element->data, lv_pct(PCT_HOR_SIZE), lv_pct(100));
	lv_obj_update_layout(element->data);

	/* Set the text area background color using the style */
	lv_obj_add_style(element->data, styles_get_text_area_bg_color_style(), LV_PART_MAIN | LV_STATE_DEFAULT);

	/* Set the text font of the element in the dropdown list */
	lv_obj_t * list = lv_dropdown_get_list(element->data);
	lv_obj_add_style(list, styles_get_font_inter_regular_18(), LV_PART_MAIN | LV_STATE_DEFAULT);

	/* Align the label and the drop down list */
    lv_obj_align(element->label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align_to(element->data, element->label, LV_ALIGN_OUT_RIGHT_MID, LABEL_GAP_PX, OFFSET_Y);

	return 0;
}

static lv_obj_t* _create_keyboard(lv_obj_t *subpage)
{
	lv_obj_t *kb;

	fail_if_null(subpage, NULL, "subpage is NULL\n");

	/* Add the keyboard to enter the informations */
	kb = lv_keyboard_create(subpage);
    lv_obj_set_size(kb, lv_pct(100), lv_pct(KEYBOARD_HEIGHT));
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

	/* Add the style to the object */
	lv_obj_add_style(kb, styles_get_transp_bg_style(), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(kb, styles_get_keyboard_keys_color_style(), LV_PART_ITEMS | LV_STATE_DEFAULT);
	lv_obj_add_style(kb, styles_get_font_inter_regular_18(), LV_PART_MAIN | LV_STATE_DEFAULT);

	return kb;
}

static int _create_subpage(void)
{
	int ret = 0;
    lv_obj_t *subpage, *cont, *label, *kb;
	T_bike_menu_element menu_entry[NB_ENTRY] = {0};

	log("Create subpage: %d\n", bike_screen.btn_cnt);

	/* Create the menu subpage */
    subpage = lv_menu_page_create(bike_screen.menu, NULL);
	fail_if_null(subpage, -1, "subpage is NULL\n");

	/* Add font style to the whole page */
	lv_obj_add_style(subpage, styles_get_font_inter_regular_18(), LV_PART_MAIN | LV_STATE_DEFAULT);

	/* Populate subpage content in display order */
	ret = _create_text_entry(&menu_entry[0], subpage, _("Name:"), _("Bike name"));
	fail_if_negative(ret, -2, "_create_text_entry failed, return: %d\n", ret);

	ret = _create_text_entry(&menu_entry[1], subpage, _("Brand:"), _("Bike brand"));
	fail_if_negative(ret, -3, "_create_text_entry failed, return: %d\n", ret);

	ret = _create_text_entry(&menu_entry[2], subpage, _("Model:"), _("Bike model"));
	fail_if_negative(ret, -4, "_create_text_entry failed, return: %d\n", ret);

	ret = _create_text_entry(&menu_entry[3], subpage, _("Weight:"), _("Bike weight in Kg"));
	fail_if_negative(ret, -5, "_create_text_entry failed, return: %d\n", ret);

	ret = _create_drop_down_entry(&menu_entry[4], subpage);
	fail_if_negative(ret, -6, "_create_drop_down_entry failed, return: %d\n", ret);

	ret = _create_text_entry(&menu_entry[5], subpage, _("Wheel size:"), _("Bike wheel size"));
	fail_if_negative(ret, -7, "_create_text_entry failed, return: %d\n", ret);

	/* Create the keyboard */
	kb = _create_keyboard(subpage);
	fail_if_null(kb, -8, "kb is NULL\n");

	/* Add keyboard event callback */
    lv_obj_add_event_cb(menu_entry[0].data, _ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_event_cb(menu_entry[1].data, _ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_event_cb(menu_entry[2].data, _ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_event_cb(menu_entry[3].data, _ta_event_cb, LV_EVENT_ALL, kb);
	/* The entry 4 with the drop down doesn't need keyboard input */
    lv_obj_add_event_cb(menu_entry[5].data, _ta_event_cb, LV_EVENT_ALL, kb);

	/* Put the focus on the first element */
	lv_obj_add_state(menu_entry[0].data, LV_STATE_FOCUSED);
	lv_keyboard_set_textarea(kb, menu_entry[0].data);

	/* Update the main page to add the subpage entry */
    cont = lv_menu_cont_create(bike_screen.main_page);
    label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "New bike%"LV_PRIu32, bike_screen.btn_cnt);
    lv_menu_set_load_page_event(bike_screen.menu, cont, subpage);

	/* Scroll to the new entry on the front page, this make it visible */
    lv_obj_scroll_to_view_recursive(cont, LV_ANIM_ON);

	return 0;
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
    lv_obj_align(bike_screen.menu, LV_ALIGN_TOP_MID, 0, 0);

	/* Make the bike screen menu background transparent and set the correct font */
	lv_obj_add_style(bike_screen.menu, styles_get_transp_bg_style(), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(bike_screen.menu, styles_get_font_inter_regular_18(), LV_PART_MAIN | LV_STATE_DEFAULT);

	log("Create menu main page\n");
    /*Create a main page that will contain the bike list */
    bike_screen.main_page = lv_menu_page_create(bike_screen.menu, NULL);

	/* TODO Populate the menu with existing bike configuration */

	log("Create menu button\n");
    /* Create floating button to add new bike configuration  */
    lv_obj_t * float_btn = lv_button_create(bike_screen.main_page);
	int btn_size = (ui_get_resolution_hor()/4);
    lv_obj_set_size(float_btn, btn_size, btn_size);
    lv_obj_add_flag(float_btn, LV_OBJ_FLAG_FLOATING);
    lv_obj_align(float_btn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(float_btn, _float_button_handler, LV_EVENT_CLICKED, bike_screen.menu);
    lv_obj_set_style_radius(float_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_img_src(float_btn, LV_SYMBOL_PLUS, 0);

	log("Create menu button\n");
	/* Go to the main page */
    lv_menu_set_page(bike_screen.menu, bike_screen.main_page);

	return 0;
}

int bike_screen_exit(void)
{
	return 0;
}
