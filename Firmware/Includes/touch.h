/** @file touch.h
*
* @brief  This file contains drivers for initializing and reading the resistive touchscreen of the LCD
* @author Aaron Vorse
* @date   7/23/2020
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

#ifndef TOUCH_H
#define TOUCH_H

#define TOUCH_X_PLUS GPIOA,1
#define TOUCH_Y_PLUS GPIOA,2
#define TOUCH_X_MINUS GPIOA,6
#define TOUCH_Y_MINUS GPIOA,4
#define adc_enable() ADC1->CR2 |= ADC_CR2_ADON;
#define adc_disable() ADC1->CR2 &= ~ADC_CR2_ADON;

#define ADC_SQR1_2_CONVERSIONS (1ul << 20)
#define ADC_SQR3_SQ1_CHANNEL1 1ul
#define ADC_SQR3_SQ1_CHANNEL2 2ul
#define ADC_SQR3_SQ2_CHANNEL2 (2ul << 5)
#define ADC_CCR_ADCPRE_DIV4 (1ul << 16)
#define ADC_SMPR2_SMP1_15_CYCLES (1ul << 3)
#define ADC_SMPR2_SMP2_15_CYCLES (1ul << 6)

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f410rx.h"
#include "base_gpio_drivers.h"
#include "timers.h"
#include "personal_function_toolbox.h"
#include "uart.h"

/*
****************************************************
******** Public Function Defined in touch.c ********
****************************************************
*/
void touch_adc_init(void);
void touch_adc_xy_sample(uint16_t *tmp_adc_samples);
uint16_t touch_read_x_position(void);
uint16_t touch_read_y_position(void);
void touch_idle_state(void);
void EXTI1_IRQHandler(void);
void touch_update_position(void);
void touch_get_position(uint16_t *tmp_position);

#endif /* TOUCH_H */

/* end of file */
