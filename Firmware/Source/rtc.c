/** @file rtc.c
*
* @brief This file contains functions to initialize and access the real time clock of the STM32 MCU
* @author Aaron Vorse
* @date   9/12/2020
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

#include "rtc.h"


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


/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/

/*!
* @brief Initialize the real time clock
* @param[in] NONE
* @return NONE
*
* @note Time is initialized in AM/PM format
*       Prescalers are set for a 32.768KHz external oscillator
*/
void
rtc_init(void)
{
   /* Init RCC Registers */
   PWR->CR |= PWR_CR_DBP;

   RCC->BDCR |= RCC_BDCR_BDRST;  //Reset BDRST
   RCC->BDCR &= ~RCC_BDCR_BDRST;

   RCC->BDCR |= RCC_BDCR_LSEON; //Enable low speed external oscillator

   while(!(RCC->BDCR & RCC_BDCR_LSERDY)); //Wait until low speed clock is ready

   RCC->BDCR |= RCC_BDCR_RTCSEL_0; //Select External oscillator as low speed clock source

   RCC->BDCR |= RCC_BDCR_RTCEN; //Enable the real time clock

   /* Init RTC Registers */
   RTC->WPR = 0xCA; //Override write protection
   RTC->WPR = 0x53;

   RTC->ISR |= RTC_ISR_INIT; //Put RTC registers in init mode

   while(!(RTC->ISR & RTC_ISR_INITF)) {} //Wait until init mode is active

   RTC->PRER &= ~(RTC_PRER_PREDIV_A_Msk); //Set prescale divisors based on 32.768KHz external oscillator
   RTC->PRER |= (127 << RTC_PRER_PREDIV_A_Pos);

   RTC->PRER &= ~(RTC_PRER_PREDIV_S_Msk);
   RTC->PRER |= (255 << RTC_PRER_PREDIV_S_Pos);

   RTC->CR |= RTC_CR_FMT;  //Set RTC to AM/PM format
   RTC->CR |= RTC_CR_ALRAE; //Enable alarm A

   RTC->ISR &= ~RTC_ISR_INIT; //Take RTC registers out of init mode

   RTC->WPR = 0x05;  //Turn on write protection
}


/*!
* @brief Initialize the real time clock and set its initial time
* @param[in] hours Hours in AM/PM format, aka hours <= 12
* @param[in] minutes
* @param[in] seconds
* @return NONE
* @note Time is initialized in AM/PM format
*       Prescalers are set for a 32.768KHz external oscillator
*/
void
rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
   PWR->CR |= PWR_CR_DBP;

   RCC->BDCR |= RCC_BDCR_BDRST;  //Reset BDRST
   RCC->BDCR &= ~RCC_BDCR_BDRST;

   RCC->BDCR |= RCC_BDCR_LSEON; //Enable low speed external oscillator

   while(!(RCC->BDCR & RCC_BDCR_LSERDY)); //Wait until low speed clock is ready

   RCC->BDCR |= RCC_BDCR_RTCSEL_0; //Select External oscillator as low speed clock source

   RCC->BDCR |= RCC_BDCR_RTCEN; //Enable the real time clock

   RTC->WPR = 0xCA; //Override write protection
   RTC->WPR = 0x53;

   RTC->ISR |= RTC_ISR_INIT; //Put RTC registers in init mode

   while(!(RTC->ISR & RTC_ISR_INITF)) {} //Wait until init mode is active

   RTC->PRER &= ~(RTC_PRER_PREDIV_A_Msk); //Set prescale divisors based on 32.768KHz external oscillator
   RTC->PRER |= (127 << RTC_PRER_PREDIV_A_Pos);

   RTC->PRER &= ~(RTC_PRER_PREDIV_S_Msk);
   RTC->PRER |= (255 << RTC_PRER_PREDIV_S_Pos);

   RTC->CR |= (RTC_CR_FMT); //Set RTC to AM/PM format

   RTC->ISR |= RTC_ISR_ALRAWF; //Allow alarm value to be updated dynamically
   RTC->ALRMAR |= (RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2); //Set alarm A to ignore everything except seconds

   RTC->ALRMAR &= ~(RTC_ALRMAR_ST_Msk);
   RTC->ALRMAR |= (((seconds / 10) + 1) << RTC_ALRMAR_ST_Pos); //Init alarm A to trigger at next 10-second overflow

   uint32_t tmp_time = 0;

   tmp_time |= (hours / 10 << RTC_TR_HT_Pos);
   tmp_time |= (hours % 10 << RTC_TR_HU_Pos);
   tmp_time |= (minutes / 10 << RTC_TR_MNT_Pos);
   tmp_time |= (minutes % 10 << RTC_TR_MNU_Pos);
   tmp_time |= (seconds / 10 << RTC_TR_ST_Pos);
   tmp_time |= (seconds % 10 << RTC_TR_SU_Pos);

   RTC->TR |= tmp_time; //Load initial time into the RTC registers

   RTC->CR |= RTC_CR_ALRAE; //Enable alarm A

   RTC->ISR &= ~RTC_ISR_INIT; //Take RTC registers out of init mode
}


/*!
* @brief Increment the RTC alarm value so it continually triggers every 10 seconds
* @param[in] NONE
* @return NONE
*
*/
void
rtc_update_alarm(void)
{
   RTC->CR &= ~(RTC_CR_ALRAE); //Disable alarm

   while(!(RTC->ISR & RTC_ISR_ALRAWF)); //Poll until  access is allowed

   //Read the current alarm value
   uint8_t next_alarm_trigger = ((RTC->ALRMAR & RTC_ALRMAR_ST_Msk) >> RTC_ALRMAR_ST_Pos);

   //Increment alarm to trigger on the next ten second overflow.
   if(5 > next_alarm_trigger)
   {
      next_alarm_trigger++; //Only increment if current trigger value is less than 50 seconds
   }

   else
   {
      //If current trigger value equals 50 seconds, reset it to zero so it doesn't accidently increment past 60 seconds
      next_alarm_trigger = 0;
   }

   RTC->ALRMAR &= ~(RTC_ALRMAR_ST_Msk); //Clear leftover alarm trigger value
   RTC->ALRMAR |= (next_alarm_trigger << RTC_ALRMAR_ST_Pos); //Load next alarm trigger into RTC registers

   RTC->ISR &= ~(RTC_ISR_ALRAF); //Clear alarm flag

   RTC->CR |= RTC_CR_ALRAE; //Enable alarm
}


/*!
* @brief Return the current value of the RTC time registers
* @param[in] tmp_time_buffer Buffer to hold time values:
*            [0] = hours_tens, [1] = hours_uints, [2] = minutes_tens, [3] = minutes_uints,
*            [4] = seconds_tens, [5] = seconds_units
* @return NONE
*
*/
void
rtc_get_time(uint8_t *tmp_time_buffer)
{
   //Hours
   tmp_time_buffer[0] = ((RTC->TR & RTC_TR_HT_Msk) >> RTC_TR_HT_Pos);
   tmp_time_buffer[1] = ((RTC->TR & RTC_TR_HU_Msk) >> RTC_TR_HU_Pos);

   //Minutes
   tmp_time_buffer[2] = ((RTC->TR & RTC_TR_MNT_Msk) >> RTC_TR_MNT_Pos);
   tmp_time_buffer[3] = ((RTC->TR & RTC_TR_MNU_Msk) >> RTC_TR_MNU_Pos);

   //Seconds
   tmp_time_buffer[4] = ((RTC->TR & RTC_TR_ST_Msk) >> RTC_TR_ST_Pos);
   tmp_time_buffer[5] = ((RTC->TR & RTC_TR_SU_Msk) >> RTC_TR_SU_Pos);
}


/*!
* @brief Return the current value of the RTC seconds register
* @param[in] NONE
* @return Current RTC time of seconds_units register
*
*/
uint8_t
rtc_get_seconds(void)
{
   return((RTC->TR & RTC_TR_SU_Msk) >> RTC_TR_SU_Pos);
}


/*
****************************************************
********** Private Function Definitions ************
****************************************************
*/


/* end of file */
