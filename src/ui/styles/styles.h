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

#ifndef _STYLES_HEADER_
#define _STYLES_HEADER_

#include <lvgl.h>

#ifndef DEBUG_TOPBAR_STYLE
	#define DEBUG_TOPBAR_STYLE 0
#endif

#ifndef DEBUG_VIRTUAL_SCREEN_STYLE
	#define DEBUG_VIRTUAL_SCREEN_STYLE 0
#endif

int styles_init(void);
lv_style_t* styles_get_virtual_screen_style(void);
void styles_disable_scrollbar(lv_obj_t *obj);

#endif /* _STYLES_HEADER_ */

