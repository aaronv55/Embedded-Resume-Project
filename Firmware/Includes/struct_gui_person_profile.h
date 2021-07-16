/** @file struct_gui_person_profile.h
*
* @brief  This contains the common structure for a personal profile used for the "References"
*         app.
* @author Aaron Vorse
* @date   8/24/2020
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

#ifndef STRUCT_GUI_PERSON_PROFILE_H
#define STRUCT_GUI_PERSON_PROFILE_H

#include <stdint.h>


typedef struct t_person_profile_tag
{
   char *name, *phone_number, *email, *job_title, *company;
   uint32_t profile_image;

} t_person_profile;


#endif /* STRUCT_GUI_PERSON_PROFILE_H */

/* end of file */
