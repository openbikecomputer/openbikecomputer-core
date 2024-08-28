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
#include "topbar_styles.h"

#include <lvgl.h>


static struct {
	bool is_initialized;
	lv_style_t general;
	lv_style_t back_button;
} topbar_styles = {
	.is_initialized = false,
};

static int _general_styles_init(void)
{
	/* Initialized general topbar style */
	lv_style_init(&topbar_styles.general);

	/* Padding, outline width */
	lv_style_set_pad_all(&topbar_styles.general, 0);
	lv_style_set_outline_width(&topbar_styles.general, 0);

	/* Border color and transp */
	lv_style_set_border_width(&topbar_styles.general, 0);
	lv_style_set_border_opa(&topbar_styles.general, LV_OPA_TRANSP);

	/* Background color and opacity */
	lv_style_set_bg_color(&topbar_styles.general, lv_color_hex(SCREEN_BACKGROUND_COLOR));
	lv_style_set_bg_opa(&topbar_styles.general, SCREEN_BACKGROUND_OPACITY);

	/* Text color and opacity */
	lv_style_set_text_color(&topbar_styles.general, lv_color_black());
	lv_style_set_text_opa(&topbar_styles.general, LV_OPA_90);
	lv_style_set_text_font(&topbar_styles.general, &lv_font_montserrat_48);

	/* Debug options, show border, padding, outline, etc... */
	#if DEBUG_TOPBAR_STYLE
	/* Show border */
	lv_style_set_border_width(&topbar_styles.general, 1);
	lv_style_set_border_color(&topbar_styles.general, lv_color_hex(0xFF0000));
	lv_style_set_border_opa(&topbar_styles.general, LV_OPA_COVER);
	#endif

	/* Remove radius */
	lv_style_set_radius(&topbar_styles.general, 0);

	return 0;
}

static int _back_button_styles_init(void)
{
	/* Initialized back_button topbar style */
	lv_style_init(&topbar_styles.back_button);

	/* Padding, outline width */
	lv_style_set_pad_all(&topbar_styles.back_button, 8);
	lv_style_set_outline_width(&topbar_styles.back_button, 0);

	/* Border color and transp */
	lv_style_set_border_width(&topbar_styles.back_button, 0);
	lv_style_set_border_opa(&topbar_styles.back_button, LV_OPA_TRANSP);

	/* Background color and opacity */
	lv_style_set_bg_color(&topbar_styles.back_button, lv_color_hex(TOPBAR_BACK_BUTTON_COLOR));
	lv_style_set_bg_opa(&topbar_styles.back_button, TOPBAR_BACK_BUTTON_OPACITY);

	/* Text color and opacity */
	lv_style_set_text_color(&topbar_styles.back_button, lv_color_white());
	lv_style_set_text_opa(&topbar_styles.back_button, LV_OPA_COVER);
	lv_style_set_text_font(&topbar_styles.back_button, &lv_font_montserrat_20);

	/* Remove radius */
	lv_style_set_radius(&topbar_styles.back_button, 48);

	return 0;
}

int topbar_styles_init(void)
{
	int ret = 0;

	ret = _general_styles_init();
	fail_if_negative(ret, -1, "_general_styles_init failed, returned: %d\n");

	ret = _back_button_styles_init();
	fail_if_negative(ret, -2, "_back_button_styles_init failed, returned: %d\n");

	/* Set is_initialized */
	topbar_styles.is_initialized = true;

	return 0;
}

lv_style_t* topbar_styles_get_general_style(void)
{
	fail_if_false(topbar_styles.is_initialized, NULL, "topbar_styles is not initialized\n");

	return &topbar_styles.general;
}

lv_style_t* topbar_styles_get_back_button_style(void)
{
	fail_if_false(topbar_styles.is_initialized, NULL, "topbar_styles is not initialized\n");

	return &topbar_styles.back_button;
}
