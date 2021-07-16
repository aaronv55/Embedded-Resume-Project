/** @file lcd.h
*
* @brief  This file contains both low-level LCD drivers and a light set of graphics functions
*         used to display bitmaps, text and other elements on the screen. It is specifically meant for
*         the QD-Tech QD3502 LCD, which includes a built-in ILI9486 LCD controller.
*
* @author Aaron Vorse
* @author Aaron Vorse
* @date   6/12/2020
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

#ifndef LCD_H
#define LCD_H

/****** LCD Control Pins ************/
#define LCD_REST GPIOC,10
#define LCD_CS  GPIOA,12
#define LCD_RS  GPIOA,15
#define LCD_WR  GPIOA,8
#define LCD_RD  GPIOA,11
#define LCD_BACKLIGHT GPIOC,12 //This is initialized as open drain


/****** LCD Commands List ***********/
#define LCD_CMD_COLUMN_ADDRESS_SET 0x2A
#define LCD_CMD_PAGE_ADDRESS_SET 0x2B //Page means row here
#define LCD_CMD_MEMORY_WRITE 0x2C
#define LCD_CMD_POWER_CONTROL_1 0xC0
#define LCD_CMD_POWER_CONTROL_2 0xC1
#define LCD_CMD_POWER_CONTROL_3 0xC2
#define LCD_CMD_VCOM_CONTROL 0xC5
#define LCD_CMD_FRAME_RATE_CONTROL 0xB1
#define LCD_CMD_DISPLAY_INVERSION_CONTROL 0xB4
#define LCD_CMD_DISPLAY_FUNCTION_CONTROL 0xB6
#define LCD_CMD_ENTRY_MODE_SET 0xB7
#define LCD_CMD_PGAMCTRL 0xE0 //Positive gamma control
#define LCD_CMD_NGAMCTRL 0xE1 //Negative gamma control
#define LCD_CMD_MEMORY_ACCESS_CONTROL 0x36
#define LCD_CMD_INTERFACE_PIXEL_FORMAT 0x3A
#define LCD_CMD_SLEEP_OUT 0x11
#define LCD_CMD_DISPLAY_ON 0x29 
#define LCD_CMD_DISPLAY_INVERSION_ON 0x21
#define LCD_CMD_DISPLAY_INVERSION_OFF 0x20
#define LCD_CMD_BRIGHTNESS_VALUE 0x51

#define LCD_BMP_OFFSET_LOCATION 0x0B
#define LCD_MAX_SINGLE_LINE_CHARACTERS 26

#define lcd_backlight_enable() gpio_clear(LCD_BACKLIGHT)
#define lcd_backlight_disable() gpio_set(LCD_BACKLIGHT)

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f410rx.h"
#include "base_gpio_drivers.h"
#include "timers.h"
#include "microsd.h"
#include "personal_function_toolbox.h"
#include "bitmaps.h"
#include "font.h"

/*
****************************************************
******* Public Functions Defined in lcd.c **********
****************************************************
*/
void lcd_init(void);
void lcd_invert_screen_on(void);
void lcd_invert_screen_off(void);
void lcd_background_squares(void);
void lcd_image_from_sd(uint16_t x_in ,uint16_t y_in,uint16_t x_fin ,uint16_t y_fin, uint32_t memory_starting_address);

void lcd_draw_rectangle(uint16_t color,uint16_t x_in ,uint16_t y_in,uint16_t x_fin ,uint16_t y_fin);
void lcd_print_string(char *tmp_string, uint16_t x, uint16_t y, uint16_t font_color, uint16_t background_color);
void lcd_print_string_small(char *tmp_string, uint16_t x, uint16_t y, uint16_t font_color, uint16_t background_color);
void lcd_send_bitmap(const uint8_t *tmp_bmp, uint16_t x, uint16_t y, uint16_t main_color, uint16_t background_color);


#endif /* LCD_H */

/*** end of file ***/

