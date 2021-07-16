/** @file monitor.c
*
* @brief  This file contains functions that monitor various background aspects of the system
*         like battery voltage, headphone and USB sense etc.
* @author Aaron Vorse
* @date   8/15/2020
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

#include "monitor.h"

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
static volatile uint8_t monitor_jacksense_detection = 0;
static volatile uint8_t monitor_power_button_detection = 0;


/*
****************************************************
********** Private Function Prototypes *************
****************************************************
*/
uint8_t monitor_adc_to_battery_level(uint16_t current_adc_value);


/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/

/**
* @brief Initialize usbsense pin as input
* @param[in] NONE
* @return NONE
*
* @note EXTI1 is already used for the touch_sense interrupt generated
*       when the LCD screen is touched. USBsense simply acts as a
*       GPIO input and should be polled in its retaining loop
*/
void
monitor_usbsense_init(void)
{
   //Initialize pin to input, no pull-up/down, low speed
   gpio_gen_input_init(MONITOR_USBSENSE_PIN);

}


/**
* @brief Get the current state of usbsense
* @param[in] NONE
* @return usb_sense Returns 2 if USB charger is plugged in, else 0. Returning 2 for TRUE
*                   saves a few instructions by not having to shift the result to the right.
*/
uint8_t
monitor_get_usb_status(void)
{
   //Read usbsense pin
   return(GPIOC->IDR & 0x02);
}


/**
* @brief Initialize PA3 to trigger an interrupt when headphones are removed or plugged in
* @param[in] NONE
* @return NONE
*
* @note Jacksense pin is PA3.
*       Logic low = headphones unplugged
*       Logic high = headphones plugged in
*/
void
monitor_jacksense_init(void)
{
   //Initialize pin to input, no pull-up/down, low speed
   gpio_gen_input_init(MONITOR_JACKSENSE_PIN);

   //Unmask interrupt for pin 3
   EXTI -> IMR |= EXTI_IMR_IM3;

   //Set pin as an interrupt
   SYSCFG -> EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA; //Tie port A to interrupt line 0
   EXTI ->RTSR |= EXTI_RTSR_TR3; // Set trigger pin 3 to rising edge
   EXTI ->FTSR |= EXTI_FTSR_TR3; // Set trigger pin 3 to falling edge
   NVIC_SetPriority(EXTI3_IRQn,2); // Set interrupt priority to 2
   NVIC_EnableIRQ(EXTI3_IRQn);

   //Read the initial state of jacksense, so the system doesn't have to wait for the headphones
   //to be plugged in (triggering an interrupt) in order to display the headphone icon if already plugged in.
   if(gpio_read_pin(MONITOR_JACKSENSE_PIN))
   {
      monitor_jacksense_detection = 1;
   }
}


/**
* @brief Initialize the batterysense pin as an analog input
* @param[in] NONE
* @return NONE
* */
void
monitor_batterysense_init(void)
{
   //Set battery sense pin to basic analog mode
   gpio_func_init(MONITOR_BATTERYSENSE_PIN, GPIO_MODER_ANALOG);//Set pins to analog mode
   gpio_pupd_init(MONITOR_BATTERYSENSE_PIN,GPIO_PUPDR_NONE);//No pull up/pull down
   gpio_speed_init(MONITOR_BATTERYSENSE_PIN,GPIO_OSPEEDR_LOW);//Low speed

   //Set ADC sample wait states
   ADC1->SMPR2 |= (ADC_SMPR2_SMP7_15_CYCLES);
}


/**
* @brief Take multiple voltage samples of the battery and return their average
* @param[in] sample_flag Flag used to trigger data sampling only when requested and not every pass of the main loop
* @return battery_level
*
* @warning Make sure the FPU is enabled or this will introduce large amounts of latency
* */
uint8_t
monitor_get_battery_level(uint8_t sample_flag)
{
   //Do not initialize battery level below 6, or the system will purposely shutdown on startup
   static uint8_t battery_level = 20;

   //Only sample the ADC if the sample flag is 1
   if(sample_flag)
   {
      //Disable ADC to change settings
      adc_disable();

      //Set ADC to single channel sampling
      ADC1->SQR1 &= ~(0xFF << 20);

      //Set scan order to scan PA7 (battery sense pin)
      ADC1->SQR3 &= ~0x1F; //Clear register
      ADC1->SQR3 |= (ADC_SQR3_SQ1_CHANNEL7);

      //Enable ADC after settings change
      adc_enable();

      uint16_t sample_buffer[NUMBER_OF_BATTERY_SAMPLES] = {0};

      //Take multiple different samples
      for(uint8_t current_sample = 0; current_sample < NUMBER_OF_BATTERY_SAMPLES; current_sample++)
      {
         //Start conversion
         ADC1->CR2 |= ADC_CR2_SWSTART;

         //Poll for end of conversion
         while(!((ADC1->SR & ADC_SR_EOC) >> ADC_SR_EOC_Pos));

         //Get data for channel 7
         sample_buffer[current_sample] = ADC1->DR;

         //Clear status flags
         ADC1->SR = 0;
      }

      //Disable ADC to change settings
      adc_disable();

      //Average the samples
      float average_samples = 0; //Float needed to avoid rounding errors

      for(uint8_t current_sample = 0; current_sample < NUMBER_OF_BATTERY_SAMPLES; current_sample++)
      {
         average_samples += sample_buffer[current_sample];
      }

      average_samples /= NUMBER_OF_BATTERY_SAMPLES;

      //Remap the raw ADC values to a battery percentage of 0 - 100
      battery_level = monitor_adc_to_battery_level(average_samples);
   }

   return(battery_level);
}


/**
* @brief Take multiple voltage samples of the battery and return their average
* @param[in] sample_flag Flag used to sample data only when requested and not every pass of the main loop
* @return battery_level
*
* */
uint16_t
monitor_get_raw_battery_level(uint8_t sample_flag)
{
   static uint16_t battery_level = 20;

   //Only sample the ADC if the sample flag is 1
   if(sample_flag)
   {
      //Disable ADC to change settings
      adc_disable();

      //Sample a single channel
      ADC1->SQR1 &= ~(0xFF << 20);

      //Set scan order to scan PA7 (battery sense pin)
      ADC1->SQR3 &= ~0x1F; //Clear register
      ADC1->SQR3 |= (ADC_SQR3_SQ1_CHANNEL7);

      //Enable ADC after settings change
      adc_enable();

      uint16_t sample_buffer[NUMBER_OF_BATTERY_SAMPLES] = {0};

      //Take "NUMBER_OF_BATTERY_SAMPLES" number of samples
      for(uint8_t current_sample = 0; current_sample < NUMBER_OF_BATTERY_SAMPLES; current_sample++)
      {
         //Start conversion
         ADC1->CR2 |= ADC_CR2_SWSTART;

         //Poll for end of conversion
         while(!((ADC1->SR & ADC_SR_EOC) >> ADC_SR_EOC_Pos));

         //Get data for channel 7
         sample_buffer[current_sample] = ADC1->DR;

         //Clear status flags
         ADC1->SR = 0;

      }

      //Disable ADC to change settings
      adc_disable();

      //Average the samples
      float average_samples = 0; //Float needed to avoid rounding errors

      for(uint8_t current_sample = 0; current_sample < NUMBER_OF_BATTERY_SAMPLES; current_sample++)
      {
         average_samples += sample_buffer[current_sample];
      }

      average_samples /= NUMBER_OF_BATTERY_SAMPLES;

      battery_level = (uint16_t)(average_samples);
   }

   return(battery_level);
}


/**
* @brief Get the current state of usbsense
* @param[in] NONE
* @return usb_sense Returns 2 if headphones are plugged in, else 0
*
* @note This is added for encapsulation. Other files should not
*       have access to the variable monitor_jacksense_detection
*       which is therefore global only to this file, monitor.c
*/
uint8_t
monitor_get_headphone_status(void)
{
   return(monitor_jacksense_detection);
}


/**
* @brief Get the current state of the main external user button
* @param[in] NONE
* @return monitor_power_button_detection
*
*/
uint8_t
monitor_get_main_power_button_status(void)
{
   return(monitor_power_button_detection);
}


/**
* @brief Reset the current state of the main external user button to not clicked
* @param[in] NONE
* @return NONE
*/
void
monitor_reset_main_power_button_status(void)
{
   monitor_power_button_detection = 0;
}


/**
* @brief Initialize external button (the main power button) as an interrupt pin
* @param[in] NONE
* @return NONE
*
*/
void
monitor_power_button_init(void)
{
   //Initialize pin to input, no pull-up/down, low speed
   gpio_gen_input_init(MONITOR_MAIN_POWER_BUTTON);

   //Unmask interrupt for pin 3
   EXTI -> IMR |= EXTI_IMR_IM0;

   //Set pin as an interrupt
   SYSCFG -> EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA; //Tie port A to interrupt line 0
   EXTI ->FTSR |= EXTI_FTSR_TR0; // Set trigger pin 3 to falling edge, it has an external pullup
   NVIC_SetPriority(EXTI0_IRQn,2); // Set interrupt priority to 2
   NVIC_EnableIRQ(EXTI0_IRQn);
}


/**
* @brief Determine if the main power button is clicked shortly or if it is held down
* @param[in] NONE
* @return tmp_press_type The type (either SHORT or LONG) of the button press
*
*@warning This function locks the system in a loop for roughly 3 seconds. It should only ever
*         be called before entering or leaving a sleep state, so this should not be a problem.
*@note All other threads (audio, sd card communication) should be killed before calling this.
*/
uint8_t
monitor_get_power_press_type(void)
{
   uint32_t time_pressed = 0;
   uint32_t timeout = 120;

   //Continually increase the count while the button remains pressed
   while( (!(gpio_read_pin(MONITOR_MAIN_POWER_BUTTON))) && (time_pressed < timeout))
   {
      time_pressed++;
      timers_delay(100);
   }

   uint8_t tmp_press_type = MAIN_POWER_SHORT_PRESS;

   //If the button was held beyond the timeout length,
   if(timeout == time_pressed)
   {
      tmp_press_type = MAIN_POWER_LONG_PRESS;
   }

   return(tmp_press_type);
}

/*
****************************************************
********** Private Function Definitions ************
****************************************************
*/

/**
* @brief ISR that is called when PA0 experiences a
*        rising edge (main power button is pressed)
* @param[in] NONE
* @return  NONE
*/
void
EXTI0_IRQHandler(void)
{
   monitor_power_button_detection = 1;

   //Clear interrupt flag
   EXTI->PR = EXTI_PR_PR0;
}


/**
* @brief ISR that is called when PA3 experiences a falling or
*        rising edge (headphone is plugged in or taken out)
* @param[in] NONE
* @return  NONE
*/
void
EXTI3_IRQHandler(void)
{

   //If pin is logic high (headphones plugged in)
   if(GPIOA->IDR & 0x08)
   {
      monitor_jacksense_detection = 1;
   }

   else
   {
      monitor_jacksense_detection = 0;
   }

   //Clear interrupt flag
   EXTI->PR = EXTI_PR_PR3;
}


/**
* @brief Remap the raw ADC values 0 - 4095 to a battery percentage of 0 - 100
* @param[in] current_adc_value Raw ADC reading
* @return  tmp_battery_level Converted battery level
*/
uint8_t
monitor_adc_to_battery_level(uint16_t current_adc_value)
{
   //Initialize remapping parameters to board revisions C and newer
   uint16_t battery_level_min = 1400;
   uint16_t battery_level_max = 2200;
   uint16_t divisor = 8; //Do not set to zero, or the system may accidently divide by zero and end up in a hard fault

   //If the board revision is older than rev C, use the old remapping parameters.
   if(BATTERY_MONITOR_VERSION == BATTERY_MONITOR_VERSION_1)
   {
      battery_level_max *= 2;
      battery_level_min *= 2;
      divisor *= 2;
   }

   //Restrict ADC values within the range of min and max battery level
   if(current_adc_value > battery_level_max)
   {
      current_adc_value = battery_level_max;
   }

   else if (current_adc_value < battery_level_min)
   {
      current_adc_value = battery_level_min;
   }

   //Remap the ADC values to a battery percentage from 0 - 100
   uint16_t tmp_battery_level = ((current_adc_value - battery_level_min) / divisor);

   return((uint8_t)(tmp_battery_level));

}


/* end of file */
