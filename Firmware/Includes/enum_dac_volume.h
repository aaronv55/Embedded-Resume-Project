/** @file enum_dac_volume.h
*
* @brief  This file contains an enum type for defining volume levels of the DAC
* @author Aaron Vorse
* @date 7/25/2020
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


#ifndef ENUM_DAC_VOLUME_H
#define ENUM_DAC_VOLUME_H

typedef enum e_dac_volume_type_tag
{

   volume_muted,
   volume_low,
   volume_medium,
   volume_high,
   volume_on_init

} e_dac_volume_type;

#endif /* ENUM_DAC_VOLUME_H */

/* end of file */
