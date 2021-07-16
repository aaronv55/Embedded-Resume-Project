/** @file microsd.c
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

#include "microsd.h"

#pragma GCC push_options
#pragma GCC optimize ("O3")

/*
****************************************************
***** Private Types and Structure Definitions ******
****************************************************
*/
typedef enum e_sd_file_type
{
   bmp_file,
   wav_file,
   txt_file

} e_sd_file_type;


typedef struct t_sd_file
{
   const e_sd_file_type type;
   const uint8_t identifier[5];
   uint32_t address;
   uint32_t size;

} t_sd_file;

/*
****************************************************
************* File-Static Variables ****************
****************************************************
*/

//List of all files on the SD card
//@warning The file order must match with the list in enum_sd_file_list.h
static t_sd_file file_list[max_total_addresses] =
{
      {.type = bmp_file, .identifier = {0x00, 0x00, 0x00, 0x00, 0x00}, .address = 0}, //null_address

      /*** App Company ***/
      {.type = wav_file, .identifier = {0x25, 0x78, 0x99, 0x1B, 0x65}, .address = 0}, //company_audio
      {.type = bmp_file, .identifier = {0x25, 0x78, 0x99, 0x1B, 0x66}, .address = 0}, //company_image

      /*** Core System Files ***/
      {.type = wav_file, .identifier = {0x42, 0x7A, 0x97, 0x8D, 0x99}, .address = 0}, //menu_button_audio
      {.type = bmp_file, .identifier = {0x11, 0x03, 0x35, 0xBD, 0xF8}, .address = 0}, //homescreen_picture
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x00}, .address = 0}, //startup_animation_0
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x01}, .address = 0}, //startup_animation_1
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x02}, .address = 0}, //startup_animation_2
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x03}, .address = 0}, //startup_animation_3
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x04}, .address = 0}, //startup_animation_4
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x05}, .address = 0}, //startup_animation_5
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x06}, .address = 0}, //startup_animation_6
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x07}, .address = 0}, //startup_animation_7
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x08}, .address = 0}, //startup_animation_8
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x09}, .address = 0}, //startup_animation_9
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x0A}, .address = 0}, //startup_animation_10
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x0B}, .address = 0}, //startup_animation_11
      {.type = bmp_file, .identifier = {0x58, 0x48, 0xD0, 0x5F, 0x0C}, .address = 0}, //startup_animation_12 This is the startup title

      /*** App References ***/
      {.type = bmp_file, .identifier = {0x15, 0x75, 0x26, 0x54, 0xC4}, .address = 0}, // person1_large
      {.type = bmp_file, .identifier = {0x18, 0x91, 0xBA, 0xDC, 0x00}, .address = 0}, // person1_small_pressed
      {.type = bmp_file, .identifier = {0x25, 0x51, 0x22, 0xDF, 0xD6}, .address = 0}, // person1_small_not_pressed
      {.type = bmp_file, .identifier = {0x55, 0x68, 0x79, 0x53, 0x25}, .address = 0}, // person3_large
      {.type = bmp_file, .identifier = {0x89, 0x28, 0xCD, 0xF9, 0x33}, .address = 0}, // person3_small_pressed
      {.type = bmp_file, .identifier = {0x78, 0x98, 0xD9, 0x6B, 0x58}, .address = 0}, // person3_small_not_pressed
      {.type = bmp_file, .identifier = {0xDF, 0x34, 0x97, 0xAC, 0x00}, .address = 0}, // person2_large
      {.type = bmp_file, .identifier = {0xEA, 0xCB, 0x59, 0x56, 0x25}, .address = 0}, // person2_small_pressed
      {.type = bmp_file, .identifier = {0xBE, 0x52, 0x32, 0x87, 0x62}, .address = 0}, // person2_small_not_pressed
      {.type = bmp_file, .identifier = {0x62, 0x55, 0x20, 0x99, 0x01}, .address = 0}, //person4_large
      {.type = bmp_file, .identifier = {0x62, 0x55, 0x20, 0x99, 0x02}, .address = 0}, //person4_small_pressed
      {.type = bmp_file, .identifier = {0x62, 0x55, 0x20, 0x99, 0x03}, .address = 0}, //person4_small_not_pressed

      /*** App Contact ***/
      {.type = bmp_file, .identifier = {0x54, 0x99, 0x98, 0x75, 0x00}, .address = 0}, // aaron_large
      {.type = bmp_file, .identifier = {0xDF, 0xA3, 0x77, 0x86, 0x51}, .address = 0}, //github_logo_light

      /*** App Skills ***/
      {.type = bmp_file, .identifier = {0xDF, 0xA3, 0x77, 0x86, 0x50}, .address = 0}, // github_logo
      {.type = bmp_file, .identifier = {0x65, 0x4D, 0x6F, 0x45, 0x80}, .address = 0}, // linkedin_logo
      {.type = bmp_file, .identifier = {0xDA, 0x52, 0x55, 0x96, 0xD0}, .address = 0}, // skills_arm
      {.type = bmp_file, .identifier = {0x65, 0x20, 0x02, 0x02, 0x98}, .address = 0}, // skills_circuit
      {.type = bmp_file, .identifier = {0x58, 0xAD, 0xFD, 0xFC, 0x54}, .address = 0}, // skills_c
      {.type = bmp_file, .identifier = {0xEC, 0x89, 0x81, 0x16, 0x51}, .address = 0}, // skills_equipment
      {.type = bmp_file, .identifier = {0x65, 0x18, 0x9D, 0xAF, 0x10}, .address = 0}, // skills_pcb
      {.type = bmp_file, .identifier = {0x55, 0x5D, 0x5A, 0xF5, 0x15}, .address = 0}, // skills_solder

      /*** App Portfolio ***/
      {.type = bmp_file, .identifier = {0x56, 0x72, 0x93, 0x05, 0xDF}, .address = 0}, //slide_portfolio_acq_adc
      {.type = bmp_file, .identifier = {0xAB, 0x99, 0x8B, 0xE0, 0x00}, .address = 0}, //slide_portfolio_acq_bode
      {.type = bmp_file, .identifier = {0xBE, 0xA0, 0x92, 0x34, 0x80}, .address = 0}, //slide_portfolio_acq_breadboard
      {.type = bmp_file, .identifier = {0x98, 0x45, 0x60, 0x98, 0x45}, .address = 0}, //slide_portfolio_acq_gui
      {.type = bmp_file, .identifier = {0x00, 0x78, 0x50, 0x65, 0x40}, .address = 0}, //slide_portfolio_acq_filter
      {.type = bmp_file, .identifier = {0x21, 0x54, 0x55, 0x41, 0x25}, .address = 0}, //slide_portfolio_acq_memory
      {.type = bmp_file, .identifier = {0x65, 0x74, 0x88, 0x77, 0x45}, .address = 0}, //slide_portfolio_fobo_code
      {.type = bmp_file, .identifier = {0x20, 0x50, 0x40, 0x55, 0x01}, .address = 0}, //slide_portfolio_fobo_standing
      {.type = bmp_file, .identifier = {0xEE, 0x65, 0x94, 0xE5, 0x00}, .address = 0}, //slide_portfolio_fobo_fritzing
      {.type = bmp_file, .identifier = {0x01, 0x10, 0x25, 0x80, 0xFF}, .address = 0}, //slide_portfolio_fobo_leg
      {.type = bmp_file, .identifier = {0x76, 0x45, 0x65, 0x4E, 0x0F}, .address = 0}, //slide_portfolio_tamogatchi_code
      {.type = bmp_file, .identifier = {0x31, 0x25, 0x61, 0x10, 0x01}, .address = 0}, //slide_portfolio_tamogatchi_case
      {.type = bmp_file, .identifier = {0x98, 0x45, 0x25, 0x20, 0x00}, .address = 0}, //slide_portfolio_tamogatchi_pcb3D
      {.type = bmp_file, .identifier = {0x87, 0x95, 0x22, 0x54, 0x20}, .address = 0}, //slide_portfolio_tamogatchi_pcbactual
      {.type = bmp_file, .identifier = {0xAA, 0xBE, 0xF1, 0x58, 0x50}, .address = 0}, //slide_portfolio_tamogatchi_perf
      {.type = bmp_file, .identifier = {0xBA, 0xC0, 0x09, 0x82, 0x80}, .address = 0}, //slide_portfolio_other_candy_main
      {.type = bmp_file, .identifier = {0x04, 0x05, 0x50, 0x56, 0x44}, .address = 0}, //slide_portfolio_other_candy_minor
      {.type = bmp_file, .identifier = {0x00, 0x07, 0x8B, 0xEF, 0xA0}, .address = 0}, //slide_portfolio_other_workshop
      {.type = bmp_file, .identifier = {0x51, 0x56, 0x16, 0x51, 0x21}, .address = 0}, //slide_portfolio_other_UA741
      {.type = bmp_file, .identifier = {0x65, 0x1F, 0xCA, 0x58, 0x00}, .address = 0}, //slide_portfolio_other_recorder

      /*** App Portfolio Audio ***/
      {.type = wav_file, .identifier = {0x63, 0x49, 0x53, 0x83, 0x63}, .address = 0}, //portfolio_fobo1_audio
      {.type = wav_file, .identifier = {0x42, 0x41, 0x48, 0x50, 0x47}, .address = 0}, //portfolio_fobo2_audio
      {.type = wav_file, .identifier = {0x37, 0x47, 0x57, 0x69, 0x96}, .address = 0}, //portfolio_fobo3_audio
      {.type = wav_file, .identifier = {0x71, 0x70, 0x84, 0x78, 0x99}, .address = 0}, //portfolio_fobo4_audio
      {.type = wav_file, .identifier = {0x39, 0x38, 0x75, 0x80, 0x35}, .address = 0}, //portfolio_gameboard1_audio
      {.type = wav_file, .identifier = {0x45, 0x54, 0x64, 0x46, 0x97}, .address = 0}, //portfolio_gameboard2_audio
      {.type = wav_file, .identifier = {0x55, 0x53, 0x72, 0x32, 0x23}, .address = 0}, //portfolio_gameboard3_audio
      {.type = wav_file, .identifier = {0x82, 0x28, 0xA6, 0x4F, 0x76}, .address = 0}, //portfolio_gameboard4_audio
      {.type = wav_file, .identifier = {0x67, 0x54, 0x25, 0x9B, 0x4B}, .address = 0}, //portfolio_gameboard5_audio
      {.type = wav_file, .identifier = {0x76, 0x72, 0x65, 0x89, 0x91}, .address = 0}, //portfolio_data1_audio
      {.type = wav_file, .identifier = {0x72, 0x66, 0x59, 0x45, 0x77}, .address = 0}, //portfolio_data2_audio
      {.type = wav_file, .identifier = {0x88, 0x78, 0x56, 0x6E, 0x6F}, .address = 0}, //portfolio_data3_audio
      {.type = wav_file, .identifier = {0x55, 0x80, 0x90, 0x91, 0x85}, .address = 0}, //portfolio_data4_audio
      {.type = wav_file, .identifier = {0x69, 0x77, 0x88, 0x99, 0x40}, .address = 0}, //portfolio_data5_audio
      {.type = wav_file, .identifier = {0x46, 0x49, 0x88, 0x59, 0x95}, .address = 0}, //portfolio_data6_audio
      {.type = wav_file, .identifier = {0x7D, 0x80, 0x7B, 0x76, 0x79}, .address = 0}, //portfolio_candymain_audio
      {.type = wav_file, .identifier = {0x80, 0x7B, 0x76, 0x81, 0x7D}, .address = 0}, //portfolio_candy3D_audio
      {.type = wav_file, .identifier = {0x81, 0x75, 0x7B, 0x72, 0x73}, .address = 0}, //portfolio_workshop_audio
      {.type = wav_file, .identifier = {0x72, 0x70, 0x82, 0x70, 0x73}, .address = 0}, //portfolio_ua741_audio
      {.type = wav_file, .identifier = {0x71, 0x72, 0x78, 0x74, 0x84}, .address = 0}, //portfolio_voice_audio

      /*** App Languages ****/
      {.type = bmp_file, .identifier = {0xEC, 0xB3, 0x51, 0x65, 0x50}, .address = 0}, //languages_main_screen
      {.type = wav_file, .identifier = {0x01, 0x28, 0x15, 0x72, 0x02}, .address = 0}, //german_audio
      {.type = wav_file, .identifier = {0x01, 0x28, 0x15, 0x72, 0x03}, .address = 0}, //spanish_audio
      {.type = bmp_file, .identifier = {0x01, 0x28, 0x15, 0x72, 0x04}, .address = 0}, //german_image
      {.type = bmp_file, .identifier = {0x01, 0x28, 0x15, 0x72, 0x05}, .address = 0}, //spanish_image

      /*** App About Me ***/
      {.type = bmp_file, .identifier = {0x58, 0x45, 0x56, 0x45, 0x22}, .address = 0}, //about_me_main_menu
      {.type = bmp_file, .identifier = {0x68, 0x55, 0x98, 0x45, 0x00}, .address = 0}, //about_me_main_education
      {.type = bmp_file, .identifier = {0xAD, 0x5A, 0x20, 0x10, 0x30}, .address = 0}, //about_me_main_goals
      {.type = bmp_file, .identifier = {0x50, 0x90, 0x19, 0x68, 0xB0}, .address = 0}, //about_me_main_hobbies
      {.type = bmp_file, .identifier = {0x90, 0x08, 0x55, 0x46, 0xBC}, .address = 0}, //about_me_main_interests
      {.type = bmp_file, .identifier = {0xBD, 0xEF, 0x25, 0x45, 0x50}, .address = 0}, //about_me_main_experience
      {.type = bmp_file, .identifier = {0x48, 0x51, 0x1B, 0x56, 0x56}, .address = 0}, //about_me_sub_education
      {.type = bmp_file, .identifier = {0x05, 0x78, 0xB0, 0x66, 0x30}, .address = 0}, //about_me_sub_goals
      {.type = bmp_file, .identifier = {0xCD, 0x0B, 0xE5, 0x98, 0x80}, .address = 0}, //about_me_sub_hobbies
      {.type = bmp_file, .identifier = {0x56, 0x89, 0x5A, 0x55, 0xD8}, .address = 0}, //about_me_sub_interests
      {.type = bmp_file, .identifier = {0x01, 0xA0, 0xB0, 0xD5, 0x4D}, .address = 0}, //about_me_sub_experience

      /*** App Device Images ***/
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x00}, .address = 0}, //slide_device_intro_drawing
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x01}, .address = 0}, //slide_device_intro_model
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x02}, .address = 0}, //slide_device_mechanical_prototype
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x03}, .address = 0}, //slide_device_mechanical_final
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x0E}, .address = 0}, //slide_device_mechanical_processing
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x04}, .address = 0}, //slide_device_hardware_schematic
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x05}, .address = 0}, //slide_device_hardware_layout
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x06}, .address = 0}, //slide_device_hardware_PCB
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x07}, .address = 0}, //slide_device_hardware_solder
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x08}, .address = 0}, //slide_device_hardware_firmware
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x09}, .address = 0}, //slide_device_hardware_jig
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x0A}, .address = 0}, //slide_device_product_photo
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x0B}, .address = 0}, //slide_device_product_box
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x0C}, .address = 0}, //slide_device_product_foam
      {.type = bmp_file, .identifier = {0x85, 0x71, 0x52, 0x33, 0x0D}, .address = 0}, //slide_device_product_manual

      /*** App Device Audio ***/
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x00}, .address = 0}, //device_start1_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x01}, .address = 0}, //device_start2_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x02}, .address = 0}, //device_mechanical1_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x03}, .address = 0}, //device_mechanical2_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x04}, .address = 0}, //device_mechanical3_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x05}, .address = 0}, //device_hwfw1_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x06}, .address = 0}, //device_hwfw2_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x07}, .address = 0}, //device_hwfw3_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x08}, .address = 0}, //device_hwfw4_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x09}, .address = 0}, //device_hwfw5_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x0A}, .address = 0}, //device_hwfw6_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x0B}, .address = 0}, //device_product1_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x0C}, .address = 0}, //device_product2_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x0D}, .address = 0}, //device_product3_audio
      {.type = wav_file, .identifier = {0x66, 0x25, 0x91, 0x88, 0x0E}, .address = 0}, //device_product4_audio

      /*** App Intro ***/
      {.type = wav_file, .identifier = {0x01, 0x28, 0x15, 0x72, 0x01}, .address = 0}, //intro_audio


};


/*
****************************************************
********** Private Function Prototypes *************
****************************************************
*/
void sd_send_command(uint8_t command, uint32_t data);
uint8_t sd_receive_r1_response(void);
void sd_receive_r3_response(uint8_t *r3_response);
void sd_receive_r7_response(uint8_t *r7_response);
void sd_send_app_command(void);
uint8_t sd_idle_sequence(void);
uint8_t sd_interface_condition_sequence(void);
uint8_t sd_read_ocr(void);
uint8_t sd_send_operating_condition(void);
void sd_cmd24_termination_sequence(void);
void sd_import_file_addresses(void);
uint32_t sd_parse_wav_header(uint32_t tmp_address);


/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/


/*!
* @brief Initializes the MCU's gpio, defines transfer settings and does the initial handshake with the SD card
* @param[in] NONE
* @return  init_success if successful this will be 1, otherwise 0;
*
* @note SPI2_CS is not hardware controlled and must be managed in software
*
*/
uint8_t
microsd_init(void)
{
   
   //Make it easy to identify the start of init on the PC serial terminal
   uart1_printf("\n\r -----------------SD CARD INITIALIZATION----------------- \n\r\0" );
   uart1_printf(" -------------------------------------------------------- \n\r\0" );
   uart1_printf(" -------------------------------------------------------- \n\r\0" );
   
   uint8_t init_success = 1;

   //Start contact with SD card and put it in idle state
   init_success = sd_idle_sequence();
   
   //Verify SD version greater than 2.0 and confirm voltage of 3.3v is acceptable
   init_success = sd_interface_condition_sequence();
   
   //Read OCR for power status and acceptable voltage
   sd_read_ocr();
   
   uint16_t timeout_count = 0;

   //Send command to begin SD card initialization
   //Repeat ACMD41 until card is out of idle or timeout is reached
   while(!(sd_send_operating_condition()))
   {
      timeout_count++;

      if(200 < timeout_count)
      {
         break;
      }
   }      

   //Check OCR to see if card is high capacity
   init_success = sd_read_ocr();
   
   //Read the file address lookup table and send them to their corresponding structures
   sd_import_file_addresses();

   //If there were no errors, this should still be 1
   return(init_success);
}


/*!
* @brief Read a single block (512bytes for SDHC) from the SD card
* @param[in] p_read_buffer Buffer used to store the block.
* @param[in] block_address Address of desired block. 
* @return  NONE 
* @warning Buffer must be larger than 512 bytes or this function will write to memory out of bounds
* @warning block_address is the address of the entire 512-byte block, not each individual byte address
*          For example, to read byte 520, you would set block_address = 1 (address starts at 0)
*
*/
void
sd_read_block(uint8_t *p_read_buffer, uint32_t block_address)
{
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_clear(SPI2_CS);
   spi_send_byte(0xFF);
   
   //Send CMD17, aka READ_SINGLE_BLOCK
   sd_send_command(17,block_address);
   spi_send_byte(SD_DUMMY_CRC);
   
   //Receive R1 response
   spi_receive_byte(0xFF); //dummy byte. SD card responds only after 8 clocks
   sd_receive_r1_response();
   sd_receive_r1_response();
   
   uint16_t response_timeout = 0;

   //Wait for the correct token 0xFE
   while((spi_receive_byte(0xFF) != SD_CMD17_TOKEN)) 
   {
      response_timeout++;

   }

   //Receive all 512 bytes
   for(uint16_t current_byte = 0; current_byte < 512; current_byte++)
   {
      p_read_buffer[current_byte] = spi_receive_byte(0xFF);
   }
   
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_set(SPI2_CS);
   spi_send_byte(0xFF);
}



/*!
* @brief Write a single block (512bytes for SDHC) to the SD card
* @param[in] tmp_write_buffer Data that will be written to the sd card
* @param[in] block_address Address of desired block.
* @return  NONE
*/
void
sd_write_block(uint8_t *tmp_write_buffer, uint32_t block_address)
{

   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_clear(SPI2_CS);
   spi_send_byte(0xFF);

   //Send CMD24, aka WRITE_SINGLE_BLOCK
   sd_send_command(24,block_address);
   spi_send_byte(SD_DUMMY_CRC);

   //Receive R1 response
   spi_receive_byte(0xFF); //dummy byte. SD card responds only after 8 clocks
   sd_receive_r1_response();
   sd_receive_r1_response();

   //Send start block token
   spi_send_byte(SD_CMD17_TOKEN); //CMD17 token is the same as CMD24 token

   //Send all 512 bytes
   for(uint16_t current_byte = 0; current_byte < 512; current_byte++)
   {
      spi_send_byte(tmp_write_buffer[current_byte]);
   }

   sd_cmd24_termination_sequence();

   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_set(SPI2_CS);
   spi_send_byte(0xFF);
}

/*!
* @brief Read a single block (512bytes for SDHC) from the SD card and send it over uart
* @param[in] block_address Address of desired block. 
* @return  NONE 
*
* @warning block_address is the address of the entire 512-byte block, not each individual byte address
*          For example, to read byte 520, you would set block_address = 1 (address starts at 0)
*
*/
void
sd_print_block(uint64_t block_address)
{
   uint16_t response_timeout = 0;
   uint16_t current_byte = 0;
   
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_clear(SPI2_CS);
   spi_send_byte(0xFF);
   
   //Send CMD17, aka READ_SINGLE_BLOCK
   sd_send_command(17,block_address);
   spi_send_byte(SD_DUMMY_CRC);
   
   //Receive R1 response
   spi_receive_byte(0xFF); //dummy byte. SD card responds only after 8 clocks
   sd_receive_r1_response();
   sd_receive_r1_response();
   
   //Wait for the correct token 0xFE
   while((spi_receive_byte(0xFF) != SD_CMD17_TOKEN)) 
   {
      if(6000 < response_timeout)
      {
         uart1_printf("Timeout error attempting to print block");
         break;
      }
      
      response_timeout++;
   }
   
   //Receive all 512 bytes
   for(current_byte = 0; current_byte < 512; current_byte++)
   {
      uart1_send_byte(spi_receive_byte(0xFF));
      
   }
   
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_set(SPI2_CS);
   spi_send_byte(0xFF);
}


/*!
* @brief Tell the SD card to continue sending data until told to stop
* @param[in] start_address 
* @return  NONE 
*
* @warning start_address is the address of the entire 512-byte block, not each individual byte address
*          For example, to read byte 520, you would set block_address = 1 (address starts at 0)
*
*/
void
sd_read_multiple_block(uint32_t start_address)
{
   uint16_t response_timeout = 0;
   
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_clear(SPI2_CS);
   spi_send_byte(0xFF);
   
   //Send CMD18, aka READ_MULTIPLE_BLOCK
   sd_send_command(18, start_address);
   spi_send_byte(SD_DUMMY_CRC);
   
   //Receive R1 response
   spi_receive_byte(0xFF); //dummy byte. SD card responds only after 8 clocks
   sd_receive_r1_response();
   sd_receive_r1_response();
   
   //Wait for the correct token 0xFE
   while((spi_receive_byte(0xFF) != SD_CMD17_TOKEN)) //CMD17 token is the same for CMD18
   {
      response_timeout++;

      if(response_timeout > 6000)
      {
         break;
      }
   }
}


/*!
* @brief Finds the address of a particular file within the sd card
* @param[in] file_identifier List of 5 bytes in the first block of the file used to
*                            locate it in the SD card
* @return  file_address 
*
* @note Address is represented by each 512-byte block
*
*/
uint32_t
sd_find_file_address(const uint8_t *file_identifier)
{
   uint8_t success = 0; 
   uint32_t file_address = 0;
   
   for(uint64_t current_block = 15000; current_block < 4294967294; current_block++) //Largest uint32_t: 4,294,967,294
   {
      uint8_t buffer[512] = {0};
      //Read the entire next block (512 bytes)
      sd_read_block(buffer, current_block);
      
      //Parse buffer
      for(uint16_t current_byte = 0; current_byte <512; current_byte++)
      {
         //Check the current 5-byte window against the image identifier
         if(buffer[current_byte] == file_identifier[0])
         {
            if((buffer[current_byte + 1] == file_identifier[1]) &&
               (buffer[current_byte + 2] == file_identifier[2]) &&
               (buffer[current_byte + 3] == file_identifier[3]) &&
               (buffer[current_byte + 4] == file_identifier[4]))
            {
               file_address = current_block;
               success = 1;
            }
         }
      }
      
      //If the file is found before the entire card is read, exit early to decrease latency
      if(success)
      {
         uart1_printf("File Address Found \n \r");
         break;         
      }
 
   }

   return(file_address);
}


/*!
* @brief This does a brute force procedural search of the SD card looking for file identifiers.
*        It then copies the start addresses of each file to a lookup table in the SD card itself
*        at a memory block very far outside of the scope that the FAT32 file system should ever
*        manipulate for this system.
* @param[in] NONE
* @return  NONE
* @note This is very slow yet more maintainable. The function only ever runs once before flashing the main
*       binary just to put the addresses in the sd card. It doesn't even get included in the main binary,
*       so speed is not a great concern.
*
*/
void
sd_search_file_addresses(void)
{
   uint16_t total_files = max_total_addresses;

   lcd_draw_rectangle(0x00, 0, 0, 320, 480);

   //Do a brute force search to find each file starting address
   for(uint8_t current_file = 0; current_file < total_files; current_file++)
   {
      char tmp_string[10] = {0};

      //Send file information to PC console
      pft_uint32_to_string(current_file, tmp_string);
      uart1_printf("Current file number:  ");
      uart1_printf(tmp_string);
      uart1_printf(" \n \r");

      //Display file information on LCD
      lcd_print_string("Current file number: ", 50, 100, 0x03E0, 0x0000);
      lcd_print_string(tmp_string, 50, 120, 0x03E0, 0x0000);

      //Search for current file address
      file_list[current_file].address = sd_find_file_address(file_list[current_file].identifier);

   }

   uint8_t address_buffer[1024] = {0}; //Buffer to be written to the SD card file address cheat sheet block

   total_files = (max_total_addresses)*4; // Four bytes per uint32

   //Split each uint32 address into four bytes and store them in the array
   for(uint8_t current_file = 0; current_file < max_total_addresses; current_file ++)
   {
      address_buffer[(current_file*4) + SD_ADDRESS_LIST_OFFSET] = (file_list[current_file].address >> 24);
      address_buffer[(current_file*4) + SD_ADDRESS_LIST_OFFSET + 1] = (file_list[current_file].address >> 16);
      address_buffer[(current_file*4) + SD_ADDRESS_LIST_OFFSET + 2] = (file_list[current_file].address >> 8);
      address_buffer[(current_file*4) + SD_ADDRESS_LIST_OFFSET + 3] = (file_list[current_file].address & 0xFF);
   }

   //Transfer the file addresses to the cheat sheet inside the SD Card
   sd_write_block(address_buffer, SD_ADDRESS_CHEAT_SHEET);\
   sd_write_block((address_buffer + 511), (SD_ADDRESS_CHEAT_SHEET + 1));
   uart1_printf("\n\r File search complete \n \r");
   lcd_print_string("File search complete",  100, 50, 0x03E0, 0x0000);
}


/*!
* @brief This does a brute force procedural search of the SD card looking for file identifiers.
*        It then copies the start addresses of each file and appends them to the end of the
*        lookup table in the SD card itself, starting at the input file number
*        at a memory block very far outside of the scope that the FAT32 file system should ever
*        manipulate for this system.
* @param[in] start_address The address of the first file you wish to append in the SD card's lookup table
* @param[in] stop_address The address of the last file you wish to append in the SD card's lookup table
* @return  NONE
*
* @note This is very slow yet more maintainable. The function only ever runs once before flashing the main
*       binary just to put the addresses in the sd card. It doesn't even get included in the main binary,
*       so speed is not a great concern.
*
*/
void
sd_append_file_addresses(e_sd_address start_address, e_sd_address stop_address)
{
   lcd_draw_rectangle(0x00, 0, 0, 320, 480);

   //Do a brute force search to find each file starting address
   for(e_sd_address current_file = start_address; current_file < stop_address; current_file++)
   {
      char tmp_string[10] = {0};
      pft_uint32_to_string(current_file, tmp_string);
      uart1_printf("Current file number:  ");
      uart1_printf(tmp_string);
      uart1_printf(" \n \r");

      lcd_print_string("Current file number: ", 50, 100, 0x03E0, 0x0000);
      lcd_print_string(tmp_string, 50, 120, 0x03E0, 0x0000);

      //Search for current file address
      file_list[current_file].address = sd_find_file_address(file_list[current_file].identifier);

   }

   uint8_t address_buffer[1024] = {0}; //Buffer to be written to the SD card file address cheat sheet block
   sd_read_block(address_buffer, SD_ADDRESS_CHEAT_SHEET); //Fill buffer with current addresses
   sd_read_block((address_buffer + 511), (SD_ADDRESS_CHEAT_SHEET + 1));

   //Split each uint32 address into four bytes and append to the end of the array
   for(uint8_t current_file = start_address; current_file < stop_address; current_file ++)
   {
      address_buffer[(current_file*4) + SD_ADDRESS_LIST_OFFSET] = (file_list[current_file].address >> 24);
      address_buffer[(current_file*4) + SD_ADDRESS_LIST_OFFSET + 1] = (file_list[current_file].address >> 16);
      address_buffer[(current_file*4) + SD_ADDRESS_LIST_OFFSET + 2] = (file_list[current_file].address >> 8);
      address_buffer[(current_file*4) + SD_ADDRESS_LIST_OFFSET + 3] = (file_list[current_file].address & 0xFF);
   }

   sd_write_block(address_buffer, SD_ADDRESS_CHEAT_SHEET);
   sd_write_block((address_buffer + 511), (SD_ADDRESS_CHEAT_SHEET + 1));
   uart1_printf("\n\r File search complete \n \r");
   lcd_print_string("File search complete",  100, 50, 0x03E0, 0x0000);
}



/*!
* @brief This reads each file from its corresponding t_sd_file structure
*        and stores the addresses in an input buffer.
* @param[in] NONE
* @return  NONE
* @note This function will only work after sd_import_file_addresses() has been called.
*       Otherwise all addresses will be returned as their init values, "0"
*
*/
void
sd_get_file_addresses(uint32_t *tmp_file_list)
{
   for(e_sd_address current_file = 0; current_file < max_total_addresses; current_file++)
   {
      tmp_file_list[current_file] = file_list[current_file].address;
   }

}


/*!
* @brief Return the file size of the current file in blocks
* @param[in] tmp_file File number as defined in enum_sd_address_list.h
* @return  file_size Size of the desired WAV file
*
* @warning Only WAV files are initialized with a size, all other types will
*          return random, uninitialized values
*/
uint32_t
sd_get_wav_size(uint8_t tmp_file)
{
   return(file_list[tmp_file].size);
}

/*
****************************************************
********** Private Function Definitions ************
****************************************************
*/

/*!
* @brief Sends 5-byte command/data to the SD card
* @param[in] command 6-bit SD card command
* @param[in] data 4-bytes of information corresponding to the command
* @return  NONE
*
*/
void 
sd_send_command(uint8_t command, uint32_t data)
{
   //Send command, transmission bit must always be set for commands
   spi_send_byte(command | SD_TRANSMISSION_BIT);
   
   //Send data, one byte at a time. Needs a type cast, since SPI_DR is 8 bit
   spi_send_byte((uint8_t)(data >> 24)); //Most significant byte first
   spi_send_byte((uint8_t)(data >> 16));
   spi_send_byte((uint8_t)(data >> 8));
   spi_send_byte((uint8_t)(data)); //Least significant byte
}


/*!
* @brief Recieves R1 response from SD card
* @param[in] NONE
* @return  r1_response Byte containing r1 response error information
*
*/
uint8_t
sd_receive_r1_response(void)
{
   uint8_t tmp_r1_response = 0;
   
   tmp_r1_response = spi_receive_byte(0xFF);
   
   return((uint8_t)(tmp_r1_response));
}
   

  

/*!
* @brief Receives R3 response from SD card
* @param[in] NONE
* @return  NONE
* @note This response returns SD card operating voltages, capacity and power up state
*
*/
void
sd_receive_r3_response(uint8_t *r3_response)
{
   //Receive all 5 bytes
   r3_response[0] = spi_receive_byte(0xFF); //First byte is identical to R1 response
   r3_response[1] = spi_receive_byte(0xFF);
   r3_response[2] = spi_receive_byte(0xFF); //Voltage Byte
   r3_response[3] = spi_receive_byte(0xFF);
   r3_response[4] = spi_receive_byte(0xFF);
}
   

/*!
* @brief Receives R7 response from SD card
* @param[in] NONE
* @return  r7_response Byte containing r1 response error information
*
*/
void
sd_receive_r7_response(uint8_t *r7_response)
{ 
   //Receive all 5 bytes
   r7_response[0] = spi_receive_byte(0xFF); //First byte is identical to R1 response
   r7_response[1] = spi_receive_byte(0xFF); 
   r7_response[2] = spi_receive_byte(0xFF); 
   r7_response[3] = ((spi_receive_byte(0xFF)) & 0x0F); //Voltage Byte. Warning, includes lower nibble mask
   r7_response[4] = spi_receive_byte(0xFF); //Echo Byte
}
   

/*!
* @brief Puts SD card into idle state
* @param[in] NONE
* @return  command0_success Flag for command0 error, 0 = error,
*                           1 = valid response and SD is in idle state
*/
uint8_t
sd_idle_sequence(void)
{
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_clear(SPI2_CS);
   spi_send_byte(0xFF);
   
   //set clock to below 400kHz
   //Clock divisor,0x00 = fclk/2, 0x01 = fclk/4, 0x03 = fclk/16, 0x07 = fclk/256 = 200kHz
   spi_set_clk_low_speed();
   
   //Disable CS
   gpio_set(SPI2_CS);
   
   //Delay at least 1ms
   timers_delay(2);
   
   //Send at least 74 clock pulses i.e 10 bytes
   for(uint8_t current_byte = 0; current_byte < 11; current_byte++)
   {
      spi_receive_byte(0xFF);
   }
   
   //Enable CS
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_clear(SPI2_CS);
   spi_send_byte(0xFF);
   
   //Send CMD0, data = 0, CRC = 0x95
   sd_send_command(0, 0); //Data is only dummy bytes, but will change crc
   spi_send_byte(0x95); 
   
   //Recieve and parse R1 response from sd card
   spi_send_byte(0xFF); //dummy byte to give 8 clocks
   
   uint8_t r1_response = sd_receive_r1_response(); // This must be done twice to clear the FIFO buffer
   r1_response = sd_receive_r1_response();
   
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_set(SPI2_CS);
   spi_send_byte(0xFF);
   
   uint8_t command0_success = 1;

   //Check for error
   if(r1_response == 0x01)
   {
      uart1_printf(" CMD0 accepted, SD card in idle state \n \r\0");
   }
   
   else
      {
         uart1_printf("Error in R1 response to CMD0 \n \r R1 = \0");
         uart1_send_byte(r1_response);
         command0_success = 0;
      }
   
   return(command0_success);
}


/*!
* @brief Checks if SD is 2.0 and sets voltage to 3.3v
* @param[in] NONE
* @return  command8_success Flag for command0 error, 0 = error,
*                           1 = valid response and SD is in idle state
*/
uint8_t
sd_interface_condition_sequence(void)
{
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_clear(SPI2_CS);
   spi_send_byte(0xFF);

   //Send CMD8, CRC = 0x95
   sd_send_command(8, SD_CMD8_DATA);
   spi_send_byte(SD_CMD8_CRC);
      
   //Receive and parse R7 response from sd card
   spi_receive_byte(0xFF); //dummy byte. SD card responds only after 8 clocks
   spi_receive_byte(0xFF); //dummy byte to clear spi FIFO buffer to
   uint8_t r7_response[5] = {0};
   sd_receive_r7_response(r7_response); 
   
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_set(SPI2_CS);
   spi_send_byte(0xFF);
   
   uint8_t command8_success = 1;
   //Check for error in r1, acceptable voltage or echo byte
   if((r7_response[0] == 0x01) && (r7_response[3] == (SD_CMD8_VHS_3v3 >> 8)) && (r7_response[4] == SD_CMD8_CHECK_PATTERN))
   {
      uart1_printf("CMD8 accepted, SD card version 2.0 or greater, 3.3v \n \r\0");
   }
   
   else
      {
         uart1_printf("Error in R7 response to CMD8 \n \r\0");
         command8_success = 0;
      }
   
   return(command8_success);
}
   
/*!
* @brief Checks if SD card is powered on, voltage is acceptable and 
* @param[in] NONE
* @return command58_success
*                           
*/
uint8_t
sd_read_ocr(void)
{
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_clear(SPI2_CS);
   spi_send_byte(0xFF);

   //Send CMD58, data = 0, CRC = dummy
   sd_send_command(58, 0);
   spi_send_byte(SD_DUMMY_CRC);

   //Receive and parse R3 response from sd card
   spi_receive_byte(0xFF); //dummy byte. SD card responds only after 8 clocks
   spi_receive_byte(0xFF); //dummy byte to clear spi FIFO buffer to
   uint8_t r3_response[5] = {0};
   sd_receive_r3_response(r3_response); 

   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_set(SPI2_CS);
   spi_send_byte(0xFF);
   
   uint8_t command58_success = 1;

   //Check for error in r3_response
   if((r3_response[0] == 0x00) &&  //Check r1 response good
      ((r3_response[1] & (SD_RESPONSE3_POWER_UP | SD_RESPONSE3_CCS)) >> 6) && //Check power busy and CCS
      (r3_response[2] & SD_RESPONSE3_3V2_3V4)) //check acceptable voltage
   {
      uart1_printf("CMD58 accepted, power ready, voltage range 3.2v - 3.4v acceptable \n \r\0");
   }
   
   else
      {
         uart1_printf("Error in R3 response to CMD58 \n \r\0");
         command58_success = 0;
      }
      
   return(command58_success);
}


/*!
* @brief Tell the SD card that the next command will be an application specific command
* @param[in] NONE
* @return
*/
void
sd_send_app_command(void)
{
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_clear(SPI2_CS);
   spi_send_byte(0xFF);
   
   //Send CMD55, data = 0, CRC = dummy
   sd_send_command(55, 0);
   spi_send_byte(SD_DUMMY_CRC);
   
   //Read response
   sd_receive_r1_response();

   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_set(SPI2_CS);
   spi_send_byte(0xFF);
}


/*!
* @brief Activates the SD card initialization process
* @param[in] NONE
* @return  command_success Flag for command0 error, 0 = error,
*                           1 = valid response 
*/
uint8_t
sd_send_operating_condition(void)
{
   //Tell SD card to expect an application command
   sd_send_app_command();
   spi_send_byte(0xFF);

   //Send ACMD41, data = 0, CRC = dummy
   gpio_clear(SPI2_CS);
   sd_send_command(41, SD_ACMD41_DATA);
   spi_send_byte(0x77); //crc
   
   //Receive R1 response
   spi_receive_byte(0xFF); //dummy byte. SD card responds only after 8 clocks
   uint8_t r1_response = 0;
   r1_response = sd_receive_r1_response();
   r1_response = sd_receive_r1_response();

   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_set(SPI2_CS);
   spi_send_byte(0xFF);
   
   uint8_t command41_success = 1;
   if(r1_response == 0x00)
   {
      uart1_printf("ACMD41 accepted, card is initialized and out of idle state \n \r\0");
   }
   
   else
   {
      command41_success = 0;
      uart1_printf("ACMD41 command failed \n \r\0");
   }

   return(command41_success);
   
}


/*!
* @brief Tell the SD card to stop transmitting data, aka stop MULTIPLE_BLOCK_READ
* @param[in] current_byte How many bytes have already been read from the current block
* @return  NONE
*/
void
sd_stop_transmission(void)
{
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_clear(SPI2_CS);
   spi_send_byte(0xFF);
   
   //Send CMD12 akak STOP_TRANSMISSION, data = 0, CRC = dummy
   sd_send_command(12, 0);
   spi_send_byte(SD_DUMMY_CRC);
   
   //Flush an entire sd card block to make sure it is done transmitting
   for(uint16_t current_byte = 0; current_byte < 512; current_byte++)
   {
      spi_receive_byte(0xFF);
   }
   
   //Wrap CS transition in dummy bytes to make sure SD card acknowledges it
   spi_send_byte(0xFF);
   gpio_set(SPI2_CS);
   spi_send_byte(0xFF);
}


/*!
* @brief Wait for SD to return correct responses after writing a single block
* @param[in] NONE
* @return  NONE
*/
void
sd_cmd24_termination_sequence(void)
{
   //wait for Data accepted token
   while((spi_receive_byte(0xFF) & 0x1F) != 0x05)
   {
      uint16_t timeout = 0;
      timeout++;

      if(7000 < timeout)
      {
         uart1_printf("Timeout error writing to SD\0");
         break;
      }
   }

   //SD will return 0 until it finishes writing all data
   while(spi_receive_byte(0xFF) == 0)
   {
      uint16_t timeout = 0;
      if(7000 < timeout)
      {
         uart1_printf("Timeout error writing to SD\0");
         break;
      }
      timeout++;
   }
}


/*!
* @brief Read the header, contained in the first block, of a WAV file and
*        return its file size in blocks
* @param[in] tmp_address Address of the WAV file to be parsed
* @return  tmp_file_size In blocks
*
*/
uint32_t
sd_parse_wav_header(uint32_t tmp_address)
{
   //Start reading the WAV file from the SD card
   sd_read_multiple_block(tmp_address);

   //Jump to file size section of header.
   spi_receive_byte(0xFF);
   spi_receive_byte(0xFF);
   spi_receive_byte(0xFF);
   spi_receive_byte(0xFF);

   uint32_t file_size = 0; //Size of WAV in blocks

   //Read file size from header. Warning, file size is little endian in WAV header
   file_size = (uint32_t)(spi_receive_byte(0xFF));
   file_size |= (((uint32_t)(spi_receive_byte(0xFF))) << 8);
   file_size |= (((uint32_t)(spi_receive_byte(0xFF))) << 16);
   file_size |= (((uint32_t)(spi_receive_byte(0xFF))) << 24);

   //Stop the transmission, since we already have the file size info
   sd_stop_transmission();

   //512 bytes-per-block
   return(file_size / 512);
}


/*!
* @brief This reads the file address lookup table from the SD card generated by
*        sd_search_file_addresses() and stores each address in  its corresponding t_sd_file structure
* @param[in] NONE
* @return  NONE
*
* NOTE This should be called upon system startup after SPI and the SD card have been initialized
*/
void
sd_import_file_addresses(void)
{
   uint8_t tmp_read_buffer[1024] = {0};

   //Read the sd card address lookup table and store it in a buffer
   sd_read_block(tmp_read_buffer, SD_ADDRESS_CHEAT_SHEET);
   sd_read_block((tmp_read_buffer + 511), (SD_ADDRESS_CHEAT_SHEET + 1));

   //This is slower yet more maintainable. The function only runs once upon startup, so speed is not a great concern.
   for(uint8_t current_file = 0; current_file < max_total_addresses; current_file ++)
   {
      uint32_t tmp_address = 0;
      tmp_address = 0;

      for(uint8_t current_byte = 0; current_byte < 4; current_byte++)
      {
        tmp_address |= (tmp_read_buffer[(current_file*4) + SD_ADDRESS_LIST_OFFSET + current_byte] << (24 - (8*current_byte)));
      }

      file_list[current_file].address = tmp_address;

      //If current file is a WAV, parse the header for file size
      if(wav_file == file_list[current_file].type)
      {
         uint32_t file_size = sd_parse_wav_header(file_list[current_file].address);
         file_list[current_file].size = file_size;
      }
   }
}

#pragma GCC pop_options
/* end of file */
