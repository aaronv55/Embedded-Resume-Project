/** @file lcd.c
*
* @brief  This file contains both low-level LCD drivers and a light set of graphics functions
*         used to display bitmaps, text and other elements on the screen. It is specifically meant for
*         the QD-Tech QD3502 LCD, which includes a built-in ILI9486 LCD controller.
*
* @author Aaron Vorse
* @date   6/1/2020
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

#include "lcd.h"

#pragma GCC push_options
//#pragma GCC optimize ("O3")
//#pragma GCC optimize ("funroll-loops")


/*
****************************************************
************* File-Static Variables ****************
****************************************************
*/
static const uint8_t color_table_map[193] =
{
      0, 1, 2, 3, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3
};


/*
****************************************************
********** Private Function Prototypes *************
****************************************************
*/
void lcd_write_to_bus(uint8_t tmp_byte);
void lcd_send_command(uint8_t tmp_command);
void lcd_send_data(uint8_t tmp_data);
void lcd_set_window_address(uint16_t x_initial,uint16_t y_initial,uint16_t x_final,uint16_t y_final);
void lcd_reset(void);
void lcd_color_correction_set(void);
void lcd_power_mode_set(void);
void lcd_frame_rate_set(void);
void lcd_inversion_control_set(void);
void lcd_display_function_control_set(void);
void lcd_gamma_control_set(void);
void lcd_entry_mode_set(void);
void lcd_memory_access_control_set(void);
void lcd_pixel_format_set(void);
void lcd_gpio_init(void);
uint16_t lcd_decode_bmp_pixel(uint8_t red, uint8_t green, uint8_t blue);
uint8_t lcd_skip_bmp_header(void);
void lcd_get_font_color_table(uint16_t *tmp_table, uint16_t tmp_font_color, uint16_t tmp_background_color);
void lcd_parse_local_bitmap(const uint8_t *tmp_bitmap, uint16_t *tmp_dimensions);


/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/

/*!
* @brief Initializes LCD with system default parameters
* @param[in] NONE
* @return NONE
*/
void
lcd_init(void)
{
   //Initialize control and data lines
   lcd_gpio_init();
   
   lcd_reset();
   gpio_clear(LCD_CS); //Enable LCD
  
   //Setup base LCD register values
   //many of these are present in vendor startup code
   lcd_color_correction_set();
   lcd_power_mode_set();
   lcd_frame_rate_set();
   lcd_inversion_control_set();
   lcd_display_function_control_set();
   lcd_entry_mode_set();
   lcd_gamma_control_set();
   lcd_memory_access_control_set();
   lcd_pixel_format_set();
  
   // Turn off sleep mode 
   lcd_send_command(LCD_CMD_SLEEP_OUT);
   timers_delay(120);
   
   // Start displaying contents of LCD GRAM
   lcd_send_command(LCD_CMD_DISPLAY_ON);
   
   // Minimum 5ms delay after on command.
   // With system clock = 100MHz, timers_delay(400) is more than acceptable
   timers_delay(400);
}


/*!
* @brief Inverts the color of the entire LCD screen
* @param[in] NONE
* @return NONE
* @note This is done automatically by the LCD
*       frame buffer and not by the MCU
*/
void
lcd_invert_screen_on(void)
{
   gpio_clear(LCD_CS);
   lcd_send_command(LCD_CMD_DISPLAY_INVERSION_ON);
   gpio_set(LCD_CS);
}


/*!
* @brief Returns the entire LCD screen from inversion to normal mode
* @param[in] NONE
* @return NONE
* @note This is done automatically by the LCD
*       frame buffer and not by the MCU
*/
void
lcd_invert_screen_off(void)
{
   gpio_clear(LCD_CS);
   lcd_send_command(LCD_CMD_DISPLAY_INVERSION_OFF);
   gpio_set(LCD_CS);
}


/*!
* @brief Draws a filled rectangle on screen of a specific color
* @param[in] color A 16-bit 5-6-5 RGB color
* @param[in] x_in Initial X position
* @param[in] y_in Initial Y position
* @param[in] x_fin Final X position
* @param[in] y_fin Final Y position
* @return NONE
*/
void
lcd_draw_rectangle(uint16_t color,uint16_t x_in ,uint16_t y_in,uint16_t x_fin ,uint16_t y_fin)
{ 

   //Select LCD
   gpio_clear(LCD_CS);

   //Define the window the of the LCD to be written to
   lcd_set_window_address(x_in, y_in, x_fin-1, y_fin-1);

   //Put LCD in data mode
   gpio_set(LCD_RS);
   
   //Loop through each row and column, filling each pixel with color
   for(uint32_t row = y_in; row < y_fin; row++)
   {
      for(uint32_t column = x_in; column < x_fin; column++)
      {
         GPIOB->BSRR = 0xFFFF0000; //Clear the LCD data bus
         GPIOB->BSRR = (color >> 8); //write most significant byte
         GPIOA->BSRR = 0x00000100; //Set LCD_WR, clocking the bus data into the LCD
         GPIOA->BSRR = 0x01000000; //Clear LCD_WR

         GPIOB->BSRR = 0xFFFF0000; //Clear the LCD data bus
         GPIOB->BSRR = (color & 0x00FF); //write least significant byte
         GPIOA->BSRR = 0x00000100; //Set LCD_WR, clocking the bus data into the LCD
         GPIOA->BSRR = 0x01000000; //Clear LCD_WR
      }
   }
    
  gpio_set(LCD_CS);
    
}


/*!
* @brief Sends image from SD card to LCD 
* @param[in] x_in Initial X position
* @param[in] y_in Initial Y position
* @param[in] x_fin Final X position
* @param[in] y_fin Final Y position
* @param[in] memory_starting_address Memory block location of the image on the SD card
* @return NONE
*
* @note This function appears long and unruly due to the fact that many intermediate
*       functions were unrolled and optimization for speed was performed
*/
void
lcd_image_from_sd(uint16_t x_in ,uint16_t y_in,uint16_t x_fin ,uint16_t y_fin, uint32_t memory_starting_address)
{
   uint32_t total_blocks = 0, total_bytes = 0;
   
   //Formula: total-bytes = ((total-pixels) x (3 bytes-per-pixel))
   total_bytes = ((x_fin - x_in) * (y_fin - y_in) * 3);
   
   //512 bytes-per-block
   total_blocks = (total_bytes / 512);

   //Select LCD
   gpio_clear(LCD_CS);
   
   //Set upper left corner of image as starting address
   lcd_set_window_address(x_in, y_in, x_fin - 1, y_fin - 1);

   //Put LCD in data mode
   gpio_set(LCD_RS); //LCD_RS=1;
   
   //Start reading the image from the SD card
   sd_read_multiple_block(memory_starting_address);
   
   //Burn through the header to get to the image, starts at 0x36 offset 
   uint32_t current_byte = 512;
   current_byte -= lcd_skip_bmp_header();

   uint32_t color_buffer[6] = {0}; // blue green red blue green red
   uint32_t color_number = 0;
   

   for(uint32_t block_number = total_blocks; block_number != 0; --block_number)
   {

      while(0 != current_byte)
      {
         /* This is just spi_receive_byte(0xFF) unrolled */
         //Send dummy byte byte
         SPI2->DR = 0xFF;

         // Wait for the receive register to fill
         while(!(SPI2->SR & SPI_SR_RXNE)){}

         color_buffer[color_number] = (SPI2->DR);
         color_number++;
         
         if(6 == color_number)
         {
            //First Pixel
            GPIOB->BSRR = 0xFFFF0000; //Clear data bus
            GPIOB->BSRR = ((color_buffer[2] & 0xF8) | (color_buffer[1] >> 5)); //byte_high;
            GPIOA->BSRR = 0x00000100;//Set LCD_WR
            GPIOA->BSRR = 0x01000000; //Clear LCD_WR

            GPIOB->BSRR = 0xFFFF0000; //Clear data bus
            GPIOB->BSRR = (((color_buffer[1] & 0x1C) << 3) | (color_buffer[0] >> 3)); //byte_low;
            GPIOA->BSRR = 0x00000100;//Set LCD_WR
            GPIOA->BSRR = 0x01000000; //Clear LCD_WR

            //Second Pixel
            GPIOB->BSRR = 0xFFFF0000; //Clear data bus
            GPIOB->BSRR = ((color_buffer[5] & 0xF8) | (color_buffer[4] >> 5)); //byte_high;
            GPIOA->BSRR = 0x00000100;//Set LCD_WR
            GPIOA->BSRR = 0x01000000; ////Clear LCD_WR

            GPIOB->BSRR = 0xFFFF0000; //Clear data bus
            GPIOB->BSRR = (((color_buffer[4] & 0x1C) << 3) | (color_buffer[3] >> 3)); //byte_low;
            GPIOA->BSRR = 0x00000100;//Set LCD_WR
            GPIOA->BSRR = 0x01000000; ////Clear LCD_WR

            color_number = 0;
         }
         
         current_byte--;

      }

      current_byte = 512;

      //Burn through the CRC bits and wait until SD card sends data valid token
      spi_receive_byte(0xFF);
      spi_receive_byte(0xFF);

      while(SD_CMD17_TOKEN != spi_receive_byte(0xFF)) {} //CMD17 token is the same as CMD18, 0xFE
   }
   
   //The sd card must return an entire block. Flush the unused bytes from the last block
   sd_stop_transmission();
}


/*!
* @brief Displays a simple background of small squares to the LCD
* @param[in] NONE
* @return NONE
*/
void
lcd_background_squares(void)
{
   //blank blue screen
   lcd_draw_rectangle(0x0000, 0, 0, 320, 480);
   
   //Init square color
   uint16_t square_color = 0x07E0;
   
   //Put squares all over screen
   for(uint16_t row = 0; row < 470; row += 15)
   {
      square_color -= 1;
      
      for(uint16_t column = 0; column < 320; column += 15)
      {
         lcd_draw_rectangle(square_color, column, row, (column + 10), (row + 10));
      }
   }
}


/*!
* @brief Display a given string from a 2-bit bitmap font
* @param[in] tmp_string Message to be displayed
* @param[in] x
* @param[in] y
* @param[in] font_color
* @param[in] background_color
* @return NONE
*
* @note This is specifically for medium font
*/
void
lcd_print_string(char *tmp_string, uint16_t x, uint16_t y, uint16_t font_color, uint16_t background_color)
{
   uint8_t bit_mask_table[4] = {0xC0, 0x30, 0x0C, 0x03};
   uint16_t pixel_value = 0;
   uint16_t color_table[4] = {0};

   //Create a four-color color palette for anti-aliased font
   lcd_get_font_color_table(color_table, font_color, background_color);

   //Select LCD
   gpio_clear(LCD_CS);

   //Loop through string one char at a time
   for(uint8_t current_char = 0; current_char < strlen(tmp_string); current_char++)
   {
      //Put LCD in command mode
      gpio_clear(LCD_RS); //LCD_RS=0;

      //Set starting screen address
      lcd_set_window_address(x, y, x + 15, y + 20);

      //Put LCD in data mode
      gpio_set(LCD_RS); //LCD_RS=1;

      //Loop through bitmap. @note Font is two bits per pixel
      for(uint8_t current_byte = 0; current_byte < ((16*20*2) / 8); current_byte++)    //width = 16, height = 20, 2 bits/pixel, 8 bits/byte
      {
         for(uint8_t current_pixel = 0; current_pixel < 4; current_pixel++)
         {
            pixel_value = color_table[color_table_map[((jet_font[tmp_string[current_char] - 32][current_byte]) & bit_mask_table[current_pixel])]];

            //Send pixel to LCD
            GPIOB->BSRR = 0xFFFF0000; //Clear data bus
            GPIOB->BSRR = (uint8_t)(pixel_value >> 8);
            GPIOA->BSRR = 0x00000100;//Set LCD_WR
            GPIOA->BSRR = 0x01000000; //Clear LCD_WR
            GPIOB->BSRR = 0xFFFF0000; //Clear data bus
            GPIOB->BSRR = (uint8_t)(pixel_value & 0xFF);
            GPIOA->BSRR = 0x00000100;//Set LCD_WR
            GPIOA->BSRR = 0x01000000; //Clear LCD_WR
         }
      }

      //Move to next character position and leave space in between each character
      if(tmp_string[current_char] == '\'')
      {
         x += 7;
      }

      else
      {
        x += 13;
      }

    }

   //Deselect LCD
   gpio_set(LCD_CS);
}



/*!
* @brief Display a given string from a 2-bit bitmap
* @param[in] tmp_string Message to be displayed
* @param[in] x
* @param[in] y
* @param[in] font_color
* @param[in] background_color
* @return NONE
*
* @note This is specifically for small font
*/
void
lcd_print_string_small(char *tmp_string, uint16_t x, uint16_t y, uint16_t font_color, uint16_t background_color)
{
   uint8_t bit_mask_table[4] = {0xC0, 0x30, 0x0C, 0x03};
   uint16_t color_table[4] = {0};

   //Create a four-color color palette for anti-aliased font
   lcd_get_font_color_table(color_table, font_color, background_color);

   //Select LCD
   gpio_clear(LCD_CS);

   //Loop through string one char at a time
   for(uint8_t current_char = 0; current_char < strlen(tmp_string); current_char++)
   {
      //Put LCD in command mode
      gpio_clear(LCD_RS); //LCD_RS=0;

      //Set starting screen address
      lcd_set_window_address(x, y, x + 15, y + 20);

      //Put LCD in data mode
      gpio_set(LCD_RS); //LCD_RS=1;

      //Loop through bitmap. @note Font is two bits per pixel
      for(uint8_t current_byte = 0; current_byte < ((16*16*2)/8);current_byte++) //width = 16, height = 16, 2 bits/pixel, 8 bits/byte
      {
         for(uint8_t current_pixel = 0; current_pixel < 4; current_pixel++)
         {
            uint16_t pixel_value = color_table[color_table_map[((jet_font_small[tmp_string[current_char] - 32][current_byte]) & bit_mask_table[current_pixel])]];

            //Send pixel to LCD
            GPIOB->BSRR = 0xFFFF0000; //Clear data bus
            GPIOB->BSRR = (uint8_t)(pixel_value >> 8);
            GPIOA->BSRR = 0x00000100;//Set LCD_WR
            GPIOA->BSRR = 0x01000000; //Clear LCD_WR
            GPIOB->BSRR = 0xFFFF0000; //Clear data bus
            GPIOB->BSRR = (uint8_t)(pixel_value & 0xFF);
            GPIOA->BSRR = 0x00000100;//Set LCD_WR
            GPIOA->BSRR = 0x01000000; //Clear LCD_WR
         }
      }

      //Move to next character position and leave space in between each character
      if((tmp_string[current_char] == '\'') || (tmp_string[current_char] == '.'))
      {
         x += 5;
      }

      else if(tmp_string[current_char] == ':')
      {
         x += 8;
      }

      else
      {
        x += 11;
      }

    }

   //Deselect LCD
   gpio_set(LCD_CS);
}




/*!
* @brief Display a given string from a 2-bit bitmap
* @param[in] tmp_bmp A 2-bit bitmap stored on internal FLASH
* @param[in] x
* @param[in] y
* @param[in] main_color
* @param[in] background_color
* @return NONE
*
*/
void
lcd_send_bitmap(const uint8_t *tmp_bmp, uint16_t x, uint16_t y, uint16_t main_color, uint16_t background_color)
{
   uint16_t color_table[4] = {0};

   //Create a four-color color palette for anti-aliased font
   lcd_get_font_color_table(color_table, main_color, background_color);

   //Select LCD
   gpio_clear(LCD_CS);

   //Put LCD in command mode
   gpio_clear(LCD_RS);

   uint16_t bitmap_dimensions[2] = {0};
   lcd_parse_local_bitmap(tmp_bmp, bitmap_dimensions);

   //Only print a bitmap if a bitmap was initialized
   if(NULL != tmp_bmp)
   {
      //Set starting screen address
      lcd_set_window_address(x, y, (x + bitmap_dimensions[0] - 1), y + bitmap_dimensions[1]);

      //Put LCD in data mode
      gpio_set(LCD_RS);

      //FORMULA: ((Length x Width x 2 bits_per_pixel) / 8bits_per_byte) + offset
      uint16_t bitmap_size = ((((bitmap_dimensions[0]*bitmap_dimensions[1])*2) / 8) + BITMAPS_LOCAL_BMP_OFFSET);

      uint8_t bit_mask_table[4] = {0xC0, 0x30, 0x0C, 0x03};

      //Loop through bitmap. @note Bitmap is two bits per pixel
      for(uint16_t current_byte = BITMAPS_LOCAL_BMP_OFFSET; current_byte < bitmap_size; current_byte++)
      {
         for(uint8_t current_pixel = 0; current_pixel < 4; current_pixel++)
         {
            uint16_t pixel_value = color_table[color_table_map[(tmp_bmp[current_byte]) & bit_mask_table[current_pixel]]];

            //Send pixel to LCD
            GPIOB->BSRR = 0xFFFF0000; //Clear data bus
            GPIOB->BSRR = (uint8_t)(pixel_value >> 8);
            GPIOA->BSRR = 0x00000100;//Set LCD_WR
            GPIOA->BSRR = 0x01000000; //Clear LCD_WR
            GPIOB->BSRR = 0xFFFF0000; //Clear data bus
            GPIOB->BSRR = (uint8_t)(pixel_value & 0xFF);
            GPIOA->BSRR = 0x00000100;//Set LCD_WR
            GPIOA->BSRR = 0x01000000; //Clear LCD_WR
         }
      }
   }

   //Deselect LCD
   gpio_set(LCD_CS);
}




/*
****************************************************
********** Private Function Definitions ************
****************************************************
*/

/*!
* @brief Send a command to LCD
* @param[in] tmp_command
* @return NONE
* @note Refer to command table on the datasheet
*/
void
lcd_send_command(uint8_t tmp_command)
{
   //Set LCD to command mode, RS = 0
   gpio_clear(LCD_RS);
   
   //Set data bus to command byte
   lcd_write_to_bus(tmp_command);
}


/*!
* @brief Send a single byte to LCD
* @param[in] tmp_data
* @return NONE
* @note Refer to command table
*/
void
lcd_send_data(uint8_t tmp_data)
{
   //Set LCD to command mode, RS = 1
   gpio_set(LCD_RS);
   
   //Set data bus to command byte
   lcd_write_to_bus(tmp_data);
}


/*!
* @brief Sets the bus to the desired byte and clocks it into the LCD
* @param[in] tmp_byte
* @return NONE
*/
void
lcd_write_to_bus(uint8_t tmp_byte)
{
   GPIOB->BSRR = (0xFFFF << 16); //Clear lcd data bus
   
   GPIOB->BSRR = (tmp_byte); //write selected byte to DB0 - DB7
   
   //Give WR line a clock pulse. This can be replaced with GPIOx->BSRR for faster toggle rate
   GPIOA->BSRR = 0x0100;
   GPIOA->BSRR = 0x01000000;
}


/*!
* @brief Sets the window on the LCD to fill with image data
* @param[in] x_initial
* @param[in] y_initial
* @param[in] x_final
* @param[in] y_final
* @return NONE
*
* @note Initial row/column values cannot equal final values. 
*       To write a single row, y_initial = desired row, y_final = (y_initial + 1)
*/
void
lcd_set_window_address(uint16_t x_initial,uint16_t y_initial,uint16_t x_final,uint16_t y_final)
{
   //Set window columns
   lcd_send_command(LCD_CMD_COLUMN_ADDRESS_SET);
   lcd_send_data(x_initial >> 8); //upper byte
   lcd_send_data(x_initial & 0xFF); //lower byte
   lcd_send_data(x_final >> 8); 
   lcd_send_data(x_final & 0xFF); 
   
   //Set window rows
   lcd_send_command(LCD_CMD_PAGE_ADDRESS_SET);
   lcd_send_data(y_initial >> 8); //upper byte
   lcd_send_data(y_initial & 0xFF); //lower byte
   lcd_send_data(y_final >> 8); 
   lcd_send_data(y_final & 0xFF);
   
   //Start of image data transfer
   lcd_send_command(LCD_CMD_MEMORY_WRITE);
}


/*!
* @brief  Clears LCD GRAM buffer and restores default register values
* @param[in] NONE
* @return NONE
*/
void
lcd_reset(void)
{
   //Reset LCD. The delays are necessary 
   gpio_set(LCD_REST);
   timers_delay(50); 
   gpio_clear(LCD_REST);
   timers_delay(150);
   gpio_set(LCD_REST);
   timers_delay(150);
}


/*!
* @brief Fills color parameters to adjust picture quality
* @param[in] NONE
* @return NONE
* @note Command 0xF9 is not in the datasheet
*/
void
lcd_color_correction_set(void)
{
   //Color Correction
   lcd_send_command(0XF9); //Appears in example startup code but not datasheet
   lcd_send_data(0x00);
   lcd_send_data(0x08);
}


/*!
* @brief Set up various power registers for correct operation
* @param[in] NONE
* @return NONE
* @warning Changing this module can cause real-world damage to the LCD
*          Refer to the datasheet before altering. 
*/
void
lcd_power_mode_set(void)
{
   // Set VREG Positive & Negative Gamma 
   lcd_send_command(LCD_CMD_POWER_CONTROL_1);
   lcd_send_data(0x19);
   lcd_send_data(0x1a);

   // Set step up factor
   // WARINING: To prevent the device damage, keep VGH � DDVDH < 8V condition.
   //           See datasheet page 158
   lcd_send_command(LCD_CMD_POWER_CONTROL_2);
   lcd_send_data(0x45);//VGH,VGL    VGH>=14V.
   lcd_send_data(0x00);
   
   // Set step up frequency in normal mode. 
   // Higher frequency = better quality but more power consumption
   lcd_send_command(LCD_CMD_POWER_CONTROL_3);
   lcd_send_data(0x33);
   
   // Set VCOM voltage
   // Dependant on VREG2OUT
   lcd_send_command(LCD_CMD_VCOM_CONTROL);
   lcd_send_data(0x00); // Bit 0 is nVM, set automatically when nVM is programmed
   lcd_send_data(0x28);//VCM_REG[7:0]. <=0X80.
}


/*!
* @brief Set frame rate
* @param[in] NONE
* @return NONE
*/
void
lcd_frame_rate_set(void)
{
  // Set frame rate
  lcd_send_command(LCD_CMD_FRAME_RATE_CONTROL);//OSC Freq set.
  lcd_send_data(0x90); // 0x90 = 56Hz, 0xA0=62Hz, 0XB0 =70HZ, 0xF0 = 117Hz max
  lcd_send_data(0x11); // Normal clock division. 0x11 = Fosc/8
}


/*!
* @brief Set LCD inversion mode
* @param[in] NONE
* @return NONE
* @note Inversion is initialized off below
*/
void
lcd_inversion_control_set(void)
{
   // Disable inversion control
   lcd_send_command(LCD_CMD_DISPLAY_INVERSION_CONTROL);
   lcd_send_data(0x02); //2-dot inversion mode, bit4 = ZINV: 0 = inversion disabled, 1 = enabled 
}


/*!
* @brief Sets LCD operating parameters such as scan direction
* @param[in] NONE
* @return NONE
*/
void
lcd_display_function_control_set(void)
{
  // Display function control
   lcd_send_command(LCD_CMD_DISPLAY_FUNCTION_CONTROL); 
   lcd_send_data(0x00);
   lcd_send_data(0x42);//0 GS SS SM ISC[3:0], GS = 1, SS = SM = 0, ISC = 2
   lcd_send_data(0x3B);
}


/*!
* @brief Set gamma correction
* @param[in] NONE
* @return NONE
* @note This entire module is taken directly from the vendor 
*       example startup code. None of the codes 0xF1 - 0xF8 
*       are in the datasheet, but they should be included here in startup
*/
void
lcd_gamma_control_set(void)
{

   // Set positive gamma control
   lcd_send_command(LCD_CMD_PGAMCTRL);
   lcd_send_data(0x1F);
   lcd_send_data(0x25);
   lcd_send_data(0x22);
   lcd_send_data(0x0B);
   lcd_send_data(0x06);
   lcd_send_data(0x0A);
   lcd_send_data(0x4E);
   lcd_send_data(0xC6);
   lcd_send_data(0x39);
   lcd_send_data(0x00);
   lcd_send_data(0x00);
   lcd_send_data(0x00);
   lcd_send_data(0x00);
   lcd_send_data(0x00);
   lcd_send_data(0x00);
   
   // Set negative gamma control
   // Values from vender startup code. Leave as is
   lcd_send_command(LCD_CMD_NGAMCTRL);
   lcd_send_data(0x1F);
   lcd_send_data(0x3F);
   lcd_send_data(0x3F);
   lcd_send_data(0x0F);
   lcd_send_data(0x1F);
   lcd_send_data(0x0F);
   lcd_send_data(0x46);
   lcd_send_data(0x49);
   lcd_send_data(0x31);
   lcd_send_data(0x05);
   lcd_send_data(0x09);
   lcd_send_data(0x03);
   lcd_send_data(0x1C);
   lcd_send_data(0x1A);
   lcd_send_data(0x00);
   
   lcd_send_command(0XF1);
   lcd_send_data(0x36);
   lcd_send_data(0x04);
   lcd_send_data(0x00);
   lcd_send_data(0x3C);
   lcd_send_data(0x0F);
   lcd_send_data(0x0F);
   lcd_send_data(0xA4);
   lcd_send_data(0x02);
   
   lcd_send_command(0XF2);
   lcd_send_data(0x18);
   lcd_send_data(0xA3);
   lcd_send_data(0x12);
   lcd_send_data(0x02);
   lcd_send_data(0x32);
   lcd_send_data(0x12);
   lcd_send_data(0xFF);
   lcd_send_data(0x32);
   lcd_send_data(0x00);
   
   lcd_send_command(0XF4);
   lcd_send_data(0x40);
   lcd_send_data(0x00);
   lcd_send_data(0x08);
   lcd_send_data(0x91);
   lcd_send_data(0x04);
   
   lcd_send_command(0XF8);
   lcd_send_data(0x21);
   lcd_send_data(0x04);
}


/*!
* @brief Set parameters such as low voltage detection, deep standby etc.
* @param[in] NONE
* @return NONE
*/
void
lcd_entry_mode_set(void)
{
  // Set entry mode
  lcd_send_command(LCD_CMD_ENTRY_MODE_SET);
  lcd_send_data(0x07); // EPF[1:0] 0 0 DSTB GON DTE GAS

}


/*!
* @brief Defines read/write scanning direction of frame memory
* @param[in] NONE
* @return NONE
*/
void
lcd_memory_access_control_set(void)
{
   // Set memory access control
  lcd_send_command(LCD_CMD_MEMORY_ACCESS_CONTROL);
  lcd_send_data(0xC8);
}


/*!
* @brief Sets LCD to 16 bits/pixel
* @param[in] NONE
* @return NONE
*/
void
lcd_pixel_format_set(void)
{
   // Set interface pixel format
  lcd_send_command(LCD_CMD_INTERFACE_PIXEL_FORMAT);
  lcd_send_data(0x55); // 16 bits/pixel
}


/*!
* @brief Initialize GPIO for LCD control and data lines
* @param[in] NONE
* @return NONE
*/
void
lcd_gpio_init(void)
{
  uint8_t pin_number = 0;
   
  //Initialize GPIO as LCD data lines DB0 - DB8
  for(pin_number = 0; pin_number < 16; pin_number++)
  {
     gpio_gen_output_init(GPIOB, pin_number);
     timers_delay_mini(100);
     gpio_clear(GPIOB, pin_number);
  }
   
  //Initialize GPIO as LCD control Lines
  gpio_gen_output_init(LCD_REST);
  gpio_gen_output_init(LCD_CS);
  gpio_gen_output_init(LCD_RS);
  gpio_gen_output_init(LCD_WR);
  gpio_gen_output_init(LCD_RD);
  
  //Backlight is connected to the gate of a PMOS with an external pullup. GPIO must be open drain.
  gpio_gen_output_init(LCD_BACKLIGHT);
  gpio_type_init(LCD_BACKLIGHT,GPIO_OTYPER_OPEN_DRAIN);
  lcd_backlight_disable(); //Turn off screen so it doesn't display garbage while initializing

  //Set all control lines to high initially
  gpio_set(LCD_REST);
  gpio_set(LCD_CS);
  gpio_set(LCD_RS);
  gpio_set(LCD_WR);
  gpio_set(LCD_RD);
}



/*!
* @brief Parse the .BMP file for offset and skip to image
* @param[in] NONE
* @return  current_byte Count so the main program can keep track of how many bytes have been
*                       sent since the file began
*
* @note This assumes the sd card has been given the read_multiple_blocks command
*       and has subsequently returned a data valid token (0xFE).
*       It should be called right before the first byte of the BMP file is received
*       
*/
uint8_t
lcd_skip_bmp_header(void)
{
   uint8_t offset_value = 0 ; //The first byte after the header ends and the real image data begins
   uint8_t current_byte = 0;
   
   //Ignore the first 10 bytes. Offset byte is at location 0x0A
   for(current_byte = 0; current_byte < LCD_BMP_OFFSET_LOCATION; current_byte++)
   {
      offset_value = spi_receive_byte(0xFF); //spi_receive_byte(0xFF);

   }

   //Jump to the first image byte. Watch out, we are now at byte 0x0A after reaching the offset byte
   while(current_byte < offset_value)
   {
      spi_receive_byte(0xFF);
      current_byte++;
   }
   
   return(current_byte);
}


/*!
* @brief Parse the LOCAL bitmap for width and height
* @param[in] tmp_bitmap Bitmap to be read
* @param[in] tmp_size Two member array for holding width and height values
* @return  NONE
*
* @warning This is meant only for bitmaps stored on local FLASH, it
*          is not intended for actual .BMPs on the SD card
*/
void
lcd_parse_local_bitmap(const uint8_t *tmp_bitmap, uint16_t *tmp_dimensions)
{
   tmp_dimensions[0] = tmp_bitmap[0];  //Width high byte
   tmp_dimensions[0] |= tmp_bitmap[1]; //      low byte

   tmp_dimensions[1] = tmp_bitmap[2];  //Height high byte
   tmp_dimensions[1] |= tmp_bitmap[3]; //       low byte
}


/*!
* @brief Determine correct color table for anti-aliased font
* @param[in] tmp_table An array external to the function where the color table
*                      will be stored once calculated
* @param[in] tmp_font_color Main color of the font, not the anti-aliased parts
* @param[in] tmp_background_color
* @return  NONE
*
*/
void
lcd_get_font_color_table(uint16_t *tmp_table, uint16_t tmp_font_color, uint16_t tmp_background_color)
{
   //Extract each pure color from the font color
   uint8_t red_font = ((tmp_font_color & 0xF800) >> 11);
   uint8_t blue_font = (tmp_font_color & 0x001F);
   uint8_t green_font = ((tmp_font_color & 0x07C0) >> 5);

   //Extract each pure color from the background color
   uint8_t red_background = ((tmp_background_color & 0xF800) >> 11);
   uint8_t blue_background = (tmp_background_color & 0x001F);
   uint8_t green_background = ((tmp_background_color & 0x07C0) >> 5);

   //Calculate the average of each pure color from both font and background
   uint8_t red_total = (red_font + red_background) / 6;
   uint8_t green_total = (green_font + green_background) / 6;
   uint8_t blue_total = (blue_font + blue_background) / 6;

   //Find the total average color value
   int32_t font_average = (red_font + blue_font + green_font) / 3;
   int32_t background_average = (red_background + blue_background + green_background) / 3;


   //If text is brighter than background or their averages are not very far apart
   if((font_average > background_average) || ((5 > (font_average - background_average)) && (-5 < (font_average - background_average))))
   {
      tmp_table[0] = tmp_font_color;
      tmp_table[1] = tmp_background_color;
      tmp_table[2] = (((red_total*4) << 11) | ((green_total*4) << 5) | (blue_total*4)); //Medium light
      tmp_table[3] = (((red_total*3) << 11) | ((green_total*3) << 5) | (blue_total*3)); //Medium Dark
   }

   //If not, reverse the intermediate table values
   else
   {
      tmp_table[0] = tmp_font_color;
      tmp_table[1] = tmp_background_color;
      tmp_table[2] = (((red_total*2) << 11) | ((green_total*2) << 5) | (blue_total*2)); //Medium Dark
      tmp_table[3] = (((red_total*3) << 11) | ((green_total*3) << 5) | (blue_total*3)); //Medium light
   }
}

#pragma GCC pop_options

/* end of file */

