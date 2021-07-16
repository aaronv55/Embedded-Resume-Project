
/** @file struct_buttons.h
*
* @brief  This contains the common structure for virtual buttons, which is meant to
*         be accessed by both the button middleware and the system state machine
* @author Aaron Vorse
* @date   8/19/2020
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

#ifndef STRUCT_BUTTONS_H
#define STRUCT_BUTTONS_H

#include <stdint.h>

/*
****************************************************
***** Private Types and Structure Definitions ******
****************************************************
*/
typedef enum e_button_type_tag
{
   button_rectangle,
   button_rounded_corners,
   button_ghost

} e_button_type;


typedef struct t_button_tag
{
   uint16_t width; //In pixels
   uint16_t height;
   uint16_t x_position;
   uint16_t y_position;
   uint16_t background_color;
   uint16_t body_color;
   uint16_t text_color;
   e_button_type type;
   const uint8_t *bmp;

} t_button;


typedef struct t_light_button_tag
{
   uint32_t image[2];
   const uint8_t *bmp;
   char *major_text;
   char *minor_text;

} t_light_button;


typedef struct t_extra_light_button_tag
{
   uint16_t x, y;
   uint8_t *bmp;

} t_extra_light_button;

#endif /* STRUCT_BUTTONS_H */

/* end of file */


