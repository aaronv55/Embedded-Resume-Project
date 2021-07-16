/** @file personal_function_toolbox.h
*
* @brief  This file constains random yet useful little helper functions that
*          are usually carried around with me from project to project
* @author Aaron Vorse
* @date   7/20/2020
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

#ifndef PERSONAL_FUNCTION_TOOLBOX_H
#define PERSONAL_FUNCTION_TOOLBOX_H


#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f410rx.h"


/* Public funtions defined in microsd.c */
void pft_uint32_to_string(uint32_t int_to_convert, char *p_tmp_string);


#endif /* PERSONAL_FUNCTION_TOOLBOX_H */

/* end of file */
