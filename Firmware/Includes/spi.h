/** @file spi.h
*
* @brief  This file contains drivers to initialize SPI and use it to send/receive data
* @author Aaron Vorse
* @date   7/8/2020
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

#ifndef SPI_H
#define SPI_H

#define SPI2_MOSI GPIOC,3
#define SPI2_MISO GPIOC,2
#define SPI2_SCK GPIOC,7
#define SPI2_CS GPIOC,8

#define spi_set_clk_low_speed()  SPI2->CR1 |= (0x07 << SPI_CR1_BR_Pos); //50MHz/256 = 200KHz
#define spi_set_clk_med_speed()  SPI2->CR1 |= (0x01 << SPI_CR1_BR_Pos); //50MHz/4 = 12.5MHz
#define spi_set_clk_high_speed() SPI2->CR1 &= ~(0x07 << SPI_CR1_BR_Pos); //50MHz/2 = 25MHz

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f410rx.h"
#include "base_gpio_drivers.h"


/*
****************************************************
******* Public Function Defined in spi.c ***********
****************************************************
*/
/* Public functions defined in spi.c */
void spi_spi2_init(void);
void spi_send_byte(uint8_t tmp_byte);
uint8_t spi_receive_byte(uint8_t dummy_byte);

#endif /* SPI_H */

/* end of file */
