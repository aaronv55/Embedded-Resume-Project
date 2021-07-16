/** @file gui.c
*
* @brief  This file contains functions for creating and interacting with graphic elements in
*         the system's menus, which are displayed on the LCD
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


#include "gui.h"

#pragma GCC push_options
//#pragma GCC optimize ("O3")

/*
****************************************************
***** Private Types and Structure Definitions ******
****************************************************
*/


/*
****************************************************
************* File-Static Variables ****************
****************************************************
*/
static uint16_t status_bar_background_color = BLACK;
static uint16_t footer_background_color = NULL_COLOR;


/*
****************************************************
********** Private Function Prototypes *************
****************************************************
*/
uint16_t gui_get_status_bar_color(void);
void gui_update_status_bar_battery(uint8_t background_color_change_flag);
void gui_get_battery_time_remaining(char *tmp_time_string);
void gui_update_status_bar_volume(uint8_t background_color_change_flag);
void gui_update_status_bar_time(uint8_t background_color_change_flag);
void gui_update_status_bar_headphones(uint8_t background_color_change_flag);
void gui_update_status_bar_usb(uint8_t background_color_change_flag);
void gui_create_title_bar(char *menu_title);
void gui_draw_audio_clock(uint16_t tmp_current_time, uint16_t tmp_total_time);
void gui_rtc_to_string(char *tmp_time_string);
void gui_settings_menu_update_volume(void);
void gui_settings_menu_update_battery(void);
void gui_time_to_string(char *tmp_time_string, uint8_t *tmp_time_buffer);

/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/

/*!
* @brief Set the status bar to a new color. This is used when transitioning into a new menu.
* @param[in] background_color
* @return  NONE
*/
void
gui_set_status_bar_color(uint16_t background_color)
{
   status_bar_background_color = background_color;
}


/*!
* @brief Set the footer to a new color.  This is used when transitioning into a new menu
* @param[in] background_color
* @return  NONE
*/
void
gui_set_footer_color(uint16_t background_color)
{
   footer_background_color = background_color;
}


/*!
* @brief Get the current footer color.  This is used when transitioning into a new menu
* @param[in] NONE
* @return  footer_background_color
*/
uint16_t
gui_get_footer_color(void)
{
   return(footer_background_color);
}


/*!
* @brief Redraw the status bar in order to match the current menu
* @param[in] NONE
* @return  NONE
*/
void
gui_update_status_bar(void)
{
   static uint16_t previous_background_color = NULL_COLOR;
   uint16_t current_background_color = gui_get_status_bar_color();
   uint8_t background_color_change_flag = 0;

   //If the status bar has changed color, it needs to be redrawn
   if(previous_background_color != current_background_color)
   {
      lcd_draw_rectangle(current_background_color, 0, 0, 320, SB_OFFSET);
      background_color_change_flag = 1;
   }

   previous_background_color = current_background_color;

   //Update each icon
   gui_update_status_bar_battery(background_color_change_flag);
   gui_update_status_bar_volume(background_color_change_flag);
   gui_update_status_bar_time(background_color_change_flag);
   gui_update_status_bar_headphones(background_color_change_flag);
   gui_update_status_bar_usb(background_color_change_flag);

}


/*!
* @brief Display a selection list type menu on the LCD
* @param[in] title The menu title that will be displayed below the status bar
* @param[in] button_list Contains the list of menu options and their attributes
* @param[in] list_length Variable to keep track of array length since passing by pointer loses this info
* @param[in] active_button If one of the menu options has been pressed by the user, this variable
*                          tells the function to make it appear different
* @return  NONE
*
* @note This function has a few "magic numbers", aka seemingly random numbers
*       without explanation. These are offsets to position each section of the menu
*       correctly and were created by trial and error and tuned with a visual check.
*       By defining most of these numbers in variables, the function trades speed
*       for coding clarity and readability.
*/
void
gui_create_menu_type1(char *title, t_light_button *button_list, size_t list_length, uint8_t active_button)
{
   gui_set_footer_color(BLACK);
   gui_set_status_bar_color(GUI_HEADER_DARK);

   gui_create_title_bar(title);

   //Text and picture/bitmap
   for(uint8_t current_button = 0; current_button < list_length; current_button++)
   {

      //Button background
      uint16_t y_initial = menu1_template[current_button].y_position;
      uint16_t y_final = y_initial + menu1_template[current_button].height;
      lcd_draw_rectangle(THEME_NEAR_WHITE, 0, y_initial, 320, y_final);

      //Display button picture
      uint16_t offset_x = MENU1_PICTURE_X_OFFSET;
      uint16_t offset_y = ((current_button*MENU1_BUTTON_SPACING) + HEADER_OFFSET + 9); //9 pixels down from the bottom of the header
      uint32_t image_address = button_list[current_button].image[0];
      const uint8_t *p_tmp_bmp = button_list[current_button].bmp;

      if(image_address != null_address)
      {
         lcd_image_from_sd(offset_x, offset_y, offset_x + MENU1_PICTURE_WIDTH, offset_y + MENU1_PICTURE_HEIGHT, image_address);
         offset_x += 80; //Move text far from the left screen, since pictures are big
         offset_y =  MENU1_TEXT_Y_OFFSET + (current_button*MENU1_BUTTON_SPACING) + SB_OFFSET;
      }

      else if (NULL != p_tmp_bmp)
      {
         //If there is no picture, display the button bitmap
         offset_x = MENU1_BMP_X_OFFSET;
         offset_y += 18;
         lcd_send_bitmap(button_list[current_button].bmp, offset_x, offset_y, BLUE_PURE, THEME_NEAR_WHITE);
         offset_x += 60; //Move text a medium amount from the left, since bitmaps are smaller than pictures
      }

      else
      {
         //If there is neither a picture nor a bitmap, shift the text down from the header
         offset_y += 18;
      }

      //Display major text
      lcd_print_string_small(button_list[current_button].major_text, offset_x, offset_y, THEME_TEXT, THEME_NEAR_WHITE);

      //Display minor text
      offset_x += 2;
      offset_y += 21;
      lcd_print_string_small(button_list[current_button].minor_text, offset_x, offset_y, THEME_SUBTEXT, THEME_NEAR_WHITE);

      //Display menu arrows
      offset_x = MENU1_ARROWS_X_OFFSET;
      offset_y =   MENU1_ARROWS_Y_OFFSET + (current_button*MENU1_BUTTON_SPACING) + SB_OFFSET;
      lcd_send_bitmap(menu_arrow_bmp, offset_x, offset_y, THEME_TEXT, THEME_NEAR_WHITE);
   }

}


/*!
* @brief If a button was clicked on the current menu1, darken the selected option
* @param[in] button_list
* @param[in] menu1_substate The substate(s) within an app that has a menu1 in it
* @param[in] tmp_current_substate
* @return  NONE
*/
void
gui_update_menu1_buttons( t_light_button *button_list, uint8_t menu1_substate, uint8_t tmp_current_substate)
{
   uint8_t active_button = buttons_status(menu1_template,(sizeof(menu1_template) / sizeof(*menu1_template)));

   if((NO_BUTTON_PRESS != active_button) && (menu1_substate == tmp_current_substate))
   {

      //Button background
      uint16_t y_initial = menu1_template[active_button].y_position;
      uint16_t y_final = y_initial + menu1_template[active_button].height;
      lcd_draw_rectangle(THEME_LIGHT_GRAY, 0, y_initial, 320, y_final);

      //Display button picture
      uint16_t offset_x = MENU1_PICTURE_X_OFFSET;
      uint16_t offset_y = ((active_button*MENU1_BUTTON_SPACING) + HEADER_OFFSET + 9); //9 pixels down from the bottom of the header
      uint32_t image_address = button_list[active_button].image[1];
      const uint8_t *p_tmp_bmp = button_list[active_button].bmp;

      if(image_address != null_address)
      {
         lcd_image_from_sd(offset_x, offset_y, offset_x + MENU1_PICTURE_WIDTH, offset_y + MENU1_PICTURE_HEIGHT, image_address);
         offset_x += 80; //Move text far from the left screen, since pictures are big
         offset_y =  MENU1_TEXT_Y_OFFSET + (active_button*MENU1_BUTTON_SPACING) + SB_OFFSET;
      }

      else if (NULL != p_tmp_bmp)
      {
         //If there is no picture, display the button bitmap
         offset_x = MENU1_BMP_X_OFFSET;
         offset_y += 18;
         lcd_send_bitmap(button_list[active_button].bmp, offset_x, offset_y, THEME_MAIN, THEME_LIGHT_GRAY);
         offset_x += 60; //Move text a medium amount from the left, since bitmaps are smaller than pictures
      }

      else
      {
         //If there is neither a picture nor a bitmap, shift the text down from the header
         offset_y += 18;
      }

      //Display major text
      lcd_print_string_small(button_list[active_button].major_text, offset_x, offset_y, THEME_TEXT, THEME_LIGHT_GRAY);

      //Display minor text
      offset_x += 2;
      offset_y += 21;
      lcd_print_string_small(button_list[active_button].minor_text, offset_x, offset_y, THEME_SUBTEXT, THEME_LIGHT_GRAY);

      //Display menu arrows
      offset_x = MENU1_ARROWS_X_OFFSET;
      offset_y =   MENU1_ARROWS_Y_OFFSET + (active_button*MENU1_BUTTON_SPACING) + SB_OFFSET;
      lcd_send_bitmap(menu_arrow_bmp, offset_x, offset_y, THEME_TEXT, THEME_LIGHT_GRAY);
   }
}


/*!
* @brief Display a personal profile similar to a cell phone's "contacts" menu
* @param[in] tmp_profile A structure providing the attributes of a given person
* @return  NONE
*/
void
gui_create_menu_type2(const t_person_profile tmp_profile)
{
   gui_set_footer_color(BLACK);
   gui_set_status_bar_color(GUI_HEADER_DARK);
   gui_create_title_bar("Profile");

   //Large profile picture. All profile pictures have the dimensions 320 x 164
   lcd_image_from_sd(0, HEADER_OFFSET, 320, HEADER_OFFSET + MENU2_PICTURE_HEIGHT, tmp_profile.profile_image);

   //Gray background for menu text
   lcd_draw_rectangle(THEME_NEAR_WHITE, 0, HEADER_OFFSET + MENU2_PICTURE_HEIGHT, 320, FOOTER_OFFSET);

   //Display main text
   uint16_t x_offset = 60;
   uint16_t y_offset = HEADER_OFFSET + MENU2_PICTURE_HEIGHT + MENU2_TEXT_OFFSET;
   lcd_print_string_small(tmp_profile.job_title, x_offset, y_offset, THEME_TEXT, THEME_NEAR_WHITE);
   y_offset += MENU2_TEXT_SPACING;
   lcd_print_string_small(tmp_profile.phone_number, x_offset, y_offset, THEME_TEXT, THEME_NEAR_WHITE);
   y_offset += MENU2_TEXT_SPACING;
   lcd_print_string_small(tmp_profile.email, x_offset, y_offset, THEME_TEXT, THEME_NEAR_WHITE);

   //Display subtext
   x_offset += 5;
   y_offset = HEADER_OFFSET + MENU2_PICTURE_HEIGHT + MENU2_SUBTEXT_OFFSET;
   lcd_print_string_small(tmp_profile.company, x_offset, y_offset, THEME_SUBTEXT, THEME_NEAR_WHITE);
   y_offset += MENU2_TEXT_SPACING;
   lcd_print_string_small("Mobile", x_offset, y_offset, THEME_SUBTEXT, THEME_NEAR_WHITE);
   y_offset += MENU2_TEXT_SPACING;
   lcd_print_string_small("Work", x_offset, y_offset, THEME_SUBTEXT, THEME_NEAR_WHITE);

   //Display bitmaps
   x_offset = 6;
   y_offset = HEADER_OFFSET + MENU2_PICTURE_HEIGHT + MENU2_TEXT_OFFSET;

   lcd_send_bitmap(job_icon_bmp, x_offset, y_offset, THEME_MAIN, THEME_NEAR_WHITE);
   y_offset += MENU2_TEXT_SPACING;
   lcd_send_bitmap(phone_icon_bmp, x_offset, y_offset, GREEN_PHONE, THEME_NEAR_WHITE);
   y_offset += MENU2_TEXT_SPACING;
   lcd_send_bitmap(mail_icon_bmp, x_offset, y_offset, RED_DARK, THEME_NEAR_WHITE);
}


/*!
* @brief Display a slide show consisting of either voice clips with pictures or voice clips with text
* @param[in] NONE
* @return  NONE
*/
void
gui_create_menu_type3(void)
{
   gui_set_footer_color(GRAY_MENU3);
   gui_set_status_bar_color(GRAY_MENU3);

   //If nothing else has triggered this function, it was a main or substate change or a slide change,
   //and the entire menu will need to be redrawn
   lcd_draw_rectangle(BLACK, 0, MENU3_UTILITIES_BAR_OFFSET, 320, FOOTER_OFFSET);

   for(e_menu3_button current_button = menu3_left_arrow; current_button < menu3_time_bar0; current_button++)
   {
      uint16_t x = menu3_template[current_button].x_position;
      uint16_t y = menu3_template[current_button].y_position;
      lcd_send_bitmap(menu3_template[current_button].bmp, x, y, THEME_MAIN, BLACK);
   }

   //Set the initial audio play button visual to "play" aka a triangle inside of a circle
   lcd_send_bitmap(play_button_inner_symbol, MENU3_PLAY_BUTTON_ICON_X_OFFSET, MENU3_PLAY_BUTTON_ICON_Y_OFFSET, WHITE_PURE, THEME_MAIN);
}


/*!
* @brief Display a menu (picture) of countries to select a language audio example
* @param[in] main_menu_image_address File address in the SD card of the main language menu picture
* @return  NONE
*
*/
void
gui_create_menu_languages(uint32_t main_menu_image_address)
{
   gui_set_footer_color(BLACK);
   gui_set_status_bar_color(GUI_HEADER_DARK);

   gui_create_title_bar("Languages");
   lcd_image_from_sd(0, HEADER_OFFSET, 320, FOOTER_OFFSET, main_menu_image_address);

   lcd_print_string_small("Choose a country", 10, HEADER_OFFSET + 10, THEME_TEXT, 0xBE9F);
   lcd_print_string_small("for a sample of", 10, HEADER_OFFSET + 25, THEME_TEXT, 0xBE9F);
   lcd_print_string_small("my language skills", 10, HEADER_OFFSET + 40, THEME_TEXT, 0xBE9F);
}


/*!
* @brief Draw the play button of a menu3, used to indicate the current audio state
* @param[in] NONE
* @return NONE
*
* @note This is generally used with a menu type 3
*/
void
gui_draw_play_button(void)
{
   uint16_t x = menu3_template[menu3_play_pause].x_position;
   uint16_t y = menu3_template[menu3_play_pause].y_position;
   lcd_send_bitmap(menu3_template[menu3_play_pause].bmp, x, y, THEME_MAIN, BLACK);
   lcd_send_bitmap(play_button_inner_symbol, MENU3_PLAY_BUTTON_ICON_X_OFFSET, MENU3_PLAY_BUTTON_ICON_Y_OFFSET, WHITE_PURE, THEME_MAIN);
}


/*!
* @brief Draw the pause button of a menu3, used to indicate the current audio state
* @param[in] NONE
* @return NONE
*
* @note This is generally used with a menu type 3
*/
void
gui_draw_pause_button(void)
{
   uint16_t x = menu3_template[menu3_play_pause].x_position;
   uint16_t y = menu3_template[menu3_play_pause].y_position;
   lcd_send_bitmap(menu3_template[menu3_play_pause].bmp, x, y, THEME_MAIN, BLACK);
   lcd_send_bitmap(pause_button_inner_symbol, MENU3_PAUSE_BUTTON_ICON_X_OFFSET, MENU3_PLAY_BUTTON_ICON_Y_OFFSET, WHITE_PURE, THEME_MAIN);
}


/*!
* @brief Display a profile of the author(Aaron Vorse) with contact info and website links
* @param[in] tmp_button_list A list of buttons with bitmaps and text to represent each contact element
* @return  NONE
*/
void
gui_create_menu_contact_info(const t_light_button *tmp_button_list)
{
   gui_set_footer_color(BLACK);
   gui_set_status_bar_color(GUI_HEADER_DARK);

   //Menu title
   gui_create_title_bar("Contact Info");

   //Profile picture
   uint16_t y_offset = MENU_CONTACT_PROFILE_OFFSET;
   lcd_image_from_sd(0, HEADER_OFFSET, 320, y_offset, tmp_button_list[4].image[0]);
   lcd_print_string(tmp_button_list[4].major_text, 153, 170, THEME_NEAR_WHITE, GRAY_DARK);

   //Text background
   lcd_draw_rectangle(THEME_NEAR_WHITE, 0, y_offset, 320, FOOTER_OFFSET);

   uint8_t x_offset = 0; //This needs to be visible outside of the main loop to provide the final text offset

   //Loop through the button list, creating the attributes of each
   for(uint8_t current_button = 0; current_button < 4; current_button++)
   {
      //Print picture
      uint32_t tmp_image = tmp_button_list[current_button].image[0];
      x_offset = PROFILE_BAR_ICON_X_OFFSET;

      if(null_address != tmp_image)
      {
         y_offset = MENU_CONTACT_PICTURE_OFFSET + (current_button*MENU_CONTACT_TEXT_SPACING);
         lcd_image_from_sd(x_offset, y_offset, x_offset + MENU_CONTACT_ICON_WIDTH, y_offset + MENU_CONTACT_ICON_WIDTH, tmp_image);
         x_offset += (PROFILE_BAR_ICON_X_OFFSET + MENU_CONTACT_ICON_WIDTH); //Offset the text from the picture
      }

      //Print bitmap
      const uint8_t *tmp_bmp =  tmp_button_list[current_button].bmp;

      if(NULL != tmp_bmp)
      {
         x_offset -= 5; //Bitmaps are right aligned and need to be shifted to match the pictures
         y_offset = MENU_CONTACT_PICTURE_OFFSET + (current_button*MENU_CONTACT_TEXT_SPACING);
         lcd_send_bitmap(tmp_bmp, x_offset, y_offset, GRAY_DARK, THEME_NEAR_WHITE);
         x_offset += (PROFILE_BAR_ICON_X_OFFSET + MENU_CONTACT_ICON_WIDTH + 5); //Offset the text from the bitmap
      }

      //Print Text
      y_offset += 10; //Center the text to the icon
      char *tmp_text =  tmp_button_list[current_button].major_text;
      lcd_print_string_small(tmp_text, x_offset, y_offset, THEME_TEXT, THEME_NEAR_WHITE);
   }

   //Print the final portion of the LinkedIn website link that wouldn't fit on a single line
   x_offset += 10; //Offset subtext from main text
   y_offset += 16;
   lcd_print_string_small(tmp_button_list[3].minor_text, x_offset, y_offset, THEME_TEXT, THEME_NEAR_WHITE);
}


/*!
* @brief Display a menu of the author's skills with bitmaps and text
* @param[in] tmp_button_list List of skills with bitmap attributes
* @param[in] length Variable to keep track of array length while passing by pointer
* @return  NONE
*/
void
gui_create_menu_skills(const t_light_button *tmp_button_list, size_t length)
{
   gui_set_footer_color(BLACK);
   gui_set_status_bar_color(GUI_HEADER_DARK);

   //Menu title
   gui_create_title_bar("Skills");

   //Text background
   lcd_draw_rectangle(GRAY_DARK, 0, HEADER_OFFSET, 320, FOOTER_OFFSET);

   //Loop through the button list, creating the attributes of each
   for(uint8_t current_button = 0; current_button < length; current_button++)
   {


      //Print picture
      uint32_t tmp_image = tmp_button_list[current_button].image[0];
      uint16_t x_offset = PROFILE_BAR_ICON_X_OFFSET;
      uint16_t y_offset = 0;

      if(null_address != tmp_image)
      {
         y_offset = MENU_SKILLS_PICTURE_Y_OFFSET + (current_button*MENU_SKILLS_TEXT_SPACING);
         lcd_image_from_sd(x_offset, y_offset, x_offset + MENU_SKILLS_ICON_WIDTH, y_offset + MENU_SKILLS_ICON_HEIGHT, tmp_image);
         x_offset += (PROFILE_BAR_ICON_X_OFFSET + MENU_SKILLS_ICON_WIDTH); //Offset the text from the picture
      }

      //Print Text
      char *tmp_text =  tmp_button_list[current_button].major_text;
      lcd_print_string_small(tmp_text, x_offset, y_offset, THEME_LIGHT_GRAY, GRAY_DARK);

      //Print the final portion of the LinkedIn website link that wouldn't fit on a single line
      x_offset += 10; //Offset subtext from main text
      y_offset += 16;
      lcd_print_string_small(tmp_button_list[current_button].minor_text, x_offset, y_offset, GRAY_MEDIUM, GRAY_DARK);
   }
}


/*!
* @brief Display the current audio play time in relation to the total time.
* @param[in] tmp_current_clock The amount of time that has passed since the audio started
* @param[in] tmp_total_time The total time the audio will take to complete
* @return NONE
*
* @note This is generally used with a menu type 3
*/
void
gui_draw_audio_clock(uint16_t tmp_current_time, uint16_t tmp_total_time)
{
   //Update current audio time
   char current_time_buffer[5] = {'0'};
   timers_uint16_to_time(tmp_current_time, current_time_buffer);

   //Update total audio time
   char total_time_buffer[5] = {'0'};
   timers_uint16_to_time(tmp_total_time, total_time_buffer);

   //Combine the two so that they are in the form    0:00 / 0:00
   char combined_time_buffer[12] = {"     /      "};
   combined_time_buffer[0] = current_time_buffer[0];
   combined_time_buffer[1] = current_time_buffer[1];
   combined_time_buffer[2] = current_time_buffer[2];
   combined_time_buffer[3] = current_time_buffer[3];

   combined_time_buffer[7] = total_time_buffer[0];
   combined_time_buffer[8] = total_time_buffer[1];
   combined_time_buffer[9] = total_time_buffer[2];
   combined_time_buffer[10] = total_time_buffer[3];
   combined_time_buffer[11] = total_time_buffer[4];  //Null character for strlen()

   lcd_print_string_small(combined_time_buffer, MENU3_TIMER_TEXT_X_OFFSET, MENU3_TIMER_TEXT_Y_OFFSET, THEME_NEAR_WHITE, BLACK);
}


/*!
* @brief Displays the amount of time that has passed since an audio clip
*        started playing in a menu type3
* @param[in] tmp_total_blocks Total number of address blocks in the SD card the audio
*                             file takes up
* @param[in] first_entry_flag This tells the function if this is  the first
*                             time it is being called for the current audio
* @return NONE
*
* @warning Make sure the FPU is enabled or this will cause a large amount of latency
*/
void
gui_update_menu3_clock(uint32_t tmp_total_blocks, uint8_t first_entry_flag)
{
   static uint16_t tmp_previous_clock = 1; //This cannot be '0', or it will not refresh upon entering a new slide
   uint16_t tmp_current_clock = timers_get_audio_clock();

   //Only redraw the current time if it has changed
   if((tmp_previous_clock != tmp_current_clock) || (1 == first_entry_flag))
   {
      //Update total audio time in seconds. FORMULA: (blocks x 512-bytes-per-block) / 22050-bytes-per-second-audio
      uint16_t tmp_total_time = (tmp_total_blocks * 512) / 22050;
      gui_draw_audio_clock(tmp_current_clock, tmp_total_time);

      //Clear the previous timer bar from the screen
      lcd_draw_rectangle(BLACK, MENU3_TIMER_BAR_X_START, MENU3_TIMER_BAR_Y_OFFSET - 6, MENU3_TIMER_BAR_X_END, MENU3_TIMER_BAR_Y_OFFSET + 8);

      //Re-map the total WAV runtime to the 200-pixel-wide timer bar
      float delta_x = (200 / tmp_total_time)*tmp_current_clock; //Floating point needed to avoid rounding errors
      uint8_t offset = (uint8_t)(delta_x);

      //Bounds check to make sure timer bar doesn't go past 200 pixels
      if(MENU3_TIMER_BAR_X_END < (offset + MENU3_MARKER_WIDTH + MENU3_TIMER_BAR_X_START))
      {
         offset = MENU3_TIMER_BAR_X_END - MENU3_MARKER_WIDTH;
      }

      else
      {
         offset += MENU3_TIMER_BAR_X_START;
      }

      //Write the new timer bar to the screen
      lcd_draw_rectangle(PINK, MENU3_TIMER_BAR_X_START, MENU3_TIMER_BAR_Y_OFFSET, offset, MENU3_TIMER_BAR_Y_OFFSET + 2);
      lcd_draw_rectangle(THEME_LIGHT_GRAY, offset, MENU3_TIMER_BAR_Y_OFFSET, MENU3_TIMER_BAR_X_END, MENU3_TIMER_BAR_Y_OFFSET + 2);
      lcd_send_bitmap(sliding_bar_marker_bmp, offset, MENU3_MARKER_Y_OFFSET, PINK, BLACK);
   }

   tmp_previous_clock = tmp_current_clock;
}


/*!
* @brief Displays a pop-up warning message to the user
* @param[in] tmp_buttons List of maximum two buttons to display as options
* @param[in] strings Text to display on the menu as well as inside the buttons
* @return NONE
*
* @note strings should be in the following form
*      char *example[5] = { "left button text", "right button text",
*                           "main warning text", "minor text line 0", "minor text line 1"};
*
*/
void
gui_create_warning_menu( t_button *tmp_buttons, char **strings)
{
   //Background. This starts at x = 10 and ends at x = 310 aka 10 pixels offset from the edges
   lcd_draw_rectangle(GRAY_MEDIUM_DARK, 10, MENU_WARNING_BACKGROUND_OFFSET, 310, MENU_WARNING_BACKGROUND_HEIGHT);

   uint16_t y_offset = MENU_WARNING_BACKGROUND_OFFSET + 25;

   //Main warning text
   lcd_print_string(strings[2], 70, y_offset, BLUE_SKY, GRAY_MEDIUM_DARK);

   //Line dividing major text and the rest of the menu
   y_offset += 40;
   lcd_draw_rectangle(BLUE_SKY, 10, y_offset, 310, y_offset + 2);

   //Minor text
   y_offset += 20;
   lcd_print_string_small(strings[3], 20, y_offset, THEME_LIGHT_GRAY, GRAY_MEDIUM_DARK);
   y_offset += 20;
   lcd_print_string_small(strings[4], 20, y_offset, THEME_LIGHT_GRAY, GRAY_MEDIUM_DARK);

   buttons_create(tmp_buttons, strings, 2); //There are ever only 2 buttons in this menu
}


/*!
* @brief Displays a menu for users to change basic device settings
* @param[in] NONE
* @return NONE
*
*/
void
gui_create_settings_menu(void)
{
   //Background. This starts at x = 10 and ends at x = 310 aka 10 pixels offset from the edges
   lcd_draw_rectangle(GRAY_MEDIUM_DARK, 10, MENU_WARNING_BACKGROUND_OFFSET, 310, MENU_WARNING_BACKGROUND_HEIGHT);

   //Main warning text
   uint16_t y_offset = MENU_WARNING_BACKGROUND_OFFSET + 25;
   uint16_t x_offset = 70;
   lcd_print_string("Settings", x_offset, y_offset, BLUE_SKY, GRAY_MEDIUM_DARK);

   //Line dividing major text and the rest of the menu
   x_offset = 10;
   y_offset += 40;
   lcd_draw_rectangle(BLUE_SKY, 10, y_offset, 310, y_offset + 2);

   //Minor text
   x_offset += 20;
   y_offset += 20;
   lcd_print_string_small("Sound", x_offset, y_offset, WHITE_PURE, GRAY_MEDIUM_DARK);

   //Toggle button
   y_offset += 40;
   lcd_print_string_small("Volume", x_offset, y_offset, WHITE_PURE, GRAY_MEDIUM_DARK);
   lcd_print_string_small("-            +", x_offset + 110, y_offset + 5, GRAY_MEDIUM, GRAY_MEDIUM_DARK);
   gui_settings_menu_update_volume();

   y_offset += 40;
   lcd_print_string_small("Battery", x_offset, y_offset, WHITE_PURE, GRAY_MEDIUM_DARK);
   gui_settings_menu_update_battery();

   y_offset += 40;
   lcd_print_string_small("Time", x_offset, y_offset, WHITE_PURE, GRAY_MEDIUM_DARK);
   lcd_print_string_small("Set          >", MENU_SETTINGS_BUTTON_X_OFFSET, y_offset + 2, GRAY_MEDIUM, GRAY_MEDIUM_DARK);

}


/*!
* @brief Redraw only the buttons and variable elements of the settings menu
* @param[in] NONE
* @return NONE
*
* @note Not redrawing the whole menu helps with flicker
*/
void
gui_update_settings_menu(void)
{
   //Toggle button
   gui_settings_menu_update_volume();
}


/*!
* @brief Displays a menu for users to set the current RTC time
* @param[in] NONE
* @return NONE
*
*/
void
gui_create_settings_time_menu(void)
{
   //Background. This starts at x = 10 and ends at x = 310 aka 10 pixels offset from the edges
   lcd_draw_rectangle(GRAY_MEDIUM_DARK, 10, MENU_WARNING_BACKGROUND_OFFSET, 310, MENU_WARNING_BACKGROUND_HEIGHT);

   //Main warning text
   uint16_t y_offset = MENU_WARNING_BACKGROUND_OFFSET + 25;
   uint16_t x_offset = 70;
   lcd_print_string("Time Set", x_offset, y_offset, BLUE_SKY, GRAY_MEDIUM_DARK);

   //Line dividing major text and the rest of the menu
   x_offset = 10;
   y_offset += 40;
   lcd_draw_rectangle(BLUE_SKY, x_offset, y_offset, x_offset + 300, y_offset + 2);

   //Display time upon menu entry
   char tmp_string[6] = {0};
   gui_rtc_to_string(tmp_string); //Pull current time from the RTC

   x_offset += 46;
   y_offset += 50;
   lcd_print_string(tmp_string, x_offset, y_offset, GRAY_EXTRA_LIGHT, GRAY_MEDIUM_DARK);

   //Navigation arrows
   x_offset += 144;
   lcd_print_string_small("<     >", x_offset, y_offset, GRAY_EXTRA_LIGHT, GRAY_MEDIUM_DARK);

   x_offset += 29;
   lcd_send_bitmap(sliding_bar_marker_bmp, x_offset, y_offset, GRAY_EXTRA_LIGHT, GRAY_MEDIUM_DARK);
   x_offset += 3;
   lcd_send_bitmap(up_arrow_bmp, x_offset, y_offset - 28, GRAY_EXTRA_LIGHT, GRAY_MEDIUM_DARK);
   lcd_send_bitmap(down_arrow_bmp, x_offset, y_offset + 34, GRAY_EXTRA_LIGHT, GRAY_MEDIUM_DARK);

   //Define button properties for the "set time" submenu
   static char *menu_text[2] =
   {
         "Cancel\0",
         "Set\0",
   };

   //Only create the first two buttons from the "set time" sebumenu list, aka the "cancel" and "set" buttons
   buttons_create(settings_time_menu_template, menu_text, 2);

   uint8_t tmp_initial_time[4] = {1, 2, 0, 0};
   gui_update_settings_time_menu(tmp_initial_time, 0); //Initialize the time displayed
}


/*!
* @brief Redraw only the time display of the time settings menu
* @param[in] time_buffer Array containing the integer form of the user input time
*                        Format: hours_tens, hours, minutes_tens,minutes
* @param[in] current_place The place (for example tens_minutes) that the user is currently editing
* @return NONE
*
* @note Not redrawing the whole menu helps with flicker
*/
void
gui_update_settings_time_menu(uint8_t *time_buffer, uint8_t current_place)
{
   uint8_t designator_xoffset[3] = {64, 97,108}; //Location of each time character in order to underline current_place

   lcd_draw_rectangle(GRAY_MEDIUM_DARK, 50, 230, 170, 232);
   lcd_draw_rectangle(BLUE_SKY, designator_xoffset[current_place], 230, designator_xoffset[current_place] + 10, 232);

   //Update the set time based on the user input
   char tmp_string[6] = {0};
   gui_time_to_string(tmp_string, time_buffer);
   lcd_print_string(tmp_string, MENU_SETTINGS_TIME_X_OFFSET, MENU_SETTINGS_TIME_Y_OFFSET, GRAY_EXTRA_LIGHT, GRAY_MEDIUM_DARK);
}


/*!
* @brief Displays the main menu for the "about_me" app
* @param[in] tmp_button_images An array containing the SD card memory addresses of each button image
* @return NONE
*
*/
void
gui_create_aboutme_main_menu(uint32_t *tmp_button_images)
{
   //Black background of the menu
   lcd_draw_rectangle(BLACK, 0, SB_OFFSET, 320, FOOTER_OFFSET);

   //Create the 5 main user buttons
   char *tmp_text[5] = {NULL, NULL, NULL, NULL, NULL};
   buttons_create(about_me_menu_template, tmp_text, 5);

   //Add the corresponding images and text to each button. The "magic numbers" here are specific
   //locations which were tuned by trial and error.
   lcd_image_from_sd(40, SB_OFFSET + 46, 112, SB_OFFSET + 96, tmp_button_images[0]);    //72  x 50
   lcd_image_from_sd(220, SB_OFFSET + 46, 260, SB_OFFSET + 152, tmp_button_images[1]);  //40  x 106
   lcd_image_from_sd(30, SB_OFFSET + 170, 134, SB_OFFSET + 290, tmp_button_images[2]);  //104 x 120
   lcd_image_from_sd(215, SB_OFFSET + 210, 295, SB_OFFSET + 286, tmp_button_images[3]); //80  x 76
   lcd_image_from_sd(25, SB_OFFSET + 340, 305, SB_OFFSET + 404, tmp_button_images[4]);  //280 x 64

   lcd_print_string("Education", 25, SB_OFFSET + 17, WHITE_PURE, GRAY_DARK_ALT);
   lcd_print_string("Goals", 210, SB_OFFSET + 17, WHITE_PURE, GRAY_DARK_ALT);
   lcd_print_string("Hobbies", 35, SB_OFFSET + 140, WHITE_PURE, GRAY_DARK_ALT);
   lcd_print_string("Interests", 180, SB_OFFSET + 185, WHITE_PURE, GRAY_DARK_ALT);
   lcd_print_string("Experience", 25, SB_OFFSET + 325, WHITE_PURE, GRAY_DARK_ALT);
}


/*!
* @brief Displays the education menu for the "about_me" app
* @param[in] NONE
* @return NONE
*
*/
void
gui_create_aboutme_education_menu(void)
{
   //Black background of the menu
   lcd_draw_rectangle(BLACK, 0, MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET, 320, FOOTER_OFFSET);
   lcd_draw_rectangle(PINK, 0, MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET, 5, FOOTER_OFFSET);

   uint16_t x_offset = MENU_ABOUTME_SUBMENU_TEXT_X_OFFSET;
   uint16_t y_offset = MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET + 20;

   //Submenu Title
   lcd_print_string("EDUCATION", x_offset, y_offset, PINK, BLACK);
   y_offset += 40;

   //Display degrees earned at CSULB
   lcd_print_string("University X", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += 30;
   lcd_print_string_small("Degree 1", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += 20;
   lcd_print_string_small("Details", x_offset, y_offset, GRAY_MEDIUM, BLACK);
   y_offset += 25;
   lcd_print_string_small("Degree 1", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += 40;

   //Display study-abroad universities attended
   lcd_print_string("Special Program", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += 30;
   lcd_print_string_small("University 1", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += 25;
   lcd_print_string_small("University 2", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += 25;
   lcd_print_string_small("University 3", x_offset, y_offset, GRAY_LIGHT, BLACK);

}


/*!
* @brief Displays the goals menu for the "about_me" app
* @param[in] NONE
* @return NONE
*
*/
void
gui_create_aboutme_goals_menu(void)
{
   //Black background of the menu
   lcd_draw_rectangle(BLACK, 0, MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET, 320, FOOTER_OFFSET);
   lcd_draw_rectangle(PINK, 0, MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET, 5, FOOTER_OFFSET);

   uint16_t x_offset = MENU_ABOUTME_SUBMENU_TEXT_X_OFFSET;
   uint16_t y_offset = MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET + 20;

   //Submenu Title
   lcd_print_string("GOALS", x_offset, y_offset, PINK, BLACK);
   y_offset += 30;

   uint8_t line_spacing = 20;

   //Display first paragraph
   lcd_print_string_small("Short-term, I plan to", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("become a professional", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("embedded systems engineer", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("and later obtain an MSCS", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += 30;

   //Display second paragraph
   lcd_print_string_small("More importantly,I believe", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("that we all make an impact", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("on this world,and over the", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("course of my career and my", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("life,I will strive to", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("leave my tiny piece just a", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("little bit better", x_offset, y_offset, WHITE_PURE, BLACK);
}


/*!
* @brief Displays the hobbies menu for the "about_me" app
* @param[in] NONE
* @return NONE
*
*/
void
gui_create_aboutme_hobbies_menu(void)
{
   //Black background of the menu
   lcd_draw_rectangle(BLACK, 0, MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET, 320, FOOTER_OFFSET);
   lcd_draw_rectangle(PINK, 0, MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET, 5, FOOTER_OFFSET);

   uint16_t x_offset = MENU_ABOUTME_SUBMENU_TEXT_X_OFFSET;
   uint16_t y_offset = MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET + 20;

   //Submenu Title
   lcd_print_string("HOBBIES", x_offset, y_offset, PINK, BLACK);
   y_offset += 40;

   uint8_t line_spacing = 20;

   //Display first paragraph
   //                     "embedded systems engineer"
   lcd_print_string_small("Outside of professional", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("work,I like to design", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("interesting personal", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("electronics gadgets", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += 30;

   //Display second paragraph
   lcd_print_string_small("I also enjoy reading old", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("German literature and", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("picking up new languages.", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("I love all things water-", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("related and hold a CA", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("sailing certificate.", x_offset, y_offset, WHITE_PURE, BLACK);
}


/*!
* @brief Displays the interests menu for the "about_me" app
* @param[in] NONE
* @return NONE
*
*/
void
gui_create_aboutme_interests_menu(void)
{
   //Black background of the menu
   lcd_draw_rectangle(BLACK, 0, MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET, 320, FOOTER_OFFSET);
   lcd_draw_rectangle(PINK, 0, MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET, 5, FOOTER_OFFSET);

   uint16_t x_offset = MENU_ABOUTME_SUBMENU_TEXT_X_OFFSET;
   uint16_t y_offset = MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET + 20;

   //Submenu Title
   lcd_print_string("INTERESTS", x_offset, y_offset, PINK, BLACK);
   y_offset += 35;

   uint8_t line_spacing = 20;

   //Display first paragraph
   lcd_print_string_small("The following is a list", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("of sectors within embedded", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("systems that I find", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("particularly interesting", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += 32;

   line_spacing = 28;

   //Display sector list
   lcd_print_string_small("-Autonomous  -Cube Sats", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small(" Vehicles    -Iot", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("-Automotive  -Robotics", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("-Aviation    -Smart Agg", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += line_spacing;
   lcd_print_string_small("-Consumer    -Space", x_offset, y_offset, GRAY_LIGHT, BLACK);

}


/*!
* @brief Displays the experience menu for the "about_me" app
* @param[in] NONE
* @return NONE
*
*/
void
gui_create_aboutme_experience_menu(void)
{
   //Black background of the menu
   lcd_draw_rectangle(BLACK, 0, MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET, 320, FOOTER_OFFSET);
   lcd_draw_rectangle(PINK, 0, MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET, 5, FOOTER_OFFSET);

   uint16_t x_offset = MENU_ABOUTME_SUBMENU_TEXT_X_OFFSET;
   uint16_t y_offset = MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET + 20;

   //Submenu Title
   lcd_print_string("EXPERIENCE", x_offset, y_offset, PINK, BLACK);
   y_offset += 30;

   //Display previous work experience
   lcd_print_string_small("Company 1", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += 20;
   lcd_print_string_small("Company 1 Details", x_offset, y_offset, GRAY_MEDIUM, BLACK);
   y_offset += 35;

   lcd_print_string_small("Company 2", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += 20;
   lcd_print_string_small("Company 2 Details", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += 35;

   lcd_print_string_small("Company 3", x_offset, y_offset, WHITE_PURE, BLACK);
   y_offset += 20;
   lcd_print_string_small("Company 3 Details", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += 20;
   lcd_print_string_small("Company 3 Details", x_offset, y_offset, GRAY_MEDIUM, BLACK);
   y_offset += 20;
   lcd_print_string_small("Company 3 Details", x_offset, y_offset, GRAY_MEDIUM, BLACK);

}


/*!
* @brief Displays a waiting "spinning circle" animation while the device is shutting down
* @param[in] x The X midpoint of the circle
* @param[in] y The Y midpoint of the circle
* @return NONE
*
* @warning This function locks the processor with delay functions. It should never be used in multithreaded applications.
* @note This is not the fastest implementation, but by definition (it's a loading screen animation)
*       latency is usually not a problem
*/
void
gui_loading_circle_animation(uint16_t x, uint16_t y)
{
   uint8_t radius = 30;
   uint8_t d_radius = (((sqrt(2)) / 2) * radius); //Each leg of a 45-45-90 triangle is sqrt(2) / 2

   //Lookup tables for each small circle position
   const uint16_t x_pos[8] = {(x - d_radius), (x - radius ), (x - d_radius), x,
                              (x + d_radius), x + radius, (x + d_radius), x};

   const uint16_t y_pos[8] = {(y - d_radius), y, (y + d_radius), (y + radius),
                              (y + d_radius), y, (y - d_radius), (y - radius)};

   //Color table for each mini circle in the spectrum of white to black
   const uint16_t color_buffer[8] = {WHITE_PURE, GRAY_EXTRA_LIGHT, GRAY_LIGHT, GRAY_LIGHT, GRAY_MEDIUM, GRAY_MEDIUM_DARK, GRAY_DARK_ALT, BLACK};
   uint8_t color_number = 0;


   //Each cycle represents redrawing all 8 mini circles with their updated colors
   for(uint8_t cycle = 0; cycle < 28; cycle++)
   {
      //Draw all 8 mini circles
      for(uint8_t mini_circle = 0; mini_circle < 8; mini_circle++)
      {

         lcd_send_bitmap(sliding_bar_marker_bmp, x_pos[mini_circle], y_pos[mini_circle], color_buffer[color_number], BLACK);

         //Update color index
         if(7 > color_number)
         {
            color_number++;
         }

         else
         {
            color_number = 0;
         }

      }

      //Increment color index so that the animation appears to be spinning
      if(7 > color_number)
      {
         color_number++;
      }

      else
      {
         color_number = 0;
      }

      //Display each cycle for roughly 500mS
      timers_delay(500);
   }

}


/*!
* @brief Displays the boot animation while the system starts up
* @param[in] animation_frames List of addresses in the SD card that hold the animation bitmaps
* @param[in] number_of_frames
* @return NONE
*
*/
void
gui_boot_animation(uint32_t *animation_frames, size_t number_of_frames)
{
   //Clear screen to start animation on a blank background
   lcd_draw_rectangle(BLACK, 0, 0, 320, 480);
   lcd_invert_screen_on();
   lcd_backlight_enable();
   timers_delay(4000);

   uint16_t x_offset = 92, y_offset = 150;

   for(uint8_t rotations = 0; rotations < 3; rotations++)
   {
      for(uint8_t current_frame = 0; current_frame < (number_of_frames - 1); current_frame++)
      {
         //Loop through all frames except for the last, which is the "EMBEDDED RESUME" title
         lcd_image_from_sd(x_offset, y_offset, x_offset + 136, y_offset + 200, animation_frames[current_frame]);
         timers_delay(300);
      }
   }

   timers_delay(4000);
   lcd_invert_screen_off(); //Bring screen back to "normal" display mode
   timers_delay(7000);

   x_offset = 27;
   y_offset = 65;
   lcd_image_from_sd(x_offset, y_offset, x_offset + 268, y_offset + 40, animation_frames[12]); //Display "EMBEDDED RESUME" title
   timers_delay(7000);

   x_offset += 44;
   y_offset += 35;
   lcd_print_string("by Aaron Vorse", x_offset, y_offset, GRAY_LIGHT, BLACK);
   timers_delay(7000);

}


/*!
* @brief Displays the introduction popup message
* @param[in] tmp_startup_status_flag Flag telling the button handler to display or erase a check mark
* @return NONE
*
*
*/
void
gui_create_intro_menu(uint8_t tmp_startup_status_flag)
{
   //Background. This starts at x = 10 and ends at x = 310 aka 10 pixels offset from the edges
   lcd_draw_rectangle(GRAY_MEDIUM_DARK, 10, MENU_WARNING_BACKGROUND_OFFSET, 310, MENU_WARNING_BACKGROUND_HEIGHT);

   uint16_t y_offset = MENU_WARNING_BACKGROUND_OFFSET + 25;

   //Main warning text
   lcd_print_string("INTRODUCTION", 70, y_offset, BLUE_SKY, GRAY_MEDIUM_DARK);

   //Line dividing major text and the rest of the menu
   y_offset += 40;
   uint8_t x_offset = 10;
   lcd_draw_rectangle(BLUE_SKY, x_offset, y_offset, 310, y_offset + 2);

   //Create buttons
   x_offset += 15;
   y_offset += 40;

   char *menu_text[3] =
   {
      "Skip\0",
      "Listen\0",
      NULL,
   };

   buttons_create(intro_menu_template, menu_text, (sizeof(intro_menu_template) / sizeof(*intro_menu_template)));

   //Check box if startup_status_flag is set
   if(tmp_startup_status_flag)
   {
      lcd_send_bitmap(check_mark_bmp, MENU_INTRO_CHECK_X_OFFSET, MENU_INTRO_CHECK_Y_OFFSET, GRAY_MEDIUM_DARK, GRAY_LIGHT);
   }

   //Minor text
   x_offset += 35;
   lcd_print_string_small("Do not show this", x_offset, y_offset, THEME_LIGHT_GRAY, GRAY_MEDIUM_DARK);
   y_offset += 20;
   lcd_print_string_small("message on startup", x_offset, y_offset, THEME_LIGHT_GRAY, GRAY_MEDIUM_DARK);
}


/*!
* @brief Display the main production test screen
* @param[in] NONE
* @return  NONE
*/
void
gui_tests_create_main_menu(void)
{
   //Create the production test main title header and body
   lcd_draw_rectangle(GRAY_MEDIUM_DARK, 0, 0, 320, GUI_TESTS_HEADER_HEIGHT);
   lcd_draw_rectangle(BLACK, 0, GUI_TESTS_HEADER_HEIGHT, 320, 480);

   uint16_t x_offset = 10, y_offset = 30;
   lcd_print_string("PRODUCTION TESTING", x_offset, y_offset, WHITE_PURE, GRAY_MEDIUM_DARK);
}


/*!
* @brief Display a screen to let the user know the currrent production test has passed
* @param[in] NONE
* @return  NONE
*/
void
gui_tests_display_pass_screen(void)
{
   //Turn screen green
   lcd_draw_rectangle(GREEN_NEON, 0, GUI_TESTS_HEADER_HEIGHT, 320, 480);
   uint16_t x_offset = 80, y_offset = (GUI_TESTS_HEADER_HEIGHT + 200);
   lcd_print_string("TEST PASSED", x_offset, y_offset, BLACK, GREEN_NEON);
   timers_delay(3500);
}


/*!
* @brief Display a menu which coincides with the production USB testing
* @param[in] NONE
* @return  NONE
*/
void
gui_tests_create_usb_menu(void)
{
   //Display test title
   uint16_t x_offset = 10, y_offset = GUI_TESTS_TITLE_Y_OFFSET;
   lcd_print_string("USB Test", x_offset, y_offset, GRAY_MEDIUM, BLACK);

   y_offset += 60;
   lcd_print_string_small("Please plug in USB", x_offset, y_offset, GRAY_LIGHT, BLACK);

}

/*!
* @brief Display a menu which coincides with the production battery level testing
* @param[in] voltage_test_number Variable to select which of the 5 possible voltage levels to test
* @return  NONE
*/
void
gui_tests_create_battery_menu(uint8_t voltage_test_number)
{

   //Create the menu backdrop
   gui_tests_create_main_menu();

   //Display test title
   uint16_t x_offset = 30, y_offset = GUI_TESTS_TITLE_Y_OFFSET;
   lcd_print_string("Battery Level Test\0", x_offset, y_offset, GRAY_MEDIUM, BLACK);

   y_offset += 60;
   lcd_print_string_small("Turn battery selector to \0", x_offset, y_offset, GRAY_LIGHT, BLACK);

   char test_voltage[5][6] =
   {
      {" OFF \0"}, {" 2.1V\0"}, {" 2.4V\0"}, {" 2.6V\0"}, {" 3.1V\0"}
   };

   //Display the current voltage being tested and send it to the command terminal
   y_offset += 30;
   lcd_print_string(test_voltage[voltage_test_number], x_offset, y_offset, GRAY_LIGHT, BLACK);
   uart1_printf(test_voltage[voltage_test_number]);

}


/*!
* @brief Display a menu which coincides with the production battery level testing
* @param[in] voltage_test_number Variable to select which of the 5 possible touch markers to test
* @return  NONE
*/
void
gui_tests_create_touch_menu(uint8_t touch_test_number)
{

   //Create the menu backdrop
   gui_tests_create_main_menu();

   //Display test title
   uint16_t x_offset = 30, y_offset = GUI_TESTS_TITLE_Y_OFFSET;
   lcd_print_string("Touchscreen Test", x_offset, y_offset, GRAY_MEDIUM, BLACK);
   y_offset += 40;
   lcd_print_string_small("Please click the \"x\"\0", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += 20;
   lcd_print_string_small("marker below \0", x_offset, y_offset, GRAY_LIGHT, BLACK);


   size_t total_targets = (sizeof(touch_targets_template) / sizeof(*touch_targets_template));

   //Turn only the currently active target red and all others the background color
   for(uint8_t target_number = 0; target_number < total_targets; target_number++)
   {
      //Detect the active target
      if(touch_test_number == target_number)
      {
         touch_targets_template[target_number].text_color = RED_PURE;
      }

      else
      {
         touch_targets_template[target_number].text_color = BLACK;
      }
   }

   //Draw all targets (although only the currently active one will be a visible red)
   char *filler_text[5] = { "x\0", "x\0", "x\0", "x\0", "x\0"};
   buttons_create(touch_targets_template, filler_text, total_targets);
}


/*!
* @brief Display a menu which coincides with the production main power button testing
* @param[in] NONE
* @return  NONE
*/
void
gui_tests_create_power_button_menu(void)
{
   //Create the menu backdrop
   gui_tests_create_main_menu();

   //Display test title
   uint16_t x_offset = 30, y_offset = GUI_TESTS_TITLE_Y_OFFSET;
   lcd_print_string("Power Button Test", x_offset, y_offset, GRAY_MEDIUM, BLACK);
   y_offset += 40;
   lcd_print_string_small("Please press the main\0", x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += 20;
   lcd_print_string_small("power button\0", x_offset, y_offset, GRAY_LIGHT, BLACK);

}

/*!
* @brief Display a menu which coincides with the production LCD switch testing
* @param[in] seconds_remaining Time until the LCD backlight is disabled
* @return  NONE
*/
void
gui_tests_create_LCD_switch_menu(uint8_t seconds_remaining)
{

   //Create the menu backdrop
   gui_tests_create_main_menu();

   //Display test title
   uint16_t x_offset = 30, y_offset = GUI_TESTS_TITLE_Y_OFFSET;
   lcd_print_string("LCD Backlight Test", x_offset, y_offset, GRAY_MEDIUM, BLACK);

   y_offset += 60;
   lcd_print_string_small("LCD screen will reset in \0", x_offset, y_offset, GRAY_LIGHT, BLACK);

   char time[5][2] =
   {
      {"0\0"}, {"1\0"}, {"2\0"}, {"3\0"}, {"4\0"}
   };


   x_offset += 100;
   y_offset += 30;
   lcd_print_string(time[seconds_remaining], x_offset, y_offset, GRAY_LIGHT, BLACK);

}


/*!
* @brief Display a menu which coincides with the production audio testing
* @param[in] test_number Variable to determine the appropriate screen to display
* @return  NONE
*/
void
gui_tests_create_audio_menu(uint8_t test_number)
{
   //Create the menu backdrop
   gui_tests_create_main_menu();

   char main_text[2][20] = {"Plug in headphones\0", "Audio is playing\0"};
   char sub_text[2][22] = {"                   \0", "Listen for test audio\0"};

   //Display title
   uint16_t x_offset = 30, y_offset = GUI_TESTS_TITLE_Y_OFFSET;
   lcd_print_string("Audio Test", x_offset, y_offset, GRAY_MEDIUM, BLACK);

   //Give the user instructions depending on the current test
   y_offset += 40;
   lcd_print_string_small(main_text[test_number], x_offset, y_offset, GRAY_LIGHT, BLACK);
   y_offset += 20;
   lcd_print_string_small(sub_text[test_number], x_offset, y_offset, GRAY_LIGHT, BLACK);

}


/*
****************************************************
********** Private Function Definitions ************
****************************************************
*/

/*!
* @brief Get the current color of the status bar, usually to determine if the
*        status bar needs be redrawn.
* @param[in] NONE
* @return  status_bar_background_color
*/
uint16_t
gui_get_status_bar_color(void)
{
   return(status_bar_background_color);
}


/*!
* @brief Update the battery icon in the status bar
* @param[in] background_color_change_flag indicates if the status bar has changed colors
* @return  NONE
*/
void
gui_update_status_bar_battery(uint8_t background_color_change_flag)
{
   //Only redraw the battery once every 10 seconds when the RTC alarm triggers. This helps with LCD flickering
   if((RTC->ISR & RTC_ISR_ALRAF) || background_color_change_flag)
   {
      uint8_t tmp_current_level = monitor_get_battery_level(BATTERY_SAMPLE_FLAG_SAMPLE);

      char tmp_string[11] = {' '};
      pft_uint32_to_string(tmp_current_level, tmp_string);
      char converted_string[4] = {"  %\0"};
      converted_string[0] = tmp_string[8];
      converted_string[1] = tmp_string[9];

      //Take out preceding "0" if battery percentage is less than 10%
      if('0'== converted_string[0])
      {
         converted_string[0] = ' ';
      }

      //Print battery icon and percentage
      lcd_print_string_small(converted_string, SB_BATTERY_TEXT_X_OFFSET, SB_BATTERY_TEXT_Y_OFFSET, THEME_NEAR_WHITE, gui_get_status_bar_color());

      uint16_t battery_color = THEME_NEAR_WHITE;

      if(10 >= tmp_current_level)
      {
         battery_color = RED_LIGHT;
      }

      lcd_send_bitmap(battery_icon_bmp, SB_BATTERY_X_OFFSET, SB_BATTERY_Y_OFFSET, battery_color, gui_get_status_bar_color());

      //Fill in battery icon with current percentage
      uint16_t battery_fill = ((15 * tmp_current_level) / 100); //Formula: ((15 pixel rows in the battery) / 100_percent) * current_percentage
      battery_fill = SB_BATTERY_FILL_Y_OFFSET - battery_fill; //Adjust number of active rows to rows above the bottom of the battery icon

      lcd_draw_rectangle(THEME_NEAR_WHITE, SB_BATTERY_X_OFFSET, battery_fill, SB_BATTERY_X_OFFSET + 10, SB_BATTERY_FILL_Y_OFFSET);
   }

}


/*!
* @brief Update the speaker icon on the status bar
* @param[in] background_color_change_flag This tells the function if the background color of the
*                                         status bar has changed, meaning the speaker icon has been
*                                         rewritten. A value of "1" means it has changed
* @return  NONE
*/
void
gui_update_status_bar_volume(uint8_t background_color_change_flag)
{
   static e_dac_volume_type previous_volume_level = volume_on_init;
   e_dac_volume_type current_volume = dac_get_volume();

   //Only redraw the volume if level has changed. This helps with LCD flickering
   if((previous_volume_level != current_volume) || (1 == background_color_change_flag))
   {
      switch(current_volume)
      {
      case volume_on_init:
         break;
      case volume_muted:
         lcd_send_bitmap(speaker_muted, SB_SPEAKER_X_OFFSET, SB_SPEAKER_Y_OFFSET, THEME_NEAR_WHITE, gui_get_status_bar_color());
         break;
      case volume_low:
         lcd_send_bitmap(speaker_low, SB_SPEAKER_X_OFFSET, SB_SPEAKER_Y_OFFSET, THEME_NEAR_WHITE,gui_get_status_bar_color());
         break;
      case volume_medium:
         lcd_send_bitmap(speaker_medium,SB_SPEAKER_X_OFFSET, SB_SPEAKER_Y_OFFSET, THEME_NEAR_WHITE,gui_get_status_bar_color());
         break;
      case volume_high:
         lcd_send_bitmap(speaker_high,SB_SPEAKER_X_OFFSET, SB_SPEAKER_Y_OFFSET, THEME_NEAR_WHITE,gui_get_status_bar_color());
         break;
      default:
         break;

      }

   }

   previous_volume_level = current_volume;
}


/*!
* @brief Redraw the time icon if a change to the status bar has occurred
* @param[in] background_color_change_flag Parameter passed in to tell the function
*                                         whether a state transition has occurred,
*                                         meaning the status bar must also be updated.
*                                         A value of "1" means that it has changed.
* @return  NONE
*/
void
gui_update_status_bar_time(uint8_t background_color_change_flag)
{

   //Only read RTC every 10 seconds when the alarm triggers
   if((RTC->ISR & RTC_ISR_ALRAF) || background_color_change_flag)
   {
      char tmp_string[6] = {0};

      gui_rtc_to_string(tmp_string);
      lcd_print_string_small(tmp_string, SB_TIME_X_OFFSET, SB_TIME_Y_OFFSET, THEME_NEAR_WHITE, gui_get_status_bar_color());
      rtc_update_alarm(); //Update alarm value to trigger at next 10 second overflow

   }
}


/*!
* @brief Redraw the headphone icon if a change to the status bar has occurred
* @param[in] background_color_change_flag Parameter passed in to tell the function
*                                         whether a state transition has occurred,
*                                         meaning the status bar must also be updated.
*                                         A value of "1" means that it has changed.
* @return  NONE
*/
void
gui_update_status_bar_headphones(uint8_t background_color_change_flag)
{
   static uint8_t previous_headphones_status = 0;
   uint8_t current_headphone_status = monitor_get_headphone_status();

   //Only redraw the icon if the headphone status has changed. This helps with LCD flickering
   if((previous_headphones_status != current_headphone_status) || (1 == background_color_change_flag))
   {
      if(1 == current_headphone_status)
      {
         lcd_send_bitmap(headphone_icon_bmp, SB_HEADPHONES_X_OFFSET, SB_HEADPHONES_Y_OFFSET, THEME_NEAR_WHITE, gui_get_status_bar_color()); //Display Icon
      }

      else
      {
         lcd_draw_rectangle(gui_get_status_bar_color(), SB_HEADPHONES_X_OFFSET, 0, SB_HEADPHONES_X_OFFSET + 20, SB_OFFSET); //Clear icon if there are no headphones
      }

   }

   previous_headphones_status = current_headphone_status;

}


/*!
* @brief Redraw the usb icon if a change to the status bar has occurred
* @param[in] background_color_change_flag Parameter passed in to tell the function
*                                         whether a state transition has occurred,
*                                         meaning the status bar must also be updated.
*                                         A value of "1" means that it has changed.
* @return  NONE
*/
void
gui_update_status_bar_usb(uint8_t background_color_change_flag)
{
   static uint8_t previous_usb_status = 0;
   uint8_t current_usb_status = monitor_get_usb_status();

   //Only redraw the icon if usb status has changed. This helps with LCD flickering
   if((previous_usb_status != current_usb_status) || (1 == background_color_change_flag))
   {
      //Status will be > 0 if usb is plugged in. See monitor.c for info.
      if(0 < current_usb_status)
      {
         lcd_send_bitmap(usb_icon_bmp, SB_USB_X_OFFSET, SB_USB_Y_OFFSET, THEME_NEAR_WHITE, gui_get_status_bar_color()); //Display Icon
      }

      else
      {
         lcd_draw_rectangle(gui_get_status_bar_color(), SB_USB_X_OFFSET, 0, SB_USB_X_OFFSET + 24, SB_OFFSET); //Clear icon if there are no headphones
      }

   }

   previous_usb_status = current_usb_status;

}


/*!
* @brief Updates the battery time remaining display in the settings menu
* @param[in] NONE
* @return NONE
*
*/
void
gui_settings_menu_update_battery(void)
{
   //String to hold remaining time
   char time_remaining[9] = {0};

   //Convert remaining time from uint8_t to a string
   gui_get_battery_time_remaining(time_remaining);

   lcd_print_string_small(time_remaining, MENU_SETTINGS_BUTTON_X_OFFSET, 262, GRAY_MEDIUM, GRAY_MEDIUM_DARK);
}


/*!
* @brief This looks at the current battery level and determines how much time remains until it runs out
* @param[in] tmp_time_string String to hold the final time value
* @return NONE
*
*/
void
gui_get_battery_time_remaining(char *final_string)
{
   static char number_char_lookup[10] = {'0', '1', '2', '3', '4', '5','6', '7', '8', '9'};

   //A lookup table representing time remaining in minutes based on current battery percentage
   static uint8_t time_remaining[100] =
   {
      0, 0, 0, 0, 0, 1, 3, 5, 7, 9, 11, 14, 17, 20, 22, 25, 29, 32, 35, 39, 43, 48, 53, 57, 63, 68, 73, 80, 84, 89, 93,
      96, 100, 103, 106, 109, 113, 115, 118, 120, 122, 124, 127, 129, 131, 133, 136, 138, 141, 143, 145, 148, 150, 152,
      154, 156, 158, 160, 163, 164, 166, 167, 169, 170, 171, 173, 174, 176, 177, 178, 180, 181, 182, 183, 184, 185, 186,
      188, 188, 189, 190, 191, 192, 193, 194, 195, 195, 196, 196, 197, 198, 198, 199, 199, 200, 200, 200, 201, 201, 202
   };

   //Get the current battery level without sampling the ADC
   uint8_t tmp_battery_level = monitor_get_battery_level(BATTERY_SAMPLE_FLAG_NONE);

   char tmp_time_string[9] = " hr   min";

   //Convert battery level from uint8 to a string
   tmp_time_string[0] = number_char_lookup[((time_remaining[tmp_battery_level]) / 60)]; //Hours
   tmp_time_string[4] = number_char_lookup[(((time_remaining[tmp_battery_level]) % 60) / 10)]; //Minutes tens
   tmp_time_string[5] = number_char_lookup[(((time_remaining[tmp_battery_level]) % 60) % 10)]; //Minutes units
   tmp_time_string[9] = '\0'; //Add null character for strcpy()

   *(final_string + 8) = '\0'; //Give the input string a termination char to avoid buffer overflow
   strcpy(final_string, tmp_time_string);

}


/*!
* @brief Updates the sound and volume displays in the settings menu
* @param[in] NONE
* @return NONE
*
*/
void
gui_settings_menu_update_volume(void)
{
   uint16_t x_offset = MENU_SETTINGS_BUTTON_X_OFFSET, y_offset = MENU_SETTINGS_BUTTON_Y_OFFSET;
   uint16_t slider_offset = x_offset + 27, tmp_color = GREEN_NEON;
   e_dac_volume_type tmp_volume = dac_get_volume();

   //Toggle is initialized to "ON". If volume is muted, change it to "OFF".
   if(volume_muted == tmp_volume)
   {
      slider_offset -= 24;
      tmp_color = THEME_SUBTEXT;
   }

   //Toggle button
   lcd_send_bitmap(button_toggle_left_bmp, x_offset, y_offset, tmp_color, GRAY_MEDIUM_DARK);
   lcd_send_bitmap(button_toggle_right_bmp, x_offset + 35, y_offset, tmp_color, GRAY_MEDIUM_DARK);
   lcd_draw_rectangle(tmp_color, x_offset + 12, y_offset, x_offset + 35, y_offset + 22); //Fill in the button with color
   lcd_send_bitmap(button_toggle_middle_bmp, slider_offset, y_offset + 3, GRAY_LIGHT, tmp_color); //Display the button slider

   //Move to the volume bar section of the display and erase the previous contents
   x_offset += 27;
   y_offset += 25;
   lcd_draw_rectangle(GRAY_MEDIUM_DARK, x_offset, y_offset, x_offset + 100, y_offset + 45);
   y_offset += 25;
   uint16_t y_offset_bars = y_offset + 15;

   //Create the one volume bar for every level of volume e_enum_dac_volume list
   for(uint8_t volume_bar = 0; volume_bar < tmp_volume; volume_bar++)
   {
      uint8_t width = 25;

      lcd_draw_rectangle(GRAY_LIGHT, x_offset, y_offset, x_offset + width, y_offset_bars);

      x_offset += (width + (width / 2));
      y_offset -= 10;
   }

}


/*!
* @brief This displays the title bar at the top of any standard menu.
*        The title bar references the main menu that the user is currently in,
*        for example "Settings".
* @param[in] menu_title
* @return  NONE
*/
void
gui_create_title_bar(char *menu_title)
{
   //Title Bar
   uint16_t offset_x =  60;
   uint16_t offset_y =   SB_OFFSET + 30;
   lcd_draw_rectangle(THEME_MAIN, 0,SB_OFFSET, 320,  SB_OFFSET + TB_HEIGHT);
   lcd_print_string(menu_title, offset_x, offset_y, THEME_WHITE, THEME_MAIN);

   lcd_draw_rectangle(THEME_WHITE, 22, (30 + SB_OFFSET), 43, (33 + SB_OFFSET));
   lcd_draw_rectangle(THEME_WHITE, 22, (36 + SB_OFFSET), 43, (39 + SB_OFFSET));
   lcd_draw_rectangle(THEME_WHITE, 22, (42 + SB_OFFSET), 43, (45 + SB_OFFSET));
}


/*!
* @brief Return the current value of the RTC time registers as a string
* @param[in] tmp_time_buffer Buffer to hold time values:
*            [0] = hours_tens, [1] = hours_uints, [2] = minutes_tens, [3] = minutes_uints,
*            [4] = seconds_tens, [5] = seconds_units
* @return NONE
*
*/
void
gui_rtc_to_string(char *tmp_time_string)
{
   static char number_char_lookup[10] = {'0', '1', '2', '3', '4', '5','6', '7', '8', '9'};

   uint8_t tmp_time_buffer[6] = {0};

   rtc_get_time(tmp_time_buffer); //Fill time buffer with current RTC value

   tmp_time_string[0] = number_char_lookup[tmp_time_buffer[0]]; //Hours tens
   tmp_time_string[1] = number_char_lookup[tmp_time_buffer[1]]; //Hours units

   tmp_time_string[2] = ':';

   tmp_time_string[3] = number_char_lookup[tmp_time_buffer[2]]; //Minutes tens
   tmp_time_string[4] = number_char_lookup[tmp_time_buffer[3]]; //Minutes units
   tmp_time_string[5] = '\0'; //Add null character for strlen()

   if('0' == tmp_time_string[0])
   {
      tmp_time_string[0] = ' ';
   }
}


/*!
* @brief Convert a time value to a string
* @param[in] tmp_time_string Buffer to hold time values as characters:
*            [0] = hours_tens, [1] = hours_uints, [2] = minutes_tens, [3] = minutes_uints,
*            [4] = seconds_tens, [5] = seconds_units
* @param[in] tmp_time_buffer Input time as an integer array
* @return NONE
*
*/
void
gui_time_to_string(char *tmp_time_string, uint8_t *tmp_time_buffer)
{
   static char number_char_lookup[10] = {'0', '1', '2', '3', '4', '5','6', '7', '8', '9'};

   tmp_time_string[0] = number_char_lookup[tmp_time_buffer[0]]; //Hours tens
   tmp_time_string[1] = number_char_lookup[tmp_time_buffer[1]]; //Hours units

   tmp_time_string[2] = ':';

   tmp_time_string[3] = number_char_lookup[tmp_time_buffer[2]]; //Minutes tens
   tmp_time_string[4] = number_char_lookup[tmp_time_buffer[3]]; //Minutes units
   tmp_time_string[5] = '\0'; //Add null character for strlen()

   if('0' == tmp_time_string[0])
   {
      tmp_time_string[0] = ' ';
   }
}


#pragma GCC pop_options

/* end of file */

