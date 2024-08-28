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

#ifndef _ASSETS_HEADER_
#define _ASSETS_HEADER_

#define LVGL_FS_LETTER "D:"

#ifndef RESOURCES_FOLDER
#define RESOURCES_FOLDER "./resources/"
#endif

/* Images path list, images are 128x128px */
#define IMAGE_COUNTER        LVGL_FS_LETTER RESOURCES_FOLDER "images/counter.png"
#define IMAGE_NAVIGATION     LVGL_FS_LETTER RESOURCES_FOLDER "images/navigation.png"
#define IMAGE_NO_IMAGE       LVGL_FS_LETTER RESOURCES_FOLDER "images/no_image.png"
#define IMAGE_PROFILES       LVGL_FS_LETTER RESOURCES_FOLDER "images/profiles.png"
#define IMAGE_PROFILES_BIKES LVGL_FS_LETTER RESOURCES_FOLDER "images/profiles_bikes.png"
#define IMAGE_PROFILES_RIDER LVGL_FS_LETTER RESOURCES_FOLDER "images/profiles_riders.png"
#define IMAGE_RESULTS        LVGL_FS_LETTER RESOURCES_FOLDER "images/results.png"
#define IMAGE_ROUTES         LVGL_FS_LETTER RESOURCES_FOLDER "images/routes.png"
#define IMAGE_SETTINGS       LVGL_FS_LETTER RESOURCES_FOLDER "images/settings.png"
#define IMAGE_MAP            LVGL_FS_LETTER RESOURCES_FOLDER "images/map.png"

/* Icons path list, icon are 32x32px */
#define ICON_BACK            LVGL_FS_LETTER RESOURCES_FOLDER "icons/back.png"

#endif /* _ASSETS_HEADER_ */
