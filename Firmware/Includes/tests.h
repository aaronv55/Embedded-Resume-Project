/** @file tests.h
*
* @brief  This file contains production tests for the Embedded Resume Device
* @author Aaron Vorse
* @date   2/05/2020
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

#ifndef TESTS_H

#define TESTS_H

#define TESTS_BATTERY_LOG_ADDRESS 4010000
#define TESTS_PRODUCTION_TEST_SENSE_PIN GPIOB, 10

#include "monitor.h"
#include "microsd.h"
#include "uart.h"
#include "personal_function_toolbox.h"
#include "gui.h"
#include "timers.h"
#include "buttons.h"
#include "dac.h"
#include "base_gpio_drivers.h"



/*
****************************************************
******* Public Functions Defined in dac.c **********
****************************************************
*/
void tests_init(void);
void tests_battery_drain_test(void);
void tests_battery_read_results(void);
void tests_production_hardware(void);


#endif /* TESTS_H */

/* end of file */
