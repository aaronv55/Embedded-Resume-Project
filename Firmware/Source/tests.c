/** @file tests.c
*
* @brief  This file contains production tests for the Embedded Resume Device
* @author Aaron Vorse
* @date   2/05/2020
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



#include "tests.h"

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

void tests_battery_write_sdcard(uint32_t tmp_block_number, uint8_t *p_data_buffer);
void tests_production_main_board(void);
void tests_production_daughter_board(void);
void tests_production_usb(void);
void tests_production_battery(void);
void tests_production_touch(void);
void tests_production_touch_single_result(uint8_t target_number);
void tests_production_power_button(void);
void tests_production_lcd_backlight(void);
void tests_production_audio(void);

/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/

/*!
* @brief Initializes the test sense pin, which tells the system if production testing is active
* @param[in] NONE
* @return NONE
* @note NONE
*/
void
tests_init(void)
{
   gpio_gen_input_init(TESTS_PRODUCTION_TEST_SENSE_PIN);
   gpio_pupd_init(TESTS_PRODUCTION_TEST_SENSE_PIN, GPIO_PUPDR_PULL_DOWN);
}


/*!
* @brief Performs a battery discharge test which queries the battery level every ten seconds during
*        normal device operation until it's so low that the system shuts down.
* @param[in] NONE
* @return NONE
* @note Results are stored in SD Card.
*/
void
tests_battery_drain_test(void)
{
   static uint16_t sample_number = 0;
   static uint8_t data_buffer[512] = {0};
   static uint32_t block_number = TESTS_BATTERY_LOG_ADDRESS;

   //Only read the battery every 10 seconds when the RTC alarm triggers
   if(RTC->ISR & RTC_ISR_ALRAF)
   {
      //Read the current battery level from the ADC and store the 16-bit data across two bytes
      uint16_t tmp_battery_level = monitor_get_raw_battery_level(BATTERY_SAMPLE_FLAG_SAMPLE);

      data_buffer[sample_number] = (uint8_t)(tmp_battery_level >> 8); //MSB
      data_buffer[(sample_number + 1)] = (uint8_t)(tmp_battery_level & 0xFF);//LSB


      //Display results on serial terminal
      char tmp_string[20] = {'0'};
      pft_uint32_to_string(data_buffer[sample_number], tmp_string);
      uart1_printf(tmp_string);
      uart1_printf(",");
      pft_uint32_to_string(data_buffer[(sample_number + 1)], tmp_string);
      uart1_printf(tmp_string);



      //Increment two samples, since it takes two bytes to store one 16-bit sample
      sample_number += 2;

      //If the sample buffer is full, write to the SD card
      if (512 == sample_number)
      {
         tests_battery_write_sdcard(block_number, data_buffer);
         //uart1_printf("Sample is 512");
         sd_print_block(block_number);
         block_number++;
         sample_number = 0;
      }


      //If battery is low write to the SD card, so data is not lost if the system shuts off
      if(1450 > tmp_battery_level)
      {
         tests_battery_write_sdcard(block_number, data_buffer);
         uart1_printf("Battery is low");
      }
   }

}


/*!
* @brief Parses data from the battery drain test which has already been stored in the SD card
* @param[in] NONE
* @return NONE
* @note This specifically spits out data over UART in a comma-separated list and is meant to be read by a PC terminal
*
*/
void
tests_battery_read_results(void)
{
   for(uint8_t current_block = 0; current_block < 7; current_block++)
   {
      sd_print_block(TESTS_BATTERY_LOG_ADDRESS + current_block);
   }
}


/*!
* @brief This is used to verify the total hardware system before installing it in the device's case
* @param[in] NONE
* @return NONE
*
*/
void
tests_production_hardware(void)
{
   //Only carry out production tests if the production test pin (DB10) is held high
   if(gpio_read_pin(TESTS_PRODUCTION_TEST_SENSE_PIN))
   {
      //Display the main testing menu
      gui_tests_create_main_menu();
      lcd_backlight_enable();

      //Test USB
      tests_production_usb();

      //Make it easy to identify the start of production testing on the PC serial terminal
      uart1_printf("\n\r\n\r\n\r------------PRODUCTION TESTING MODE ENTERED----------------- \n\r" );
      uart1_printf("------------------------------------------------------------ \n\r" );
      uart1_printf("------------------------------------------------------------ \n\r" );

      //Test each hardware peripheral
      tests_production_battery();
      tests_production_touch();
      tests_production_power_button();
      tests_production_lcd_backlight();
      tests_production_audio();

      uart1_printf(" \n\r\n\r------PRODUCTION TESTING COMPLETED SUCCESSFULLY------\n\r" );
   }
}


/*
****************************************************
********** Private Function Definitions ************
****************************************************
*/

/*!
* @brief Writes the results of the battery drain test to the SD Card.
* @param[in] tmp_sample_number Current number of samples received. This is used to determine
*                              which 512-byte block needs to be written to in the SD card.
* @return NONE
*/
void
tests_battery_write_sdcard(uint32_t tmp_block_number, uint8_t *p_data_buffer)
{
   sd_write_block(p_data_buffer, tmp_block_number);
}


/*!
* @brief This is used to verify the functionality of the USB hardware
* @param[in] NONE
* @return NONE
*
*/
void
tests_production_usb(void)
{
   //Display the usb test menu
   gui_tests_create_usb_menu();

   //Wait until USB is plugged in
   while( !(monitor_get_usb_status())) {};

   //Let user know USB was detected
   gui_tests_display_pass_screen();

   //Give the user time to connect to the PC serial terminal
   timers_delay(10000);
}


/*!
* @brief This is used to verify that the system is correctly tracking battery voltage
* @param[in] NONE
* @return NONE
*
*/
void
tests_production_battery(void)
{

   uart1_printf("\n\rBattery Testing Results \n\r\n\r\0" );

   int16_t test_voltages[5] = {10, 1460, 1640, 1810, 2155};
   int16_t tolerance = 30;

   //Loop through each test voltage, checking for accuracy
   for(uint8_t current_test = 0; current_test < 5; current_test++ )
   {
      //Display appropriate menu
      gui_tests_create_battery_menu(current_test);


      int16_t current_battery_level = 7000; //7000 is an arbitrary init value outside of the possible battery voltage readings
      char tmp_string[10] = {0};

      //Continually sample battery voltage until it is within the tolerance of the current voltage test level
      while (!(((test_voltages[current_test] - tolerance) < current_battery_level) && ((test_voltages[current_test] + tolerance) > current_battery_level)))
      {
         current_battery_level = monitor_get_raw_battery_level(BATTERY_SAMPLE_FLAG_SAMPLE);
         timers_delay(300); //Give time for transient voltages to die off after turning the physical switch

         //Display the current raw ADC output on the LCD screen
         pft_uint32_to_string(current_battery_level, tmp_string);
         lcd_print_string_small("Current ADC Value", 30, 270, GRAY_LIGHT, BLACK);
         lcd_print_string(tmp_string, 30, 300, GREEN_DARK, BLACK);
      }

      //Send the passing ADC output to the PC terminal
      uart1_printf("   " );
      uart1_printf(tmp_string);
      uart1_printf(" \n\r" );

      //Let user know the correct voltage was detected
      gui_tests_display_pass_screen();

   }

}


/*!
* @brief This is used to verify the functionality of the touchscreen for both the main and daughter boards
* @param[in] NONE
* @return NONE
*
*/
void
tests_production_touch(void)
{
   uart1_printf("\n\r\n\rTouch Testing Results \n\r\0" );

   size_t total_targets = (sizeof(touch_targets_template) / sizeof(*touch_targets_template));

   //Loop through all touch targets
   for(uint8_t target_number = 0; target_number < total_targets; target_number++)
   {
      //Display the current target
      gui_tests_create_touch_menu(target_number);

      uint8_t active_button = NO_BUTTON_PRESS;

      //Continually scan the touch position until the user correctly clicks on the current marker
      while(active_button != target_number)
      {
         touch_update_position();
         active_button = buttons_status(touch_targets_template, total_targets);
      }

      //Send touch results to the PC serial terminal
      tests_production_touch_single_result(target_number);

   }

   //Let user know the correct voltage was detected
   gui_tests_display_pass_screen();
}


/*!
* @brief A helper function to display the current
* @param[in] target_number The current marker being tested in tests_production_touch()
* @return NONE
* @note This will display results in the format   Target (number)   X: (touch x position) Y: (touch y position)
*/
void
tests_production_touch_single_result(uint8_t target_number)
{
   uart1_printf("\n\r  Target");

   //Convert the target number to a string that "uart1_printf()" will accept
   char target_as_string[3] = {" 0\0"};
   target_as_string[1] += target_number;

   //Send the target number
   uart1_printf(target_as_string);
   uart1_printf("   X:  \0");

   //Convert the touch position to a string and send it over uart
   uint16_t tmp_touch_position[2] = {0};
   touch_get_position(tmp_touch_position);
   char tmp_string[11] = {0};

   pft_uint32_to_string((uint32_t)tmp_touch_position[0], tmp_string); //X position
   uart1_printf(tmp_string);

   uart1_printf("   Y:  \0");
   pft_uint32_to_string((uint32_t)tmp_touch_position[1], tmp_string); //Y position
   uart1_printf(tmp_string);

}


/*!
* @brief This is used to verify the functionality of the main power button on the main board
* @param[in] NONE
* @return NONE
*
*/
void
tests_production_power_button(void)
{
   uart1_printf("\n\r\n\r\n\rMain Power Button Testing \n\r\n\r\0" );

   gui_tests_create_power_button_menu();

   //Keep polling the power button until it is active
   while(!(monitor_get_main_power_button_status())) {};

   //Let user know the correct voltage was detected
   gui_tests_display_pass_screen();
   uart1_printf("  Button Detected: SUCCESS\n\r\0" );

}


/*!
* @brief This is used to verify the functionality of the audio hardware
* @param[in] NONE
* @return NONE
*
*/
void
tests_production_audio(void)
{
   uart1_printf("\n\r\n\r\n\rAudio Testing Results\n\r\n\r\0" );

   static uint32_t test_addresses[max_total_addresses]; //Local copy of file addresses from SD card
   sd_get_file_addresses(test_addresses);

   // "0" for headphone detection, "1" for actual audio
   uint8_t current_test = 0;

   //Display the audio headphone detection test menu
   gui_tests_create_audio_menu(current_test);

   //Wait until headphones are plugged in
   while( !(monitor_get_headphone_status())) {};
   uart1_printf("  Headphones Detected: SUCCESS\0" );

   //Let user know headphones were detected
   gui_tests_display_pass_screen();

   //Display the audio sound test menu
   current_test++;
   gui_tests_create_audio_menu(current_test);
   uart1_printf("\n\r  Audio Playback: OPERATOR VERIFICATION REQUIRED\0" );

   //Play a test audio
   dac_enable_audio();
   dac_set_volume(volume_high);
   dac_audio_from_sd(test_addresses[portfolio_data1_audio]);

}


/*!
* @brief This is used to verify the functionality of the LCD backlight switch on the daughter board
* @param[in] NONE
* @return NONE
*
*/
void
tests_production_lcd_backlight(void)
{
   uart1_printf("\n\r\n\rLCD Backlight Testing\n\r\n\r\0" );

   gui_tests_create_power_button_menu();

   //Count down three seconds until the screen resets
   for(uint8_t current_time = 3; current_time != 0; --current_time)
   {
      gui_tests_create_LCD_switch_menu(current_time);
      timers_delay(6000);
   }

   lcd_backlight_disable();
   timers_delay(6000);
   lcd_backlight_enable();

   //Let user know the correct voltage was detected
   uart1_printf("  Screen Reset: OPERATOR VERIFICATION REQUIRED\0" );

}


/* end of file */
