/** @file timers.c
*
* @brief  This file contains drivers for internal timers, timer interrupts, and basic delays
* @author Aaron Vorse
* @date   4/12/2020
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

#include "timers.h"

/* File-Static Variables */
static uint8_t timer11_interrupt_flag = 0;
static uint16_t current_audio_clock = 0; //Seconds counter used to keep track the elapsed time an audio has been playing

/*
****************************************************
************* File-Static Variables ****************
****************************************************
*/
void timer_timer11_gpio_init(void);

/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/


/*!
* @brief Initialize the timer used by the system's audio clock
* @param[in]
* @return  NONE
* @note Timer is initialized to 22,050Hz.This is the sampling rate
*       of all WAV files in this system.
* @warning Do not enable timer 5 here. The constant timer 5 interrupts cause the
*          system to have timing errors. Timer 5 should only be enabled
*          within a function that uses it, then disabled upon returning.
*/
void
timers_timer5_init(void)
{
   //Enable main timer clock
   RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;


   //Fill timer prescale registers. Formula: total_prescale = (PSC x ARR)
   TIM5->PSC = 0;
   TIM5->ARR = 4535; //100,000,000/4535 = 22,050Hz.

   //Set timer overflow as interrupt source
   TIM5->CR1 |= TIM_CR1_URS;

   //Enable timer interrupts
   TIM5->DIER |= TIM_DIER_UIE;

   //Set timer to trigger output. This is needed to trigger DMA
   TIM5->CR2 |= TIM_CR2_MMS_1;

   //Push setting changes to timer5
   TIM5->EGR |= TIM_EGR_UG;

   //Initialize timer5 ISR
   NVIC_EnableIRQ(TIM5_IRQn);

   //Enable timer
   //TIM5->CR1 |= TIM_CR1_CEN;
}


/*!
* @brief Initialize a rough 1 second counter used to monitor the length of certain background tasks
* @param[in] 
* @return  NONE
* @note Timer is initialized to 1S
*/
void
timers_timer11_init(void)
{
   RCC->APB2ENR |= RCC_APB2ENR_TIM11EN; //Enable APB2 clock
   TIM11->ARR = 10000UL; //Set Autoreload
   TIM11->PSC = 9999UL; //Initialize Prescaler

   TIM11->EGR = TIM_EGR_UG; //Update Event
   TIM11->CCER &= ~(TIM_CCER_CC1E); //Disable Channel 1 output
   TIM11->CCMR1 |= TIM_CCMR1_OC1PE; //Enable Preload register

   //Enable timer interrupts
   TIM11->DIER |= TIM_DIER_UIE;

   //Push setting changes to timer11
   TIM11->EGR |= TIM_EGR_UG;

   /* TIM11 interrupt Init */
   NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 2);
   NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);

}

/*!
* @brief Update the amount of time that has elapsed since the current audio was started
* @param[in] NONE
* @return  NONE
*/
void
timers_update_audio_clock(void)
{

   //If timer11 has overflowed, another second should be added to the time
   if(1 == timer11_interrupt_flag)
   {
      current_audio_clock++;
      timer11_interrupt_flag = 0; //Reset flag
   }

}


/*!
* @brief Get the amount of time that has elapsed since the current audio was started
* @param[in] NONE
* @return  current_audio_clock Elapsed time in seconds
*
* @warning current_audio_clock is specifically updated by a middle function that monitors the
*          interrupt flag of timer11. Do not change current_audio_clock to update in the interrupt
*          service routine itself, or a race condition will be introduced in this function.
*/
uint16_t
timers_get_audio_clock(void)
{
   return(current_audio_clock);
}


/*!
* @brief Reset the audio clock, so the next time the function is used it does
*        not return a value from the previous audio
* @param[in] NONE
* @return  NONE
*/
void
timers_reset_audio_clock(void)
{
   current_audio_clock = 0;
}


/*!
* @brief Converts a uint16 number of seconds to a char array representing
*        minutes and seconds.
* @param[in] tmp_counter_value Current time in seconds to be converted
* @param[in] converted_time char array to output the final time value
* @return  NONE
*
* @warning The maximum time that can be converted is 9:59, or 599 seconds
* @note The output will be in the form: "minutes : seconds"
*       Example "3:45"
*/
void
timers_uint16_to_time(uint16_t tmp_counter_value, char *converted_time)
{
   static const char minutes_lookup_table[10] =
   {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
   };

   static const char seconds_lookup_table[60][2] =
   {
      "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
      "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
      "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
      "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
      "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
      "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"
   };

   //Bounds check. Max possible conversion time is 599 seconds
   if(599 > tmp_counter_value)
   {
      converted_time[0] = minutes_lookup_table[tmp_counter_value / 60]; //Minutes
      converted_time[1] = ':';

      uint8_t tmp_seconds = (tmp_counter_value % 60);
      converted_time[2] = seconds_lookup_table[tmp_seconds][0]; //Seconds high
      converted_time[3] = seconds_lookup_table[tmp_seconds][1]; //Seconds low
      converted_time[4] = '\0'; //Termination character
   }
}


/*!
* @brief Gives a single, rising edge clock pulse
* @param[in] p_gpio_tmp Pointer to GPIO structure.
* @param[in] pin_number Pin number of desired port
* @return  NONE
*/
void
timers_gpio_rising_edge_pulse(GPIO_TypeDef * p_gpio_tmp, uint8_t pin_number)
{
   gpio_clear(p_gpio_tmp, pin_number);
   gpio_set(p_gpio_tmp, pin_number);
}

/*!
* @brief Gives a single, falling edge clock pulse
* @param[in] p_gpio_tmp Pointer to GPIO structure.
* @param[in] pin_number Pin number of desired port
* @return  NONE
*/
void
timers_gpio_falling_edge_pulse(GPIO_TypeDef * p_gpio_tmp, uint8_t pin_number)
{
   gpio_set(p_gpio_tmp, pin_number);
   gpio_clear(p_gpio_tmp, pin_number);
}


/*!
* @brief Enables Timer11 channel 1 output, which pushes the current timer logic value to a specified GPIO
* @param[in] NONE
* @return  NONE
*/
void
timers_timer11_output_enable(void)
{
   TIM11->CCER |= TIM_CCER_CC1E;   
}
   
/*!
* @brief Disables Timer11 channel 1 output
* @param[in] NONE
* @return  NONE
* @note If GPIO is open drain, output of channel 1
*       will go into high z mode. 
*/
void
timers_timer11_output_disable(void)
{
   TIM11->CCER &= ~TIM_CCER_CC1E; 
}


/*!
* @brief Bad delay that locks the processor
* @param[in] NONE
* @return  NONE
*/
void
timers_delay(uint32_t delay_time)
{
   uint32_t i = 0;
   for(i = 0; i < (delay_time * 1000); i++) {} //lock MCU until delay time finishes
}


/*!
* @brief Bad delay that locks the processor
* @param[in] NONE
* @return  NONE
*/
void
timers_delay_mini(uint32_t delay_time)
{
   uint32_t i = 0;
   for(i = 0; i < (delay_time);i++) {} //lock MCU until delay time finishes
}


/*!
* @brief Return the current interrupt status of timer11
* @param[in] NONE
* @return  timer11_interrupt_flag Flag to indicate if a timer11 interrupt has occurred
*
* @note If an interrupt has occurred, it will return 1, else zero.
*       This only reads the status, a call to timers_timer11_reset_status()
*       should be given to reset the interrupt status
*/
uint8_t
timers_timer11_get_status(void)
{
   return(timer11_interrupt_flag);
}

/*!
* @brief Clear the current interrupt status of timer11 back to no interrupt
* @param[in] NONE
* @return  NONE
*
*/
void
timers_timer11_reset_status(void)
{
   timer11_interrupt_flag = 0;
}


/* Private Function Definitions */

/*!
* @brief Initialize PC12 as Timer11 channel1 output
* @param[in] NONE
* @return  NONE
* @warning Pin must be left as open drain, since it feeds the external counter
*          input, and shares the external pin with another GPIO. It must be able to be
*          placed into high Z mode.
*          Output needs 10k external pull up resistor. 
*/
void
timer_timer11_gpio_init(void)
{
   gpio_clk_init(TIMER11_CHANNEL1);
   gpio_func_init(TIMER11_CHANNEL1, GPIO_MODER_ALTERNATE_FUNCTION);
   gpio_type_init(TIMER11_CHANNEL1, GPIO_OTYPER_PUSH_PULL); //////////////////////////////////
   //gpio_type_init(TIMER11_CHANNEL1, GPIO_OTYPER_OPEN_DRAIN);
   gpio_speed_init(TIMER11_CHANNEL1, GPIO_OSPEEDR_VERY_HIGH);
   gpio_pupd_init(TIMER11_CHANNEL1, GPIO_PUPDR_NONE);
   GPIOC->AFR[1] |= GPIO_AFRH_PIN12_AF3; 
}

/*!
* @brief Timer 5 interrupt handler. Used as a source for the audio DAC
* @param[in] NONE
* @return  NONE
*/
void
TIM5_IRQHandler(void)
{

   TIM5->SR &= ~TIM_SR_UIF;
}


/*!
* @brief Timer 11 interrupt handler.
* @param[in] NONE
* @return  NONE
*/
void
TIM1_TRG_COM_TIM11_IRQHandler(void)
{

   timer11_interrupt_flag = 1;
   TIM11->SR &= ~TIM_SR_UIF; //Reset timer interrupt flag
}



/* end of file */
