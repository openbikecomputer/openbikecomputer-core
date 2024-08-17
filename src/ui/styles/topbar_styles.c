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
} topbar_styles = {
	.is_initialized = false,
};

int topbar_styles_init(void)
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
	lv_style_set_bg_color(&topbar_styles.general, lv_color_white());
	lv_style_set_bg_opa(&topbar_styles.general, LV_OPA_COVER);

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

	/* Set is_initialized */
	topbar_styles.is_initialized = true;

	return 0;
}

lv_style_t* topbar_styles_get_general_style(void)
{
	fail_if_false(topbar_styles.is_initialized, NULL, "topbar_styles is not initialized\n");

	return &topbar_styles.general;
}
