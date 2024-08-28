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

#ifndef _TOPBAR_STYLES_HEADER_
#define _TOPBAR_STYLES_HEADER_

#include <lvgl.h>

#define TOPBAR_LABEL_X_OFFSET (-16)
#define TOPBAR_BACK_BUTTON_X_OFFSET (16)
#define TOPBAR_BACK_BUTTON_Y_OFFSET (0)
#define TOPBAR_BACK_BUTTON_X_SIZE_PERCENT (35)
#define TOPBAR_BACK_BUTTON_Y_SIZE_PERCENT (75)
#define TOPBAR_BACK_BUTTON_COLOR 0x489c48
#define TOPBAR_BACK_BUTTON_OPACITY LV_OPA_COVER

int topbar_styles_init(void);
lv_style_t* topbar_styles_get_general_style(void);
lv_style_t* topbar_styles_get_back_button_style(void);

#endif /* _TOPBAR_STYLES_HEADER_ */
