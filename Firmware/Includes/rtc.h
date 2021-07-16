/** @file rtc.h
*
* @brief This file contains functions to initialize and access the real time clock of the STM32F410 MCU
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

#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f410rx.h"
#include "base_gpio_drivers.h"

/*
****************************************************
******* Public Function Defined in rtc.c *******
****************************************************
*/
void rtc_init(void);
void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
void rtc_get_time(uint8_t *tmp_time_buffer);
uint8_t rtc_get_seconds(void);
void rtc_update_alarm(void);

#endif /* RTC_H */

/* end of file */
