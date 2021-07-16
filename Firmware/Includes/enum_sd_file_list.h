/** @file enum_sd_file_list.h
*
* @brief  This file contains an enum type for defining all files on the SD card which
*         are used by the system.
* @author Aaron Vorse
* @date 7/9/2020
* @contact aaron.vorse@embeddedresume.com
*
* @note This file works in conjunction with microsd.c, and the order of the SD card file list
*        must be identical in both
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


#ifndef ENUM_SD_FILE_LIST_H
#define ENUM_SD_FILE_LIST_H

typedef enum e_sd_address
{
   null_address,

   /*** App Company ***/
   company_audio,
   company_image,

   /*** Core System Files ***/
   menu_button_audio,
   homescreen_bmp,
   startup_animation_0,
   startup_animation_1,
   startup_animation_2,
   startup_animation_3,
   startup_animation_4,
   startup_animation_5,
   startup_animation_6,
   startup_animation_7,
   startup_animation_8,
   startup_animation_9,
   startup_animation_10,
   startup_animation_11,
   startup_animation_12,

   /*** App References ***/
   person1_large,
   person1_small_pressed,
   person1_small_not_pressed,
   person3_large,
   person3_small_pressed,
   person3_small_not_pressed,
   person2_large,
   person2_small_pressed,
   person2_small_not_pressed,
   person4_large,
   person4_small_pressed,
   person4_small_not_pressed,

   /*** App Contact ***/
   aaron_large,
   github_logo_light,

   /*** App Skills ***/
   github_logo,
   linkedin_logo,
   skills_arm,
   skills_circuit,
   skills_c,
   skills_equipment,
   skills_pcb,
   skills_solder,

   /*** App Portfolio Images ***/
   slide_portfolio_acq_adc,
   slide_portfolio_acq_bode,
   slide_portfolio_acq_breadboard,
   slide_portfolio_acq_gui,
   slide_portfolio_acq_filter,
   slide_portfolio_acq_memory,
   slide_portfolio_fobo_code,
   slide_portfolio_fobo_standing,
   slide_portfolio_fobo_fritzing,
   slide_portfolio_fobo_leg,
   slide_portfolio_tamogatchi_code,
   slide_portfolio_tamogatchi_case,
   slide_portfolio_tamogatchi_pcb3D,
   slide_portfolio_tamogatchi_pcbactual,
   slide_portfolio_tamogatchi_perf,
   slide_portfolio_other_candy_main,
   slide_portfolio_other_candy_minor,
   slide_portfolio_other_workshop,
   slide_portfolio_other_UA741,
   slide_portfolio_other_recorder,

   /*** App Portfolio Audio ***/
   portfolio_fobo1_audio,
   portfolio_fobo2_audio,
   portfolio_fobo3_audio,
   portfolio_fobo4_audio,
   portfolio_gameboard1_audio,
   portfolio_gameboard2_audio,
   portfolio_gameboard3_audio,
   portfolio_gameboard4_audio,
   portfolio_gameboard5_audio,
   portfolio_data1_audio,
   portfolio_data2_audio,
   portfolio_data3_audio,
   portfolio_data4_audio,
   portfolio_data5_audio,
   portfolio_data6_audio,
   portfolio_candymain_audio,
   portfolio_candy3D_audio,
   portfolio_workshop_audio,
   portfolio_ua741_audio,
   portfolio_voice_audio,

   /*** App Languages ****/
   languages_main_screen,
   german_audio,
   spanish_audio,
   german_image,
   spanish_image,

   /*** App About Me ***/
   about_me_main_menu,
   about_me_main_education,
   about_me_main_goals,
   about_me_main_hobbies,
   about_me_main_interests,
   about_me_main_experience,
   about_me_sub_education,
   about_me_sub_goals,
   about_me_sub_hobbies,
   about_me_sub_interests,
   about_me_sub_experience,

   /*** App Device Images ***/
   slide_device_intro_drawing,
   slide_device_intro_model,
   slide_device_mechanical_prototype,
   slide_device_mechanical_final,
   slide_device_mechanical_processing,
   slide_device_hardware_schematic,
   slide_device_hardware_layout,
   slide_device_hardware_PCB,
   slide_device_hardware_solder,
   slide_device_hardware_firmware,
   slide_device_hardware_jig,
   slide_device_product_photo,
   slide_device_product_box,
   slide_device_product_foam,
   slide_device_product_manual,

   /*** App Device Audio ***/
   device_start1_audio,
   device_start2_audio,
   device_mechanical1_audio,
   device_mechanical2_audio,
   device_mechanical3_audio,
   device_hwfw1_audio,
   device_hwfw2_audio,
   device_hwfw3_audio,
   device_hwfw4_audio,
   device_hwfw5_audio,
   device_hwfw6_audio,
   device_product1_audio,
   device_product2_audio,
   device_product3_audio,
   device_product4_audio,

   /*** App Intro ***/
   intro_audio,


   max_total_addresses

} e_sd_address;

#endif /* ENUM_SD_FILE_LIST */

/* end of file */
