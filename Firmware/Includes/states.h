/** @file states.h
*
* @brief This file contains the logic for the system's  main state machine.
*        It defines the various menu attributes and how the system transitions
*        from one state to another.
* @author Aaron Vorse
* @date   8/18/2020
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

#ifndef STATES_H
#define STATES_H

#define TOTAL_APPS 9
#define CONTEXT_PUSH 0
#define CONTEXT_POP 1

#define STATES_ADDRESS_STARTUP_FLAG 4005000
#define STATES_STARTUP_FLAG_SET 1
#define STATES_STARTUP_FLAG_CLEAR 0

#include "enum_sd_file_list.h"
#include "struct_buttons.h"
#include "microsd.h"
#include "lcd.h"
#include "dac.h"
#include "gui.h"
#include "struct_gui_person_profile.h"
#include "rtc.h"

/*
****************************************************
******* Public Function Defined in states.c ********
****************************************************
*/
void states_init(void);
void states_update_main_event(void);
void states_update_main_state(void);
void states_update_footer(void);
uint32_t states_get_current_audio(void);
void states_set_current_audio(uint32_t audio_address);
void states_update_audio(void);

#endif /* STATES_H */
