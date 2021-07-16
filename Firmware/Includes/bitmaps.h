/** @file bitmaps.h
*
* @brief  This file contains bitmaps for small icons
* @author Aaron Vorse
* @date   6/14/2020
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


#ifndef BITMAPS_H
#define BITMAPS_H

#define BITMAPS_LOCAL_BMP_OFFSET 4 //The number of bytes included in the header before reaching the actual bitmap


#include <stdint.h>
/*
****************************************************
******* Public Variables Defined in bitmaps.c ******
****************************************************
*/


/*************** Button Elements ******************/
const uint8_t button_corner_m_ul[40];
const uint8_t button_corner_m_ur[31];
const uint8_t button_corner_m_ll[40];
const uint8_t button_corner_m_lr[31];
const uint8_t button_ghost_ul_bmp[22];
const uint8_t button_ghost_ur_bmp[22];
const uint8_t button_ghost_ll_bmp[22];
const uint8_t button_ghost_lr_bmp[22];
const uint8_t button_toggle_left_bmp[70];
const uint8_t button_toggle_right_bmp[70];
const uint8_t button_toggle_middle_bmp[68];

/************** Main User Buttons *****************/
const uint8_t back_button_bmp[146];
const uint8_t home_button_bmp[220];
const uint8_t square_button_bmp[152];

/************** Status Bar Icons ******************/
const uint8_t speaker_muted[77];
const uint8_t speaker_low[77];
const uint8_t speaker_medium[77];
const uint8_t speaker_high[77];
const uint8_t battery_icon_bmp[64];
const uint8_t headphone_icon_bmp[66];
const uint8_t usb_icon_bmp[125];

/************* Menu Type 2 Elements ***************/
const uint8_t job_icon_bmp[304];
const uint8_t mail_icon_bmp[304];
const uint8_t phone_icon_bmp[304];

/************* Menu Type 3 Elements ***************/
const uint8_t play_button_bmp[212];
const uint8_t pause_button_bmp[212];
const uint8_t left_music_arrows_bmp[204];
const uint8_t right_music_arrows_bmp[204];
const uint8_t sliding_bar_marker_bmp[60];
const uint8_t pause_button_inner_symbol[20];
const uint8_t pause_button_inner_symbol[20];
const uint8_t repeat_music_icon_bmp[264];
const uint8_t play_button_inner_symbol[34];

/************ Portfolio App Icons ***************/
const uint8_t data_acquisition_icon_bmp[304];
const uint8_t FOBO_icon_bmp[304];
const uint8_t tamagotchi_icon_bmp[304];
const uint8_t additional_icon_bmp[304];
const uint8_t up_arrow_bmp[28];
const uint8_t down_arrow_bmp[28];

/*************** Miscellaneous ******************/
const uint8_t check_mark_bmp[104];
const uint8_t menu_arrow_bmp[36];

#endif /* BITMAPS_H */

/* end of file */
