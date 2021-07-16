/** @file monitor.h
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

#ifndef MONITOR_H
#define MONITOR_H

#define MONITOR_JACKSENSE_PIN GPIOA,3
#define MONITOR_USBSENSE_PIN GPIOC,1
#define MONITOR_BATTERYSENSE_PIN GPIOA,7
#define MONITOR_MAIN_POWER_BUTTON GPIOA,0
#define ADC_SQR3_SQ1_CHANNEL7 7ul
#define ADC_SMPR2_SMP7_15_CYCLES (1ul << 21)

#define BATTERY_MONITOR_VERSION_1 1//This should be used if you're compiling for any PCB revision before rev C
#define BATTERY_MONITOR_VERSION_2 2//This should be used if you're compiling for any PCB revision after rev B

#define BATTERY_MONITOR_VERSION BATTERY_MONITOR_VERSION_2
#define NUMBER_OF_BATTERY_SAMPLES 25
#define BATTERY_SAMPLE_FLAG_SAMPLE 1
#define BATTERY_SAMPLE_FLAG_NONE 0

#define MAIN_POWER_SHORT_PRESS 0
#define MAIN_POWER_LONG_PRESS 1

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f410rx.h"
#include "base_gpio_drivers.h"
#include "touch.h"

/*
****************************************************
******* Public Function Defined in monitor.c *******
****************************************************
*/
void monitor_jacksense_init(void);
void monitor_usbsense_init(void);
void monitor_batterysense_init(void);
uint8_t monitor_get_usb_status(void);
uint8_t monitor_get_headphone_status(void);
uint8_t monitor_get_battery_level(uint8_t sample_flag);
uint16_t monitor_get_raw_battery_level(uint8_t sample_flag);
void monitor_power_button_init(void);
uint8_t monitor_get_main_power_button_status(void);
void monitor_reset_main_power_button_status(void);
uint8_t monitor_get_power_press_type(void);

#endif /* MONITOR_H */

/* end of file */
