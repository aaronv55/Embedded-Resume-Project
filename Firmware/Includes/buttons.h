/** @file buttons.h
*
* @brief  This file contains functions to create, interact with and handle virtual buttons
*         and button events on the LCD. It does not cover external buttons i.e switches on the PCB.
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
*
*/

#ifndef BUTTONS_H
#define BUTTONS_H

#define NO_BUTTON_PRESS 255 //Returned value if no virtual button was pressed

#include "lcd.h"
#include "touch.h"
#include "struct_buttons.h"

/*
****************************************************
******* Public Functions Defined in buttons.c ******
****************************************************
*/
void buttons_create(t_button buttons[], char **button_text, size_t list_length);
uint8_t buttons_status(t_button buttons[], size_t list_length);

#endif /* BUTTONS_H */

/* end of file */
