/** @file gui_menu_templates.c
*
* @brief  This file defines structs which are used to create the system's various GUI elements
* @author Aaron Vorse
* @date   8/23/2020
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

#include "gui_menu_templates.h"

/*
****************************************************
**************** Menu Templates ********************
****************************************************
*/

t_button menu1_template[4] =
{
   {.x_position = 0, .width = 320, .y_position = HEADER_OFFSET, .height = MENU1_BUTTON_HEIGHT},
   {.x_position = 0, .width = 320, .y_position = HEADER_OFFSET + (MENU1_BUTTON_HEIGHT), .height = MENU1_BUTTON_HEIGHT},
   {.x_position = 0, .width = 320, .y_position = HEADER_OFFSET + (2*MENU1_BUTTON_HEIGHT), .height = MENU1_BUTTON_HEIGHT},
   {.x_position = 0, .width = 320, .y_position = HEADER_OFFSET + (3*MENU1_BUTTON_HEIGHT), .height = MENU1_BUTTON_HEIGHT},
};


t_button menu3_template[4] =
{
   {.x_position = MENU3_ARROWS_LEFT_X_OFFSET, .y_position = MENU3_ARROWS_Y_OFFSET, .width = 40,      //Left Arrow
    .height = 20, .bmp = left_music_arrows_bmp},

   {.x_position = MENU3_ARROWS_RIGHT_X_OFFSET, .y_position = MENU3_ARROWS_Y_OFFSET, .width = 40,     //Right arrow
    .height = 20, .bmp = right_music_arrows_bmp},

   {.x_position = MENU3_PLAY_BUTTON_X_OFFSET, .y_position = MENU3_PLAY_BUTTON_Y_OFFSET, .width = 32, //Play/Pause Button
    .height = 26, .bmp = play_button_bmp},

   {.x_position = MENU3_REPEAT_ICON_X_OFFSET, .y_position = MENU3_REPEAT_ICON_Y_OFFSET, .width = 40, //Repeat Icon
    .height = 26, .bmp = repeat_music_icon_bmp}

};


t_button user_button_template[3] =
{
   {.width = 40, .height = 35, .x_position = 20, .y_position = 450, .body_color = 0x00,  //Back Button
    .text_color = 0xD6DA, .type = button_rectangle, .bmp = back_button_bmp},

   {.width = 40, .height = 35, .x_position = 145, .y_position = 450, .body_color = 0x00, //Home Button
    .text_color = 0xD6DA, .type = button_rectangle, .bmp = home_button_bmp},

   {.width = 40, .height = 35, .x_position = 280, .y_position = 450, .body_color = 0x00,  //Square Button
    .text_color = 0xD6DA, .type = button_rectangle, .bmp = square_button_bmp}
};


t_button warning_menu_template[2] =
{
   {.x_position = 25, .y_position = 280, .height = 50, .width = 130,},

   {.x_position = 165, .y_position = 280, .height = 50, .width = 130}
};


t_button intro_menu_template[3] =
{
   {.background_color = GRAY_MEDIUM_DARK, .body_color = THEME_LIGHT_GRAY, .text_color = GRAY_LIGHT,     //Skip
    .x_position = 25, .y_position = 280, .height = 50, .width = 130, .type = button_ghost},

   {.background_color = GRAY_MEDIUM_DARK, .body_color = GREEN_NEON,  .text_color = THEME_TEXT,          //Listen
    .x_position = 165, .y_position = 280, .height = 50, .width = 130, .type = button_rounded_corners},

   {.background_color = GRAY_MEDIUM_DARK, .body_color = GRAY_LIGHT,  .text_color = THEME_TEXT,          //Check box
    .x_position = 25, .y_position = 200, .height = 20, .width = 20, .type = button_rectangle},
};


t_button languages_menu_template[2] =
{
   {.x_position = 215, .y_position = HEADER_OFFSET + 70, .height = 120, .width = 90,}, //Germany

   {.x_position = 35, .y_position = HEADER_OFFSET + 190, .height = 130, .width = 130} //Spain
};


t_button settings_menu_template[4] =
{
   {.width = 52, .height = 22, .x_position = MENU_SETTINGS_BUTTON_X_OFFSET, .y_position = MENU_SETTINGS_BUTTON_Y_OFFSET, //Toggle sound button
    .type = button_rectangle},

   {.width = 28, .height = 24, .x_position = MENU_SETTINGS_BUTTON_X_OFFSET - 5, .y_position = MENU_SETTINGS_VOLUME_Y_OFFSET, //Lower volume button
    .type = button_rectangle},

   {.width = 24, .height = 24, .x_position = MENU_SETTINGS_BUTTON_X_OFFSET + 130, .y_position = MENU_SETTINGS_VOLUME_Y_OFFSET, //Raise volume button
   .type = button_rectangle},

   {.width = 160, .height = 35, .x_position = MENU_SETTINGS_BUTTON_X_OFFSET, .y_position = MENU_SETTINGS_TIMESET_Y_OFFSET, //Set time
   .type = button_rectangle},
};


t_button settings_time_menu_template[6] =
{
   {.background_color = GRAY_MEDIUM_DARK, .body_color = THEME_LIGHT_GRAY, .text_color = GRAY_MEDIUM, //Cancel button
    .x_position = 25, .y_position = 280, .height = 50, .width = 130, .type = button_ghost},

   {.background_color = GRAY_MEDIUM_DARK, .body_color = GREEN_NEON,  .text_color = THEME_TEXT, //Set button
    .x_position = 165, .y_position = 280, .height = 50, .width = 130, .type = button_rounded_corners},

   {.background_color = GRAY_MEDIUM_DARK, .x_position = 195, .y_position = 208, .height = 20, .width = 20, .type = button_rectangle}, //Left arrow

   {.background_color = GRAY_MEDIUM_DARK, .x_position = 258, .y_position = 208, .height = 20, .width = 20, .type = button_rectangle}, //Right arrow

   {.background_color = GRAY_MEDIUM_DARK, .x_position = 226, .y_position = 238, .height = 20, .width = 20, .type = button_rectangle}, //Down arrow

   {.background_color = GRAY_MEDIUM_DARK, .x_position = 226, .y_position = 170, .height = 20, .width = 20, .type = button_rectangle}, //Up arrow
};


t_button about_me_menu_template[5] =
{
   {.background_color = BLACK, .body_color = GRAY_DARK_ALT,  .x_position = 6, .y_position = SB_OFFSET + 7,  //Education
    .height = 110, .width = 151, .type = button_rounded_corners},

   {.background_color = BLACK, .body_color = GRAY_DARK_ALT, .x_position = 163, .y_position = SB_OFFSET + 7, //Goals
    .height = 160, .width = 151, .type = button_rounded_corners},

   {.background_color = BLACK, .body_color = GRAY_DARK_ALT, .x_position = 6, .y_position = SB_OFFSET + 124, //Hobbies
    .height = 180, .width = 151, .type = button_rounded_corners},

   {.background_color = BLACK, .body_color = GRAY_DARK_ALT, .x_position = 163, .y_position = SB_OFFSET + 7 + 160 + 6,   //Interests
    .height = 131, .width = 148, .type = button_rounded_corners},

   {.background_color = BLACK, .body_color = GRAY_DARK_ALT, .x_position = 6, .y_position = SB_OFFSET + 124 + 180 + 6, //Experience
    .height = 96, .width = 308, .type = button_rounded_corners},
};


t_button touch_targets_template[5] =
{
   {.background_color = BLACK, .body_color = BLACK, .x_position = 50, .y_position = (GUI_TESTS_TITLE_Y_OFFSET + 100),
    .height = 35, .width = 35, .text_color = BLACK, .type = button_rectangle},

   {.background_color = BLACK, .body_color = BLACK, .x_position = 220, .y_position = (GUI_TESTS_TITLE_Y_OFFSET + 100),
    .height = 35, .width = 35, .text_color = BLACK, .type = button_rectangle},

   {.background_color = BLACK, .body_color = BLACK, .x_position = 50, .y_position = (GUI_TESTS_TITLE_Y_OFFSET + 200),
    .height = 35, .width = 35, .text_color = BLACK, .type = button_rectangle},

   {.background_color = BLACK, .body_color = BLACK, .x_position = 220, .y_position = (GUI_TESTS_TITLE_Y_OFFSET + 200),
    .height = 35, .width = 35, .text_color = BLACK, .type = button_rectangle},

   {.background_color = BLACK, .body_color = BLACK, .x_position = 50, .y_position = (GUI_TESTS_TITLE_Y_OFFSET + 300),
    .height = 35, .width = 35, .text_color = BLACK, .type = button_rectangle},
};

/* end of file */

