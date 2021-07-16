/** @file microsd.h
*
* @brief  This file contains drivers for interacting with an external microSD card through SPI
* @author Aaron Vorse
* @date   7/9/2020
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

#ifndef MICROSD_H
#define MICROSD_H

#define SD_TRANSMISSION_BIT 0x40
#define SD_DUMMY_CRC 0x01
#define SD_CMD8_VHS_3v3 (0x01 << 8)
#define SD_CMD8_CHECK_PATTERN 0xAA
#define SD_CMD8_DATA (SD_CMD8_VHS_3v3 | SD_CMD8_CHECK_PATTERN)
#define SD_CMD8_CRC (0x86 | 0x01)
#define SD_ACMD41_DATA (0x40 << 24) //Bit 30 of ACMD41, high capacity select
#define SD_RESPONSE3_3V2_3V4 0X60 //SD Card accepts both 3.2v-3.3v and 3.3v-3.4v
#define SD_RESPONSE3_POWER_UP 0X80
#define SD_RESPONSE3_CCS 0x40
#define SD_CMD17_TOKEN 0xFE

/******************* File addresses *******************/
#define SD_ADDRESS_CHEAT_SHEET 4000000
#define SD_ADDRESS_LIST_OFFSET 0


#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f410rx.h"
#include "base_gpio_drivers.h"
#include "spi.h"
#include "timers.h"
#include "enum_sd_file_list.h"
#include "uart.h"
#include "lcd.h"
#include "personal_function_toolbox.h"

/*
****************************************************
******* Public Function Defined in microsd.c *******
****************************************************
*/
uint8_t microsd_init(void);
void sd_read_block(uint8_t *p_read_buffer, uint32_t block_address);
void sd_write_block(uint8_t *tmp_write_buffer, uint32_t block_address);
uint32_t sd_find_file_address(const uint8_t *file_identifier);
void sd_read_multiple_block(uint32_t start_address);
void sd_print_block(uint64_t block_address);
void sd_stop_transmission(void);
void sd_search_file_addresses(void);
void sd_get_file_addresses(uint32_t *tmp_file_list);
void sd_append_file_addresses(e_sd_address start_address, e_sd_address stop_address);
uint32_t sd_get_wav_size(uint8_t tmp_file);

#endif /* MICROSD */

/* end of file */
