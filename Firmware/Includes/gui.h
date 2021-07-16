/** @file gui.h
*
* @brief  This file is used to display menus and graphic elements to the LCD.
* @author Aaron Vorse
* @date   8/21/2020
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

#ifndef GUI_H
#define GUI_H

/*
****************************************************
*************** GUI THEME COLORS *******************
****************************************************
*/
#define WHITE_PURE 0xFFFF
#define RED_PURE 0xF800
#define RED_LIGHT 0xF945
#define RED_DARK 0xC0C2
#define GREEN_PURE 0x07E0
#define BLUE_PURE 0x001F
#define BLUE_SKY 0x0476
#define GREEN_MAIN 0x03e9
#define GREEN_DARK 0x02C7
#define GREEN_LIGHT 0x968C
#define GREEN_PHONE 0x0325 //A special shade used specifically for the phone bitmap
#define GREEN_NEON 0x35C3
#define PINK 0xF80B
#define GRAY_EXTRA_LIGHT 0xD6DA
#define GRAY_LIGHT 0xB596
#define GRAY_MEDIUM 0x6b4d
#define GRAY_MEDIUM_DARK 0x39C7
#define GRAY_DARK 0x2945
#define GRAY_DARK_ALT 0x3186
#define GRAY_MENU3 0x4208 //Gray used only in menu type 3

#define PURPLE 0x50CA
#define BLACK 0x0000
#define NULL_COLOR 0x1234

#define GUI_HEADER_MAIN 0x012D /*0x0957,0x41CF*/
#define GUI_HEADER_DARK 0x010B/*0x012D,0x318D*/
#define GUI_HEADER_GRAY GRAY_DARK

#define THEME_MAIN 0x012D /*0x0957,0x41CF*/
#define THEME_DARK 0x010B/*0x012D,0x318D*/
#define THEME_WHITE 0xFFFF
#define THEME_NEAR_WHITE GRAY_EXTRA_LIGHT
#define THEME_TEXT GRAY_DARK
#define THEME_SUBTEXT GRAY_MEDIUM
#define THEME_LIGHT_GRAY GRAY_LIGHT

#define SB_OFFSET 28 //Status bar offset
#define TB_HEIGHT 77 //Title bar offset
#define HEADER_OFFSET SB_OFFSET + TB_HEIGHT
#define FOOTER_OFFSET 441

#define SB_TIME_X_OFFSET 20
#define SB_TIME_Y_OFFSET 10

#define SB_USB_X_OFFSET 165
#define SB_USB_Y_OFFSET 3
#define SB_HEADPHONES_X_OFFSET SB_USB_X_OFFSET + 27
#define SB_HEADPHONES_Y_OFFSET 8
#define SB_SPEAKER_X_OFFSET SB_HEADPHONES_X_OFFSET + 23
#define SB_SPEAKER_Y_OFFSET 8
#define SB_BATTERY_X_OFFSET SB_SPEAKER_X_OFFSET + 33
#define SB_BATTERY_Y_OFFSET 5
#define SB_BATTERY_TEXT_X_OFFSET SB_BATTERY_X_OFFSET + 22
#define SB_BATTERY_TEXT_Y_OFFSET 10
#define SB_BATTERY_FILL_Y_OFFSET 22

#define PROFILE_BAR_ICON_X_OFFSET 22

#define GUI_TESTS_HEADER_HEIGHT 70
#define GUI_TESTS_TITLE_Y_OFFSET 100

#include <math.h>
#include <strings.h>
#include "struct_gui_person_profile.h"
#include "gui_menu_templates.h"
#include "struct_buttons.h"
#include "lcd.h"
#include "dac.h"
#include "buttons.h"
#include "enum_dac_volume.h"
#include "monitor.h"
#include "rtc.h"

/*
****************************************************
******* Public Functions Defined in gui.c **********
****************************************************
*/
void gui_set_status_bar_color(uint16_t background_color);
void gui_set_footer_color(uint16_t background_color);
uint16_t gui_get_footer_color(void);
void gui_update_status_bar(void);
void gui_create_menu_type1(char *title, t_light_button *button_list, size_t list_length, uint8_t active_button);

void gui_update_menu1_buttons( t_light_button *button_list, uint8_t menu1_substate, uint8_t tmp_current_substate);
void gui_create_menu_type2(const t_person_profile tmp_profile);
void gui_create_menu_type3(void);
void gui_update_menu3_clock(uint32_t tmp_total_blocks, uint8_t first_entry_flag);
void gui_draw_play_button(void);
void gui_draw_pause_button(void);
void gui_create_menu_contact_info(const t_light_button *tmp_button_list);
void gui_create_menu_skills(const t_light_button *tmp_button_list, size_t length);
void gui_create_warning_menu( t_button *tmp_buttons, char **strings);

void gui_create_menu_languages(uint32_t main_menu_image_address);
void gui_create_settings_menu(void);
void gui_update_settings_menu(void);
void gui_create_settings_time_menu(void);
void gui_update_settings_time_menu(uint8_t *time_buffer, uint8_t current_place);

void gui_create_aboutme_main_menu(uint32_t *tmp_button_images);
void gui_create_aboutme_education_menu(void);
void gui_create_aboutme_goals_menu(void);
void gui_create_aboutme_interests_menu(void);
void gui_create_aboutme_experience_menu(void);
void gui_create_aboutme_hobbies_menu(void);
void gui_create_intro_menu(uint8_t tmp_startup_status_flag);

void gui_loading_circle_animation(uint16_t x, uint16_t y);
void gui_boot_animation(uint32_t *animation_frames, size_t number_of_frames);

void gui_tests_create_main_menu(void);
void gui_tests_display_pass_screen(void);
void gui_tests_create_usb_menu(void);
void gui_tests_create_battery_menu(uint8_t voltage_test_number);
void gui_tests_create_touch_menu(uint8_t touch_test_number);
void gui_tests_create_power_button_menu(void);
void gui_tests_create_LCD_switch_menu(uint8_t seconds_remaining);
void gui_tests_create_audio_menu(uint8_t test_number);

#endif /* GUI_H */

/* end of file */
