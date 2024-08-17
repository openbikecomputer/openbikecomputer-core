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
#include "styles.h"
#include "styles.h"
#include "topbar_styles.h"

#include <lvgl.h>

static struct {
	bool is_initialized;
	lv_style_t virtual_screen;
} styles = {
	.is_initialized = false,
};

static int _virtual_screen_style_init(void)
{
	/* Initialized virtual_screen topbar style */
	lv_style_init(&styles.virtual_screen);

	/* Padding, outline width */
	lv_style_set_pad_all(&styles.virtual_screen, 0);
	lv_style_set_outline_width(&styles.virtual_screen, 0);

	/* Border color and transp */
	lv_style_set_border_width(&styles.virtual_screen, 0);
	lv_style_set_border_opa(&styles.virtual_screen, LV_OPA_TRANSP);

	/* Background color and opacity */
	lv_style_set_bg_color(&styles.virtual_screen, lv_color_white());
	lv_style_set_bg_opa(&styles.virtual_screen, LV_OPA_COVER);

	/* Text color and opacity */
	lv_style_set_text_color(&styles.virtual_screen, lv_color_black());
	lv_style_set_text_opa(&styles.virtual_screen, LV_OPA_90);
	lv_style_set_text_font(&styles.virtual_screen, &lv_font_montserrat_14);

	/* Remove radius */
	lv_style_set_radius(&styles.virtual_screen, 0);

	#if DEBUG_VIRTUAL_SCREEN_STYLE
	/* Show border */
	lv_style_set_border_width(&styles.virtual_screen, 1);
	lv_style_set_border_color(&styles.virtual_screen, lv_color_hex(0x00FF00));
	lv_style_set_border_opa(&styles.virtual_screen, LV_OPA_COVER);
	#endif

	return 0;
}

/* This is the entry point for all LVGL styles initialization */
int styles_init(void)
{
	int ret = 0;

	ret = topbar_styles_init();
	fail_if_negative(ret, -1, "styles_init failed, returned: %d\n", ret);

	ret = _virtual_screen_style_init();
	fail_if_negative(ret, -2, " _virtual_screen_style_init failed, returned: %d\n", ret);

	styles.is_initialized = true;

	return 0;
}

lv_style_t* styles_get_virtual_screen_style(void)
{
	fail_if_false(styles.is_initialized, NULL, "styles is not initialized\n");

	return &styles.virtual_screen;
}

void styles_disable_scrollbar(lv_obj_t *obj)
{
	lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
}