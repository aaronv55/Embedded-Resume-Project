/** @file main.c
*
* @brief
* @author Aaron Vorse
* @date   3/2/2020
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
*
*/

#include "main.h"

/*
****************************************************
********** Private Function Prototypes *************
****************************************************
*/
void main_peripherals_init(void);



/*!
* @brief This is the main system loop. It initializes each peripheral module then jumps into an infinite loop.
*        This loop constantly services the system's various state machines as the user interacts with the device.
* @param[in] NONE
* @return NONE
*
*/
int
main(void)
{

   //Initialize system and peripheral clocks
   system_clock_init();
   
   //Initialize MCU peripherals and seed background state machines
   main_peripherals_init();

   //If the test pin is high on startup, the system will launch into a series of production tests
   tests_production_hardware();


   /* main system loop */
   for(;;)
   {
      //Transition the audio state machine to the next appropriate state
      states_update_audio();

      //Poll for any LCD touch detections, and update positions if needed
      touch_update_position();

      //See if any status bar icons have changed value, or if the background needs to be redrawn
      gui_update_status_bar();

      //See if the background color of the footer has changed, and if it needs to be redrawn
      states_update_footer();

      //Get the current main system event and transition to the appropriate main state accordingly
      states_update_main_event();
      states_update_main_state();
   }

   return(0);
}
   

/*!
* @brief Initializes the MCU's peripherals and system settings and seeds various system state machines
* @param[in] NONE
* @return NONE
*
*/
void
main_peripherals_init(void)
{
   //Init LCD with correct screen/data settings
   lcd_init();

   //Init UART for the UART to USB for serial communication
   uart1_init(BR_PRESCALER_115200); //Set UART1 to 115200 baud rate. For alternatives see uart.h

   //Init SPI2 for communication with SD card and do an initial handshake with the card
   spi_spi2_init();
   microsd_init();

   //Init timer 5 as the source of the DAC audio module, set timer 11 to a rough 1 second overflow, start the audio state machine
   timers_timer5_init();
   timers_timer11_init();
   dac_init();

   //Init the ADCs associated with the touch sensing module and seed the touch sense state machine
   touch_adc_init();
   touch_idle_state();

   //Init background sense pins that monitor USB/headphone insertions, power button clicks, and battery level
   monitor_jacksense_init();
   monitor_usbsense_init();
   monitor_power_button_init();
   monitor_batterysense_init();

   //Init the RTC and seed it with a default value of 12:00pm
   rtc_init();
   rtc_set_time(12, 00, 00);

   //Init board settings for hardware production testing
   tests_init();

   //Start the main system state machine
   states_init();

   //Increase SPI to maximum bus speed for faster transfers
   spi_set_clk_high_speed();
}

/*** end of file ***/
