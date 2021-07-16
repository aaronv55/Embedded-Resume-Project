/** @file buttons.c
*
* @brief  This file contains functions to create, interact with and handle virtual buttons
*         and button events on the LCD. It does not cover external buttons i.e switches on the PCB.
* @author Aaron Vorse
* @date   8/19/2020
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

#include "buttons.h"


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


/*
****************************************************
********** Private Function Prototypes *************
****************************************************
*/
void buttons_make_rectangle_button(t_button buttons, char *button_text);
void buttons_center_text(t_button button, char *button_text, uint16_t *tmp_position);
void buttons_make_rounded_button(t_button button, char *button_text);
void buttons_make_ghost_button(t_button button, char *button_text);


/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/


/*!
* @brief Loop through a set of given virtual buttons and display them on the
*        LCD using their individual attributes
* @param[in] buttons Array of button structures to be created
* @param[in] button_text An array of strings to be displayed inside each button
* @param[in] list_length Housekeeping variable to keep track of the array length
*                        as it is passed by pointer
* @return  NONE
*/
void
buttons_create(t_button buttons[], char **button_text, size_t list_length)
{
   //Loop through all buttons in the list
   for(uint8_t current_button = 0; current_button < list_length; current_button++)
   {
      e_button_type type = buttons[current_button].type;

      switch(type)
      {
      case button_rectangle:
         buttons_make_rectangle_button(buttons[current_button], button_text[current_button]);
         break;
      case button_rounded_corners:
         buttons_make_rounded_button(buttons[current_button], button_text[current_button]);
         break;
      case button_ghost:
         buttons_make_ghost_button(buttons[current_button], button_text[current_button]);
         break;
      default:
         break;

      }
   }
}


/*!
* @brief Scroll through a list of virtual buttons and determine if the user's
*        touch falls within any of their boundaries
* @param[in] buttons A list of buttons whose logic should be checked
* @param[in] list_length Housekeeping variable to keep track of the array length
*                        as it is passed by pointer
* @return pressed_button_number The number of the button in the list which is active
*
* @note Only one virtual button can be active at a time. The function will break on
*       identifying the earliest active button in the list
*/
uint8_t
buttons_status(t_button buttons[], size_t list_length)
{

   uint8_t pressed_button_number = NO_BUTTON_PRESS;

   //Loop through all buttons in the list
   for(uint8_t current_button = 0; current_button < list_length; current_button++)
   {
      uint16_t tmp_position[2] = {0};
      touch_get_position(tmp_position); //Get current touch position on lcd from user

      uint16_t tmp_width = buttons[current_button].x_position + buttons[current_button].width;
      uint16_t tmp_height = buttons[current_button].y_position + buttons[current_button].height;

      //Check if screen touch is within the button boundaries
      if((buttons[current_button].x_position <= tmp_position[0]) && (tmp_width >= tmp_position[0]))
      {
         if((buttons[current_button].y_position <= tmp_position[1]) && (tmp_height >= tmp_position[1]))
         {
            pressed_button_number = current_button;
            break;
         }

      }
   }

   return(pressed_button_number);
}


/*
****************************************************
********** Private Function Definitions ************
****************************************************
*/

/*!
* @brief Center the button's text within its boundaries
* @param[in] button
* @param[in] button_text
* @param[in] tmp_position A pointer to an array which holds the x and y offset
*                         necessary to center the text inside the button
* @return  NONE
*
* @warning Not setting a char will result in a hardfault divide by zero error
*/
void
buttons_center_text(t_button button, char *button_text, uint16_t *tmp_position)
{
   uint16_t x_center = (button.width / 2) + button.x_position;
   uint16_t y_center = (button.height / 2) + button.y_position;

   /* Check to make sure a character has been initialized and is within the range of an ASCII char (' ' to '~').
    * If it hasn't it, it will be undetermined and strnlen() may send the processor into a hardfault error.
    * This is a simple bounds check.
    */
   uint8_t uninitialized_flag = 0;

   for(uint8_t current_char = 0; current_char < LCD_MAX_SINGLE_LINE_CHARACTERS; current_char++)
   {
      if(' ' > button_text[current_char]) //lowest acceptable char
      {
         if('~' < button_text[current_char]) //highest acceptable char
         {
            uninitialized_flag = 1;
         }
      }
   }

   if(0 == uninitialized_flag)
   {
      //Shift position the correct amount from the center of the button
      tmp_position[0] = x_center - ((strlen(button_text) / 2)*13); //There are 13 pixels from one letter to another
      tmp_position[1] = y_center - 10; //10 is half of letter height, which is 20
   }
}


/*!
* @brief Draw a simple, rectangular button with centered text and square edges
* @param[in] button
* @param[in] button_text
* @return  NONE
*/
void
buttons_make_rectangle_button(t_button button, char *button_text)
{
   uint16_t color = button.body_color;

   //Create button body
   lcd_draw_rectangle(color,button.x_position, button.y_position, button.x_position + button.width, button.y_position + button.height);

   uint16_t text_position[2] = {0};
   buttons_center_text(button, button_text, text_position);

   //Fill button with text
   uint16_t font_color = button.text_color;
   lcd_print_string(button_text, text_position[0], text_position[1], font_color, color);

   //Fill button with bitmap
   lcd_send_bitmap(button.bmp, button.x_position, button.y_position, font_color, color);

}


/*!
* @brief Draw a simple, rectangular button with centered text and round edges
* @param[in] button
* @param[in] button_text
* @return  NONE
*/
void
buttons_make_rounded_button(t_button button, char *button_text)
{
   uint16_t color = button.body_color;

   //Main body chunk
   lcd_draw_rectangle(button.body_color,button.x_position, button.y_position, button.x_position + button.width, button.y_position + button.height);

   //Round the button's corners
   lcd_send_bitmap(button_corner_m_ul, button.x_position, button.y_position, button.body_color, button.background_color); //upper left

   uint16_t y_offset = (button.y_position + button.height) - 9; //Corners have a 9-bit round over
   lcd_send_bitmap(button_corner_m_ll, button.x_position, y_offset, button.body_color, button.background_color); //lower left

   uint16_t x_offset = (button.x_position + button.width) - 9; //Corners have a 9-bit round over
   lcd_send_bitmap(button_corner_m_ur, x_offset, button.y_position, button.body_color, button.background_color); //upper right

   lcd_send_bitmap(button_corner_m_lr, x_offset, y_offset, button.body_color, button.background_color); // lower right

   uint16_t text_position[2] = {0};
   buttons_center_text(button, button_text, text_position);

   //Fill button with text
   uint16_t font_color = button.text_color;
   lcd_print_string(button_text, text_position[0], text_position[1], font_color, color);

   //Fill button with bitmap
   lcd_send_bitmap(button.bmp, button.x_position, button.y_position, font_color, color);

}


/*!
* @brief Draw a "hollow" (aka ghost) button with only the frame and text
* @param[in] button
* @param[in] button_text
* @return  NONE
*/
void
buttons_make_ghost_button(t_button button, char *button_text)
{

   uint16_t x_position = button.x_position;
   uint16_t y_position = button.y_position;
   uint16_t width = button.width;
   uint16_t height = button.height;

   uint16_t tmp_body_color =  button.text_color; //button.body_color;
   uint16_t tmp_background_color = button.background_color;

   lcd_draw_rectangle(tmp_body_color, x_position, y_position, x_position + width, y_position + 2); //upper middle
   lcd_draw_rectangle(tmp_body_color, x_position, y_position + height, x_position + width, y_position + height + 2); //lower middle

   lcd_draw_rectangle(tmp_body_color, x_position, y_position, x_position + 2, y_position + height); // left
   lcd_draw_rectangle(tmp_body_color, x_position + width, y_position, x_position + width + 2, y_position + height); //right

   //Round the button's corners
   lcd_send_bitmap(button_ghost_ul_bmp, x_position, y_position, tmp_body_color, tmp_background_color); //upper left

   uint16_t y_offset = (y_position + height) - 7; //Corners have a 8-bit round over
   uint16_t x_offset = (x_position + width) - 6; //Corners have a 8-bit round over

   lcd_send_bitmap(button_ghost_ll_bmp, x_position, y_offset, tmp_body_color, tmp_background_color); //lower left
   lcd_send_bitmap(button_ghost_ur_bmp, x_offset, y_position, tmp_body_color, tmp_background_color); //upper right
   lcd_send_bitmap(button_ghost_lr_bmp, x_offset, y_offset, tmp_body_color, tmp_background_color); // lower right

   //Fill button with text
   uint16_t text_position[2] = {0};
   buttons_center_text(button, button_text, text_position);
   lcd_print_string(button_text, text_position[0], text_position[1], button.text_color, button.background_color);
}


/* end of file */

