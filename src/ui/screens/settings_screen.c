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

#include <lvgl.h>

#include "log.h"
#include "settings_screen.h"
#include "lvgl_helper.h"
#include "locales.h"
#include "styles.h"
#include "version.h"

typedef enum {
	E_SETTING_WIFI = 0,
	E_SETTING_BLUETOOTH,
	E_SETTING_ANT,
	E_SETTING_SENSORS,
	E_SETTING_MAP,
	E_SETTING_ABOUT,
	E_SETTING_LAST, // must be last
} E_settings_subpage_id;

static struct {
	lv_obj_t *menu;
	lv_obj_t *main_page;
	lv_obj_t *sub_pages[E_SETTING_LAST];
} settings_screen;

static char* _get_subpage_title(E_settings_subpage_id id)
{
	switch(id)
	{
		case E_SETTING_WIFI:
			return _("WiFi");
			break;
		case E_SETTING_BLUETOOTH:
			return _("Bluetooth");
			break;
		case E_SETTING_ANT:
			return _("ANT");
			break;
		case E_SETTING_SENSORS:
			return _("Sensors");
			break;
		case E_SETTING_MAP:
			return _("Map");
			break;
		case E_SETTING_ABOUT:
			return _("About");
			break;

		/* In case the id doesn't exist, simply return an empty string */
		default:
		case E_SETTING_LAST:
			return "";
			break;
	}
}

static int _populate_wifi_subpage(lv_obj_t *subpage)
{
	fail_if_null(subpage, -1, "subpage is null\n");

	/*
	TODO
    lv_obj_t * cont;
    lv_obj_t * label;

    cont = lv_menu_cont_create(subpage);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, wifi parameters here");
	*/

	return 0;
}

static int _populate_bluetooth_subpage(lv_obj_t *subpage)
{
	fail_if_null(subpage, -1, "subpage is null\n");

	/*
	TODO
    lv_obj_t * cont;
    lv_obj_t * label;

    cont = lv_menu_cont_create(subpage);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, bluetooth parameters here");
	*/

	return 0;
}

static int _populate_ant_subpage(lv_obj_t *subpage)
{
	fail_if_null(subpage, -1, "subpage is null\n");

	/*
	TODO
    lv_obj_t * cont;
    lv_obj_t * label;

    cont = lv_menu_cont_create(subpage);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, ant parameters here");
	*/

	return 0;
}

static int _populate_sensors_subpage(lv_obj_t *subpage)
{
	fail_if_null(subpage, -1, "subpage sensors null\n");

	/*
	TODO
    lv_obj_t * cont;
    lv_obj_t * label;

    cont = lv_menu_cont_create(subpage);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, sensors parameters here");
	*/

	return 0;
}

static int _populate_map_subpage(lv_obj_t *subpage)
{
	fail_if_null(subpage, -1, "subpage is null\n");

	/*
	TODO
    lv_obj_t * cont;
    lv_obj_t * label;

    cont = lv_menu_cont_create(subpage);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, map parameters here");
	*/

	return 0;
}

static int _populate_about_subpage(lv_obj_t *subpage)
{
	fail_if_null(subpage, -1, "subpage is null\n");

    lv_obj_t * cont;
    lv_obj_t * label;

	/* Display OpenBikeComputer version */
    cont = lv_menu_cont_create(subpage);
    label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "OpenBikeComputer v%d.%d.%d", OPEN_BIKE_COMPUTER_VERSION_MAJOR, OPEN_BIKE_COMPUTER_VERSION_MINOR, OPEN_BIKE_COMPUTER_VERSION_FIX);

	/* Display LVGL version */
    cont = lv_menu_cont_create(subpage);
    label = lv_label_create(cont);
    lv_label_set_text_fmt(label, "LVGL v%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR);

	return 0;
}

static int _populate_subpage(lv_obj_t *subpage, E_settings_subpage_id id)
{
	int ret = 0;

	fail_if_null(subpage, -1, "subpage is null\n");

	switch(id)
	{
		case E_SETTING_WIFI:
			ret = _populate_wifi_subpage(subpage);
			fail_if_negative(ret, -2, "_populate_wifi_subpage failed, returned: %d\n", ret);
			break;
		case E_SETTING_BLUETOOTH:
			ret = _populate_bluetooth_subpage(subpage);
			fail_if_negative(ret, -3, "_populate_bluetooth_subpage failed, returned: %d\n", ret);
			break;
		case E_SETTING_ANT:
			ret = _populate_ant_subpage(subpage);
			fail_if_negative(ret, -4, "_populate_ant_subpage failed, returned: %d\n", ret);
			break;
		case E_SETTING_SENSORS:
			ret = _populate_sensors_subpage(subpage);
			fail_if_negative(ret, -5, "_populate_sensors_subpage failed, returned: %d\n", ret);
			break;
		case E_SETTING_MAP:
			ret = _populate_map_subpage(subpage);
			fail_if_negative(ret, -6, "_populate_map_subpage failed, returned: %d\n", ret);
			break;
		case E_SETTING_ABOUT:
			ret = _populate_about_subpage(subpage);
			fail_if_negative(ret, -7, "_populate_about_subpage failed, returned: %d\n", ret);
			break;

		/* In case the id doesn't exist */
		default:
		case E_SETTING_LAST:
			log_error("subpage id (%d) isn't supported\n", id);
			return -8;
			break;
	}

	return 0;
}

int settings_screen_enter(lv_obj_t *screen)
{
	int ret = 0;
    lv_obj_t * cont;
    lv_obj_t * label;

	fail_if_null(screen, -1, "screen is null\n");

    /*Create a menu object*/
    settings_screen.menu = lv_menu_create(screen);
    lv_obj_set_size(settings_screen.menu, lv_pct(100), lv_pct(100));
    lv_obj_center(settings_screen.menu);

	/* Apply style to the menu */
	lv_obj_add_style(settings_screen.menu, styles_get_transp_bg_style(), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_add_style(settings_screen.menu, styles_get_font_inter_regular_18(), LV_PART_MAIN | LV_STATE_DEFAULT);

	/* Create the menu main page */
	settings_screen.main_page = lv_menu_page_create(settings_screen.menu, NULL);

    /*Create a sub page */
	for(int i = E_SETTING_WIFI; i < E_SETTING_LAST; i++)
	{
		/* Create the subpage */
		settings_screen.sub_pages[i] = lv_menu_page_create(settings_screen.menu, NULL);

		/* Populate subpage content */
		ret = _populate_subpage(settings_screen.sub_pages[i], i);
		fail_if_negative(ret, -5, "_populate_subpage failed, returned: %d\n", ret);

		/* Create the subpage entry in the main page */
		cont = lv_menu_cont_create(settings_screen.main_page);
		label = lv_label_create(cont);
		lv_label_set_text(label, _get_subpage_title(i));
		lv_menu_set_load_page_event(settings_screen.menu, cont, settings_screen.sub_pages[i]);
	}

	/* Set the menu main_page as first one */
    lv_menu_set_page(settings_screen.menu, settings_screen.main_page);

	return 0;
}
int settings_screen_exit(void)
{
	return 0;
}
