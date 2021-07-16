/** @file personal_function_toolbox.c
*
* @brief  This file contains random yet useful little helper functions that
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

#include "personal_function_toolbox.h"

/*
****************************************************
********** Private Function Prototypes *************
****************************************************
*/

/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/

/*!
* @brief Convert a 32-bit unsigned integer to a string
* @param[in] tmp_string Char array to store value
* @return  NONE
* @warning This function is extremely slow. Do not include this in any fast loops
*
*/

void
pft_uint32_to_string(uint32_t int_to_convert, char *p_tmp_string)
{  
   //Divide each tens place, then add this to 48, the beginning of the 
   // ASCII numbers list. ASCII 48 = char 0....ASCII 57 = char 9
   p_tmp_string[0] = (48 + (int_to_convert/1000000000)); //Billions
   int_to_convert -= ((int_to_convert/1000000000) * 1000000000); //
   
   p_tmp_string[1] = (48 + (int_to_convert/100000000)); //Hundred-Millions
   int_to_convert -= ((int_to_convert/100000000) * 100000000);
   
   p_tmp_string[2] = (48 + (int_to_convert/10000000)); //Ten-Millions
   int_to_convert -= ((int_to_convert/10000000) * 10000000);
   
   p_tmp_string[3] = (48 + (int_to_convert/1000000)); //Millions
   int_to_convert -= ((int_to_convert/1000000) * 1000000);
   
   p_tmp_string[4] = (48 + (int_to_convert/100000)); //Hundred-Thousandths
   int_to_convert -= ((int_to_convert/100000) * 100000);
   
   p_tmp_string[5] = (48 + (int_to_convert/10000)); //Ten-Thousandths
   int_to_convert -= ((int_to_convert/10000) * 10000);
   
   p_tmp_string[6] = (48 + (int_to_convert/1000)); //Thousandths
   int_to_convert -= ((int_to_convert/1000) * 1000);
   
   p_tmp_string[7] = (48 + (int_to_convert/100)); //Hundreds
   int_to_convert -= ((int_to_convert/100) * 100);
   
   p_tmp_string[8] = (48 + (int_to_convert/10)); //Tens
   int_to_convert -= ((int_to_convert/10) * 10);
   
   p_tmp_string[9] = (48 + (int_to_convert)); //Ones
   
   p_tmp_string[10] = '\0'; //Null character so other functions can use it with strlen()

}


/*
****************************************************
********** Private Function Definitions ************
****************************************************
*/
