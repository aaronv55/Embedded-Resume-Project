/** @file gui_menu_templates.h
*
* @brief  This file contains definitions, structs, types and variables which are used to create
*         the system's GUI elements
* @author Aaron Vorse
* @date   Aug 23, 2020
* @contact aaron.vorse@embeddedresume.com
*
* @LICENSE:
*
*  Licensed to the Apache Software Foundation (ASF) under one
*  or more contributor license agreements.  See the NOTICE file
*  distributed with this work for additional information
*  regarding copyright ownership.  The ASF licenses this file
*  to you under the Apache License, Version 2.0 (the
*  "License"); you may not use this file except in compliance
*  with the License.  You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing,
*  software distributed under the License is distributed on an
*  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
*  KIND, either express or implied.  See the License for the
*  specific language governing permissions and limitations
*  under the License.
*/


#ifndef GUI_MENU_TEMPLATES_H
#define GUI_MENU_TEMPLATES_H

#define MENU1_BUTTON_HEIGHT 84
#define MENU1_BUTTON_SPACING 82
#define MENU1_MAX_BUTTONS 4
#define MENU1_PICTURE_WIDTH 72
#define MENU1_PICTURE_HEIGHT 70
#define MENU1_PICTURE_X_OFFSET 22
#define MENU1_BMP_X_OFFSET 11
#define MENU1_TEXT_Y_OFFSET 105
#define MENU1_ARROWS_X_OFFSET 286
#define MENU1_ARROWS_Y_OFFSET 111

#define MENU2_PICTURE_HEIGHT 160
#define MENU2_NAME_X_OFFSET 80
#define MENU2_NAME_Y_OFFSET HEADER_OFFSET + MENU2_PICTURE_HEIGHT - 27
#define MENU2_TEXT_SPACING 50
#define MENU2_TEXT_OFFSET 23
#define MENU2_SUBTEXT_OFFSET MENU2_TEXT_OFFSET + 20

#define MENU3_UTILITIES_BAR_OFFSET 359
#define MENU3_ARROWS_LEFT_X_OFFSET 60
#define MENU3_ARROWS_RIGHT_X_OFFSET 220
#define MENU3_ARROWS_Y_OFFSET MENU3_UTILITIES_BAR_OFFSET + 8
#define MENU3_PLAY_BUTTON_X_OFFSET 23
#define MENU3_PLAY_BUTTON_Y_OFFSET MENU3_UTILITIES_BAR_OFFSET + 37
#define MENU3_PAUSE_BUTTON_ICON_X_OFFSET MENU3_PLAY_BUTTON_X_OFFSET + 10
#define MENU3_PAUSE_BUTTON_ICON_Y_OFFSET MENU3_PLAY_BUTTON_Y_OFFSET + 10
#define MENU3_PLAY_BUTTON_ICON_X_OFFSET MENU3_PLAY_BUTTON_X_OFFSET + 10
#define MENU3_PLAY_BUTTON_ICON_Y_OFFSET MENU3_PLAY_BUTTON_Y_OFFSET + 8

#define MENU3_REPEAT_ICON_WIDTH 29
#define MENU3_REPEAT_ICON_X_OFFSET (320 - MENU3_PLAY_BUTTON_X_OFFSET - MENU3_REPEAT_ICON_WIDTH)
#define MENU3_REPEAT_ICON_Y_OFFSET MENU3_PLAY_BUTTON_Y_OFFSET - 1
#define MENU3_TIMER_BAR_X_START MENU3_PLAY_BUTTON_X_OFFSET + 37
#define MENU3_TIMER_BAR_X_END MENU3_TIMER_BAR_X_START + 200
#define MENU3_TIMER_BAR_Y_OFFSET MENU3_PLAY_BUTTON_ICON_Y_OFFSET + 2
#define MENU3_MARKER_WIDTH 15
#define MENU3_MARKER_Y_OFFSET MENU3_REPEAT_ICON_Y_OFFSET + 5
#define MENU3_TIMER_TEXT_X_OFFSET 100
#define MENU3_TIMER_TEXT_Y_OFFSET MENU3_TIMER_BAR_Y_OFFSET + 14

#define MENU_CONTACT_PROFILE_OFFSET HEADER_OFFSET + 148
#define MENU_CONTACT_TEXT_X_OFFSET 35
#define MENU_CONTACT_TEXT_Y_OFFSET  MENU_CONTACT_PICTURE_OFFSET + 40
#define MENU_CONTACT_TEXT_SPACING 45
#define MENU_CONTACT_PICTURE_OFFSET MENU_CONTACT_PROFILE_OFFSET + 10
#define MENU_CONTACT_ICON_WIDTH 32

#define MENU_SKILLS_ICON_WIDTH 32
#define MENU_SKILLS_ICON_HEIGHT 40
#define MENU_SKILLS_PICTURE_Y_OFFSET HEADER_OFFSET + 9
#define MENU_SKILLS_TEXT_SPACING 48

#define MENU_WARNING_BACKGROUND_OFFSET HEADER_OFFSET - 10
#define MENU_WARNING_BACKGROUND_HEIGHT MENU_WARNING_BACKGROUND_OFFSET + 250

#define MENU_SETTINGS_BUTTON_X_OFFSET 140
#define MENU_SETTINGS_BUTTON_Y_OFFSET 175
#define MENU_SETTINGS_VOLUME_Y_OFFSET MENU_SETTINGS_BUTTON_Y_OFFSET + 45
#define MENU_SETTINGS_TIMESET_Y_OFFSET MENU_SETTINGS_VOLUME_Y_OFFSET + 75

#define MENU_SETTINGS_TIME_X_OFFSET 56
#define MENU_SETTINGS_TIME_Y_OFFSET 210

#define MENU_ABOUTME_SUBMENU_TEXT_X_OFFSET 20
#define MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET 150

#define MENU_INTRO_CHECK_X_OFFSET 25
#define MENU_INTRO_CHECK_Y_OFFSET 200

#include "gui.h"
#include "struct_buttons.h"


/*
****************************************************
********** Enums for Specific Menu Buttons  ********
****************************************************
*/
typedef enum e_user_button_tag
{
   back_button,
   home_button,
   square_button

} e_user_button;


typedef enum e_menu3_button_tag
{
   menu3_left_arrow,
   menu3_right_arrow,
   menu3_play_pause,
   menu3_repeat,
   menu3_time_bar0,
   menu3_time_bar1,
   menu3_time_bar2,
   menu3_time_bar3,
   menu3_time_bar4,
   menu3_max_buttons

} e_menu3_button;


/*
****************************************************
** Menu Templates defined in gui_menu_templates.c **
****************************************************
*/
t_button menu1_template[4];
t_button user_button_template[3];
t_button menu3_template[4];
t_button warning_menu_template[2];
t_button intro_menu_template[3];
t_button languages_menu_template[2];
t_button settings_menu_template[4];
t_button settings_time_menu_template[6];
t_button about_me_menu_template[5];

t_button touch_targets_template[5];


#endif /* GUI_MENU_TEMPLATES_H */
