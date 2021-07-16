/** @file states.c
*
* @brief This file contains the main logic for the system's state machine.
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

#include "states.h"

#pragma GCC push_options
#pragma GCC optimize ("O3")

/*
****************************************************
***** Private Types and Structure Definitions ******
****************************************************
*/

typedef enum e_event_main_tag
{
   no_main_event,
   references_call,
   portfolio_call,
   skills_call,
   contact_call,
   about_me_call,
   device_story_call,
   language_call,
   why_company_call,
   intro_call,
   homescreen_call,
   headphone_error,
   low_battery_event,
   settings_event,
   restore_context_call,
   external_user_button_event,
   semi_sleep_event,
   sleep_event,
   max_main_event

} e_event_main;


typedef enum e_state_main_tag
{
   startup_state,
   system_init_state,
   home_screen,
   references_state,
   portfolio_state,
   skills_state,
   contact_state,
   about_me_state,
   device_story_state,
   language_state,
   why_company_state,
   intro_state,
   battery_handler_state,
   settings_state,
   semi_sleep_state,
   sleep_state,
   max_main_state

} e_state_main;

typedef enum e_app_number_tag
{
   references_app_number,
   portfolio_app_number,
   skills_app_number,
   contact_app_number,
   about_me_app_number,
   device_story_app_number,
   language_app_number,
   why_company_app_number,
   intro_app_number,
   max_app_number

} e_app_number;


typedef enum e_substate_tag
{
   substate0,
   substate1,
   substate2,
   substate3,
   substate4,
   substate5,
   substate6,
   substate7,
   substate8,

} e_substate;


typedef enum e_audio_event_tag
{
   audio_no_event,
   audio_play_event,
   audio_pause_event,
   audio_user_button_event,
   audio_end_of_clip_event,
   audio_slide_change_event,
   audio_substate_change_event,
   audio_max_event

} e_audio_event;


typedef enum e_audio_state_tag
{
   audio_idle_state,
   audio_start_transmission_state,
   audio_playing_state,
   audio_paused_state,
   audio_stop_transmission_state,
   audio_substate_change_state,
   audio_max_state

} e_audio_state;


typedef struct t_slide_type_tag
{
   uint8_t image, audio;

} t_slide_type;


/*
****************************************************
************* File-Static Variables ****************
****************************************************
*/

static uint32_t address_buffer[max_total_addresses] = {0};
static e_state_main current_main_state = startup_state;
static e_event_main current_main_event = no_main_event;
static e_substate current_substate = substate0;
static uint32_t current_audio = null_address;
static e_audio_state current_audio_state = audio_idle_state;
static e_audio_event current_audio_event = audio_no_event;
static uint8_t current_slide_number = 0;

static t_button main_home_buttons[9] =
{
      {.width = 75, .height = 75,  .x_position = 21,  .y_position =52}, //References
      {.width = 75, .height = 75,  .x_position = 119, .y_position = 52},//Portfolio
      {.width = 75, .height = 75,  .x_position = 220, .y_position = 52},//Skills
      {.width = 75, .height = 75,  .x_position = 21,  .y_position = 152},//Contact
      {.width = 75, .height = 75,  .x_position = 119, .y_position = 152},//About Me
      {.width = 75, .height = 75,  .x_position = 220, .y_position = 152},//Device
      {.width = 75, .height = 75,  .x_position = 21,  .y_position =252},//Languages
      {.width = 75, .height = 75,  .x_position = 119, .y_position = 252},//Company
      {.width = 75, .height = 75,  .x_position = 220, .y_position = 252}//intro
};


/*
****************************************************
********** Private Function Prototypes *************
****************************************************
*/

/***************Main Apps**************************/
void states_app_home(void);

void states_app_references(void);
void states_app_references_substate0(void);
void states_references_substate_handler(void);

void states_app_portfolio(void);
void states_portfolio_substate_handler(void);
void states_portfolio_update_slide(uint32_t *tmp_slide_info);
void states_portfolio_update_menu3(void);

void states_app_skills(void);

void states_app_contact(void);

void states_app_about_me(void);
void states_about_me_substate_handler(uint32_t *tmp_image_list);

void states_app_device(void);
void states_device_substate_handler(void);
void states_device_update_menu3(void);
void states_device_update_slide(uint32_t *tmp_slide_info);

void states_app_languages(void);
void states_languages_substate_handler(void);
void states_languages_update_menu3(void);
void states_languages_update_slide(uint32_t *tmp_slide_info);

void states_app_company(void);

void states_app_intro(void);
void states_intro_button_handler(uint8_t *tmp_startup_status_flag);

/*********Main/SubState Machine functions*********/
e_event_main states_get_main_event(void);
e_state_main states_get_main_state(void);
void states_set_main_state(e_state_main new_state);
void states_set_main_event(e_event_main new_event);
void states_main_event_homescreen(void);
void states_main_event_user_buttons(void);
void states_main_event_battery(void);
void states_main_event_main_power_button(void);

uint8_t states_get_substate(void);
void states_set_substate(uint8_t new_state);
uint8_t states_get_current_slide_number(void);
void states_set_current_slide_number(uint8_t tmp_slide_number);

void states_system_init(void);
void states_battery_handler(void);
void states_settings_menu(void);
void states_settings_main_button_handler(void);
void states_settings_time_button_handler(void);
void states_previous_context(uint8_t action_select);

void states_semi_sleep(void);
void states_sleep(void);
void states_mcu_to_deepsleep(void);
uint8_t states_read_startup_flag(void);
void states_write_startup_flag(uint8_t startup_flag_status);
void states_menu3_general_button_handler(uint32_t tmp_total_slides);


/************Audio State Machine Functions*********/
uint32_t states_get_current_audio(void);
e_audio_event states_get_current_audio_event(void);
void states_set_current_audio_event(e_audio_event tmp_event);
e_audio_state states_get_current_audio_state(void);
void states_set_current_audio_state(e_audio_state tmp_state);

void states_audio_idle(void);
uint32_t states_audio_start_transmission(void);
void states_audio_playing(uint32_t tmp_total_blocks, uint8_t tmp_early_termination_flag);
void states_audio_pause(void);
void states_audio_stop_transmission(void);
void states_audio_substate_change(void);
e_audio_state states_audio_state_transition_handler(void);


/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/


/*!
* @brief Read the file list stored on the SD card and place them into the local address_buffer array
*        so they can be used in the main state machine
* @param[in] NONE
* @return NONE
*
*/
void
states_init(void)
{
   sd_get_file_addresses(address_buffer);
}


/*!
* @brief This looks at all possible events in the main system state machine, ranks them in order
*        of importance, then sets the current main event to the most important.
* @param[in] NONE
* @return NONE
*
*/
void
states_update_main_event(void)
{
   //Clear the main event status to begin with
   states_set_main_event(no_main_event);

   //Check if any homescreen-related events have triggered
   states_main_event_homescreen();

   //See if any user buttons have been pressed
   states_main_event_user_buttons();

   //Check for low battery level or battery handler events
   states_main_event_battery();

   //Check if the main power button has been pressed
   states_main_event_main_power_button();
}


/*!
* @brief Uses the current main event along with the current main state to determine the next appropriate state
* @param[in] NONE
* @return NONE
*
*/
void
states_update_main_state(void)
{
   static void (*main_state_table[max_main_state][max_main_event])(void) =      //[current state][current event]
   {
      /* no_event                references_call             portfolio_call          skills_call             contact_call           about_me_call
       * device_story_call       language_call               why_company_call        intro_call              homescreen_call        headphone_error
       * low_battery             settings                    restore_context_call    external_button         semi_sleep             sleep
       */

      //Startup
      {states_system_init,       states_system_init,         states_system_init,     states_system_init,     states_system_init,    states_system_init,
       states_system_init,       states_system_init,         states_system_init,     states_system_init,     states_system_init,    states_system_init,
       states_system_init,       states_system_init,         states_system_init,     states_system_init,     states_system_init,    states_system_init},
      //System Init
      {states_app_home,          states_app_home,            states_app_home,        states_app_home,        states_app_home,       states_app_home,
       states_app_home,          states_app_home,            states_app_home,        states_app_intro,       states_app_home,       states_app_home,
       states_app_home,          states_app_home,            states_app_home,        states_app_home,        states_app_home,       states_app_home},
      //Home
      {states_app_home,          states_app_references,      states_app_portfolio,   states_app_skills,      states_app_contact,    states_app_about_me,
       states_app_device,        states_app_languages,       states_app_company,     states_app_intro,       states_app_home,       states_app_home,
       states_battery_handler,   states_settings_menu,       states_app_home,        states_semi_sleep,      states_semi_sleep,     states_sleep},
      //References
      {states_app_references,    states_app_references,      states_app_references,  states_app_references,  states_app_references, states_app_references,
       states_app_references,    states_app_references,      states_app_references,  states_app_references,  states_app_home,       states_app_home,
       states_battery_handler,   states_settings_menu,       states_app_references,  states_semi_sleep,      states_semi_sleep,     states_sleep},
      //Portfolio
      {states_app_portfolio,     states_app_portfolio,       states_app_portfolio,   states_app_portfolio,   states_app_portfolio,  states_app_portfolio,
       states_app_portfolio,     states_app_portfolio,       states_app_portfolio,   states_app_portfolio,   states_app_home,       states_app_home,
       states_battery_handler,   states_settings_menu,       states_app_portfolio,   states_semi_sleep,      states_semi_sleep,     states_sleep},
      //Skills
      {states_app_skills,        states_app_skills,          states_app_skills,      states_app_skills,      states_app_skills,     states_app_skills,
       states_app_skills,        states_app_skills,          states_app_skills,      states_app_skills,      states_app_home,       states_app_home,
       states_battery_handler,   states_settings_menu,       states_app_skills,      states_semi_sleep,      states_semi_sleep,     states_sleep},
      //Contact
      {states_app_contact,       states_app_contact,         states_app_contact,     states_app_contact,     states_app_contact,    states_app_contact,
       states_app_contact,       states_app_contact,         states_app_contact,     states_app_contact,     states_app_home,       states_app_home,
       states_battery_handler,   states_settings_menu,       states_app_contact,     states_semi_sleep,      states_semi_sleep,     states_sleep},
      //About Me
      {states_app_about_me,      states_app_about_me,        states_app_about_me,    states_app_about_me,    states_app_about_me,   states_app_about_me,
       states_app_about_me,      states_app_about_me,        states_app_about_me,    states_app_about_me,    states_app_home,       states_app_home,
       states_battery_handler,   states_settings_menu,       states_app_about_me,    states_semi_sleep,      states_semi_sleep,     states_sleep},
      //Device Story
      {states_app_device,        states_app_device,          states_app_device,      states_app_device,      states_app_device,     states_app_device,
       states_app_device,        states_app_device,          states_app_device,      states_app_device,      states_app_home,       states_app_home,
       states_battery_handler,   states_settings_menu,       states_app_device,      states_semi_sleep,      states_semi_sleep,     states_sleep},
      //Language
      {states_app_languages,     states_app_languages,       states_app_languages,   states_app_languages,   states_app_languages,  states_app_languages,
       states_app_languages,     states_app_languages,       states_app_languages,   states_app_languages,   states_app_home,       states_app_home,
       states_battery_handler,   states_settings_menu,       states_app_languages,   states_semi_sleep,      states_semi_sleep,     states_sleep},
      //Why Company
      {states_app_company,       states_app_company,         states_app_company,     states_app_company,     states_app_company,    states_app_company,
       states_app_company,       states_app_company,         states_app_company,     states_app_company,     states_app_home,       states_app_home,
       states_battery_handler,   states_settings_menu,       states_app_company,     states_semi_sleep,      states_semi_sleep,     states_sleep},
      //Intro
      {states_app_intro,         states_app_intro,           states_app_intro,       states_app_intro,       states_app_intro,      states_app_intro,
       states_app_intro,         states_app_intro,           states_app_intro,       states_app_intro,       states_app_home,       states_app_home,
       states_battery_handler,   states_settings_menu,       states_app_intro,       states_semi_sleep,      states_semi_sleep,     states_sleep},
      //Battery handler
      {states_battery_handler,   states_battery_handler,     states_battery_handler, states_battery_handler, states_battery_handler,states_battery_handler,
       states_battery_handler,   states_battery_handler,     states_battery_handler, states_battery_handler, states_app_home,       states_app_home,
       states_battery_handler,   states_battery_handler,     states_battery_handler, states_semi_sleep,      states_semi_sleep,     states_sleep},
      //Settings
      {states_settings_menu,     states_settings_menu,       states_settings_menu,   states_settings_menu,   states_settings_menu,  states_settings_menu,
       states_settings_menu,     states_settings_menu,       states_settings_menu,   states_settings_menu,   states_app_home,       states_settings_menu,
       states_settings_menu,     states_settings_menu,       states_settings_menu,   states_semi_sleep,      states_semi_sleep,     states_sleep},
      //Semi Sleep
      {states_semi_sleep,        states_semi_sleep,          states_semi_sleep,      states_semi_sleep,      states_semi_sleep,     states_semi_sleep,
       states_semi_sleep,        states_semi_sleep,          states_semi_sleep,      states_semi_sleep,      states_semi_sleep,     states_semi_sleep,
       states_semi_sleep,        states_semi_sleep,          states_semi_sleep,      states_semi_sleep,      states_semi_sleep,     states_sleep,  },
      //Sleep
      {states_sleep,             states_sleep,               states_sleep,           states_sleep,           states_sleep,          states_sleep,
       states_sleep,             states_sleep,               states_sleep,           states_sleep,           states_sleep,          states_sleep,
       states_sleep,             states_sleep,               states_app_home,        states_sleep,           states_sleep,          states_sleep,  },
   };


   e_event_main tmp_current_event = states_get_main_event();

   if(homescreen_call == tmp_current_event)
   {
      //Reset current slide and substate, so the next time an app is called it doesn't jump to a random menu
      states_set_substate(substate0);
      states_set_current_slide_number(0);

      //Redundancy to make sure that the system will shut off any sd card/audio activity before going to the homescreen
      //this makes sure the sd card never locks up and the DAC is never left enabled
      sd_stop_transmission();
      dac_cutoff_transmission();
   }

   e_state_main tmp_current_state = states_get_main_state();

   //Bounds check on table indices
   if((tmp_current_state < max_main_state) && (tmp_current_event < max_main_event))
   {
      main_state_table[tmp_current_state][tmp_current_event]();
   }

   //Reset event, so the same event does not trigger a transition more than once
   states_set_main_event(no_main_event);
}


/*!
* @brief Redraw the footer in order to match the current menu
* @param[in] NONE
* @return  NONE
*/
void
states_update_footer(void)
{
   static e_state_main previous_main_state = home_screen;
   static e_substate previous_substate = substate0;
   static uint16_t previous_footer_color = 0;

   uint16_t current_footer_color = gui_get_footer_color();

   //Check to see if the footer color has changed or there was a state change and the footer needs to be updated
   if((states_get_main_state() != previous_main_state) ||
      (states_get_substate() != previous_substate) ||
      (gui_get_footer_color() != previous_footer_color))
   {
      lcd_draw_rectangle(current_footer_color, 0, FOOTER_OFFSET, 320, 480);
      uint8_t length = (sizeof(user_button_template) / sizeof(*user_button_template)); //This should always be 3 for user buttons
      uint8_t active_button = NO_BUTTON_PRESS;
      active_button = buttons_status(user_button_template,length);

      //If the current button is active, draw it with a darker color
      for(uint8_t current_button = 0; current_button < length; current_button++)
      {
         uint16_t button_color = THEME_NEAR_WHITE;

         if(current_button == active_button)
         {
            button_color = GRAY_MEDIUM;
         }

         else
         {
            button_color = THEME_NEAR_WHITE;
         }

         uint16_t x = user_button_template[current_button].x_position;
         uint16_t y = user_button_template[current_button].y_position;
         const uint8_t *p_bmp = user_button_template[current_button].bmp;

         lcd_send_bitmap(p_bmp, x, y, button_color, gui_get_footer_color());
      }
   }

   previous_substate = states_get_substate();
   previous_main_state = states_get_main_state();
   previous_footer_color = gui_get_footer_color();
}


/*!
* @brief This services any audio clip that may be running. It is also responsible
*        for stopping audio transmissions safely (not locking up the SD card)
*        upon state transitions and various events.
* @param[in] NONE
* @return  NONE
*/
void
states_update_audio(void)
{
   static uint8_t tmp_previous_slide = 0;
   uint8_t tmp_current_slide = states_get_current_slide_number();
   static e_substate previous_substate = substate0 ;
   e_substate tmp_current_substate = states_get_substate();
   static uint8_t early_termination_flag = 0;

   //If the user has selected a menu option, give a click sound before advancing
   // to the next substate
   if((previous_substate != tmp_current_substate) ||
      (tmp_previous_slide != tmp_current_slide))
   {
      states_set_current_audio_event(audio_substate_change_event);
      early_termination_flag = 1;
   }

   //If the repeat button was pressed, tell the system the audio was cut off early
   else if(audio_end_of_clip_event == states_get_current_audio_event())
   {
      early_termination_flag = 1;
   }

   //Pressing the home or back button supersedes all other events, since the audio transfer needs to end to
   //avoid audio playing outside of its menu or worse, locking up the sd card. This is for reentrancy safety.
   uint8_t active_button = buttons_status(user_button_template,(sizeof(user_button_template) / sizeof(*user_button_template)));

   if(NO_BUTTON_PRESS != active_button)
   {
      states_set_current_audio_event(audio_user_button_event);
      early_termination_flag = 1;
   }

   //Get the next state based on the current audio state and event
   e_audio_state tmp_next_audio_state = states_audio_state_transition_handler();

   switch(tmp_next_audio_state)
   {
      static uint32_t total_blocks = 0; //Total blocks the current file takes up in the SD card

      case audio_idle_state:
         states_audio_idle();
         break;
      case audio_start_transmission_state:
         total_blocks = states_audio_start_transmission();
         break;
      case audio_playing_state:
      {
         uint8_t end_of_conversion = dac_service_audio(total_blocks, early_termination_flag);

         if(1 == end_of_conversion)
         {
            states_set_current_audio_event(audio_end_of_clip_event);
         }

         early_termination_flag = 0;
         break;
      }
      case audio_paused_state:
         states_audio_pause();
         break;
      case audio_stop_transmission_state:
         states_audio_stop_transmission();
         break;
      case audio_substate_change_state:
         states_audio_substate_change();
         break;
      default:
         break;
   }

   timers_update_audio_clock();
   previous_substate = tmp_current_substate;
   tmp_previous_slide = tmp_current_slide;
}

/*!
* @brief Get the most recent audio clip that is either currently playing or was the last one to play
* @param[in] NONE
* @return current_audio 32-bit address of the audio file that was played
*/
uint32_t
states_get_current_audio(void)
{
   return(current_audio);
}


/*!
* @brief Set the audio clip to be played
* @param[in] audio_address
* @return  NONE
*/
void
states_set_current_audio(uint32_t audio_address)
{
   current_audio = audio_address;
}


/*
****************************************************
********** Private Function Definitions ************
****************************************************
*/


/*!
* @brief This plays a bootup animation and initializes current state as the system starts up
* @param[in] NONE
* @return NONE
*
*/
void
states_system_init(void)
{
   uint32_t animation_frames[13] =
    {
          address_buffer[startup_animation_0], address_buffer[startup_animation_1], address_buffer[startup_animation_2],
          address_buffer[startup_animation_3], address_buffer[startup_animation_4], address_buffer[startup_animation_5],
          address_buffer[startup_animation_6], address_buffer[startup_animation_7], address_buffer[startup_animation_8],
          address_buffer[startup_animation_9], address_buffer[startup_animation_10], address_buffer[startup_animation_11],
          address_buffer[startup_animation_12]
    };

   gui_boot_animation(animation_frames, (sizeof(animation_frames)) / (sizeof(*animation_frames)));

   //Display the home menu as a backdrop to the intro popup menu in case the system opens intro first
   gui_set_status_bar_color(GRAY_DARK);
   gui_set_footer_color(BLACK);
   lcd_image_from_sd(0, SB_OFFSET, 320, FOOTER_OFFSET + 1, address_buffer[homescreen_bmp]);

   states_set_main_state(system_init_state);
}


/*!
* @brief Used inside states_update_main_event() to trigger events which are
*        only present when the system is in the homescreen
* @param[in] NONE
* @return NONE
*
*/
void
states_main_event_homescreen(void)
{
   //Check if the program is currently in the homescreen. For all other states
   //it does not matter if the app buttons are pushed
   if(home_screen == states_get_main_state())
   {
      //Determine if the user has clicked an app icon in the home screen
      e_app_number pressed_button = buttons_status(main_home_buttons,(sizeof(main_home_buttons) / sizeof(*main_home_buttons)));

      //If a user has clicked on an app in the homescreen, set the main event to that app call
      if(TOTAL_APPS > pressed_button)
      {
         e_app_number app_event_table[TOTAL_APPS] =  //There are 9 total apps on the homescreen
         {references_call, portfolio_call, skills_call, contact_call, about_me_call,
          device_story_call, language_call, why_company_call, intro_call
         };

         states_set_main_event(app_event_table[pressed_button]);
      }
   }

   else if(system_init_state == states_get_main_state())
   {
      //If the startup flag is not set, send the system to the intro popup after initialization
      if( !(states_read_startup_flag()))
      {
         states_set_main_event(intro_call);
      }

   }

   else if (intro_state == states_get_main_state())
   {
      uint8_t active_intro_button = buttons_status(intro_menu_template, (sizeof(intro_menu_template) / sizeof(*intro_menu_template)));

      //Button 0 is the "skip" button of the intro popup
      if(0 == active_intro_button)
      {
         dac_cutoff_transmission();
         states_set_current_audio_event(audio_end_of_clip_event);
         states_set_main_event(homescreen_call);
      }
   }
}


/*!
* @brief Used inside states_update_main_event() to trigger events which are
*        related to the main user buttons
* @param[in] NONE
* @return NONE
*
*/
void
states_main_event_user_buttons(void)
{

   //Check if any user buttons were pressed
   uint8_t active_button = buttons_status(user_button_template, (sizeof(user_button_template) / sizeof(*user_button_template)));

   if(home_button == active_button)
   {
      states_set_main_event(homescreen_call);
   }

   else if ((back_button == active_button) && (states_get_substate() == substate0))
   {
      states_set_main_event(homescreen_call);
   }

   else if (square_button == active_button)
   {
      if(settings_state == states_get_main_state())
      {
         states_previous_context(CONTEXT_POP);
         states_set_main_event(restore_context_call); //If settings menu is open, close it
      }

      else
      {
         states_set_main_event(settings_event); //Open settings menu
      }

   }

}

/*!
* @brief Used inside states_update_main_event() to trigger events which are
*        related to the battery and battery handler popup
* @param[in] NONE
* @return NONE
*
*/
void
states_main_event_battery(void)
{
   static uint8_t battery_warning_flag = 0; //Flag to indicate if the low battery warning has already been issued once

   //If battery is less than 10% and there is no USB, call the low battery handler
   if(!(monitor_get_usb_status()))
   {
      uint8_t tmp_battery_level = monitor_get_battery_level(BATTERY_SAMPLE_FLAG_NONE);

      if((10 > tmp_battery_level) && (0 == battery_warning_flag))
      {
         states_set_main_event(low_battery_event);
         battery_warning_flag = 1;
      }

      //If battery percentage is less than 6% force an automatic shutdown
      if(6 > tmp_battery_level)
      {
         states_set_main_event(sleep_event);
      }

   }

   else
   {
      //Reset the flag if USB is present. This lets the system give only one battery level
      //warning after each time the USB is plugged in
      battery_warning_flag = 0;
   }


   //Check if battery handler needs to trigger an event
   uint8_t active_button = buttons_status(warning_menu_template,(sizeof(warning_menu_template) / sizeof(*warning_menu_template)));

   if((battery_handler_state == states_get_main_state()) && (NO_BUTTON_PRESS != active_button))
   {
      //Shutdown
      if(0 == active_button)
      {
         states_set_main_event(sleep_event);
      }

      //Dismiss
      else if (1 == active_button)
      {
         states_previous_context(CONTEXT_POP);
         states_set_main_event(restore_context_call);
      }

   }

}


/*!
* @brief Used inside states_update_main_event() to trigger events which are
*        related to the main power button i.e. pressing or holding
* @param[in] NONE
* @return NONE
*
*/
void
states_main_event_main_power_button(void)
{
   //Check if the main power button has been pressed
   if(1 == monitor_get_main_power_button_status())
   {
      sd_stop_transmission();
      dac_cutoff_transmission();

      e_state_main tmp_state = states_get_main_state();
      uint8_t press_type = monitor_get_power_press_type();

      switch(tmp_state)
      {

      case semi_sleep_state:
         states_previous_context(CONTEXT_POP);
         states_set_main_event(restore_context_call);
         lcd_backlight_enable();
         break;

      case settings_state:
      case battery_handler_state:
         //If the system is in a popup window (ie settings or battery handler) pop the context
         //before going to semi sleep, or context will not be correctly restored when exiting semi sleep.
         states_previous_context(CONTEXT_POP);

      default:
         //Any state other than sleep or semi-sleep
         if(MAIN_POWER_SHORT_PRESS == press_type)
         {
            states_previous_context(CONTEXT_PUSH);
            states_set_main_event(semi_sleep_event);
         }

         else
         {
            states_set_main_event(sleep_event);
         }

         break;
      }

      monitor_reset_main_power_button_status();

   }

}

/*!
* @brief Display the system's "home" screen consisting of all the available apps
* @param[in] NONE
* @return NONE
*
*/
void
states_app_home(void)
{
   gui_set_footer_color(BLACK);

   e_state_main tmp_main_state = states_get_main_state();

   if((tmp_main_state != home_screen)  ||  (restore_context_call == states_get_main_event()))
   {
      gui_set_status_bar_color(GRAY_DARK);
      lcd_image_from_sd(0, SB_OFFSET, 320, FOOTER_OFFSET+1, address_buffer[homescreen_bmp]);
   }

   states_set_main_state(home_screen);
}


/*!
* @brief This is an app consisting of past employers and their contact information
* @param[in] NONE
* @return NONE
*
*/
void
states_app_references(void)
{
   t_light_button substate0_buttons[4] =
   {
         {.major_text = "person1\0", .minor_text = "Director\0", .image[0] = address_buffer[person1_small_not_pressed],
          .image[1] = address_buffer[person1_small_pressed], .bmp = NULL},
         {.major_text = "person2\0", .minor_text = "Director\0", .image[0] = address_buffer[person2_small_not_pressed],
          .image[1] = address_buffer[person2_small_pressed],.bmp = NULL},
         {.major_text = "person3\0", .minor_text = "Supervisor\0", .image[0] = address_buffer[person3_small_not_pressed],
          .image[1] = address_buffer[person3_small_pressed],.bmp = NULL},
         {.major_text = "person4\0", .minor_text = "Supervisor\0", .image[0] = address_buffer[person4_small_not_pressed],
          .image[1] = address_buffer[person4_small_pressed], .bmp = NULL},
   };


   static e_substate previous_substate = substate0 ;
   e_substate tmp_current_substate = states_get_substate();

   //Only redraw the menu if a main state transition, a substate transition or a button press has
   //occurred. Not constantly writing helps with LCD flicker.
   if((references_state != states_get_main_state()) ||
      (tmp_current_substate != previous_substate) ||
      (restore_context_call == states_get_main_event()))
   {
      switch(tmp_current_substate)
      {
      case substate0:
         gui_create_menu_type1("References", substate0_buttons, MENU1_MAX_BUTTONS, NO_BUTTON_PRESS);
         break;
      case substate1:
         {
            const t_person_profile profile_person1 =
            {.name = "person1", .job_title = "Director", .phone_number = "(555)-555-555",
             .email = "Example@csulb.edu", .company = "Company\0", .profile_image = address_buffer[person1_large]};

            gui_create_menu_type2(profile_person1);
            break;
         }

      case substate2:
         {
            const t_person_profile profile_person2 =
            {.name = "person2", .job_title = "Director", .phone_number = "(555)-555-555",
             .email = "example@csulb.edu", .company = "Company\0", .profile_image = address_buffer[person2_large]};

            gui_create_menu_type2(profile_person2);
            break;
         }
      case substate3:
         {
            const t_person_profile profile_person3 =
            {.name = "person3", .job_title = "Supervisor", .phone_number = "(555)-555-555",
             .email = "example@csulb.edu", .company = "Company\0", .profile_image = address_buffer[person3_large]};

            gui_create_menu_type2(profile_person3);
            break;
         }
      case substate4:
         {
            const t_person_profile profile_person4 =
            {.name = "person4", .job_title = "Supervisor", .phone_number = "(555)-555-555",
             .email = "example@csulb.edu", .company = "Company\0", .profile_image = address_buffer[person4_large]};

            gui_create_menu_type2(profile_person4);
            break;
         }
      default:
         break;
      }

   }


   if(references_state == states_get_main_state())
   {
      gui_update_menu1_buttons(substate0_buttons, substate0, states_get_substate());
   }

   previous_substate = states_get_substate();
   states_references_substate_handler();
   states_set_main_state(references_state);

}


/*!
* @brief Uses the current substate and button information to transition to
*        the correct next state.
* @param[in] NONE
* @return NONE
*
*/
void
states_references_substate_handler(void)
{

   if(references_state == states_get_main_state())
   {
      uint8_t active_button = buttons_status(menu1_template,(sizeof(menu1_template) / sizeof(*menu1_template)));
      e_substate tmp_current_substate = states_get_substate();

      //Only substate 0 (The main list) has interactive buttons
      if((substate0 == tmp_current_substate) && (NO_BUTTON_PRESS != active_button))
      {
         static const e_substate transition_table[] ={substate1, substate2, substate3, substate4};
         states_set_substate(transition_table[active_button]);
      }

      //If the back button was pressed, decrease and go back to the main list
      else
      {
         active_button = buttons_status(user_button_template,(sizeof(user_button_template) / sizeof(*user_button_template)));

         if(back_button == active_button)
         {
            states_set_substate(substate0);
         }
      }
   }
}


/*!
* @brief This app displays a series of slides (pictures with audios) showcasing my past work
* @param[in] NONE
* @return NONE
*
*/
void
states_app_portfolio(void)
{
   static t_light_button substate0_buttons[4] =
   {
         {.major_text = "Micro FOBO", .minor_text = "Bipedal Robot", .image[0] = null_address,
          .image[1] = null_address, .bmp = FOBO_icon_bmp},
         {.major_text = "Gameboard", .minor_text = "Custom PCB", .image[0] = null_address,
          .image[1] = null_address, .bmp = tamagotchi_icon_bmp},
         {.major_text = "Data Acquisition", .minor_text = "Mixed Signal", .image[0] = null_address,
          .image[1] = null_address, .bmp = data_acquisition_icon_bmp},
         {.major_text = "Additional", .minor_text = "Mini Projects", .image[0] = null_address,
          .image[1] = null_address, .bmp = additional_icon_bmp},
   };

   static uint8_t tmp_previous_slide = 0;
   uint8_t tmp_current_slide = states_get_current_slide_number();
   static e_substate previous_substate = substate0 ;
   e_substate tmp_current_substate = states_get_substate();

   //Only redraw the menu if a main state transition or a substate transition has
   //occurred. Not constantly writing helps with LCD flicker.
   if((states_get_main_state() != portfolio_state) ||
      (tmp_current_substate != previous_substate) ||
      (tmp_current_slide != tmp_previous_slide) ||
      (restore_context_call == states_get_main_event()))
   {
      //Substate0 is the only one with a menu type1, all others are type 3
      if(substate0 == tmp_current_substate)
      {
         gui_create_menu_type1("Portfolio", substate0_buttons, MENU1_MAX_BUTTONS, NO_BUTTON_PRESS);
      }

      else
      {
         gui_create_menu_type3();
      }
   }

   if(portfolio_state == states_get_main_state())
   {
      states_portfolio_update_menu3();
      gui_update_menu1_buttons(substate0_buttons, substate0, states_get_substate());
   }


   previous_substate = states_get_substate();
   tmp_previous_slide = tmp_current_slide;
   states_portfolio_substate_handler();
   states_set_main_state(portfolio_state);
}


/*!
* @brief Uses the current substate and button information to transition to
*        the correct next state.
* @param[in] NONE
* @return NONE
*
*/
void
states_portfolio_substate_handler(void)
{
   uint8_t active_button = buttons_status(menu1_template,(sizeof(menu1_template) / sizeof(*menu1_template)));
   e_substate tmp_current_substate = states_get_substate();

   if(portfolio_state == states_get_main_state())
   {
      //Service substate 0 (The main list) buttons
      if((substate0 == tmp_current_substate) && (NO_BUTTON_PRESS != active_button))
      {
         static const e_substate transition_table[] ={substate1, substate2, substate3, substate4};
         states_set_substate(transition_table[active_button]);
      }

      //If the back button was pressed, go back to the main list and reset the slide number
      else
      {
         active_button = buttons_status(user_button_template,(sizeof(user_button_template) / sizeof(*user_button_template)));
         if(back_button == active_button)
         {
            states_set_substate(substate0);
            states_set_current_slide_number(0);
         }
      }
   }
}


/*!
* @brief Monitor all menu3's in the portfolio app and update the audio and display
*        attributes as necessary
* @param[in] NONE
* @return NONE
*
*/
void
states_portfolio_update_menu3(void)
{
   static e_substate tmp_previous_substate = substate0 ;
   e_substate tmp_current_substate = states_get_substate();

   //Substate0 is the only non-type 3 menu
   if(substate0 < tmp_current_substate)
   {
      uint8_t state_change_flag = 0;

      static uint8_t tmp_previous_slide = 0;
      uint8_t tmp_current_slide = states_get_current_slide_number();

      static uint32_t slide_info[2] = {0}; //Total blocks, total slides

      //Update the display if there was a substate or slide change
      if((tmp_previous_substate != tmp_current_substate) ||
          (tmp_previous_slide != tmp_current_slide) ||
          (restore_context_call == states_get_main_event()))
      {
         states_portfolio_update_slide(slide_info);
         state_change_flag = 1;
      }

      //Update the slide audio display
      tmp_previous_slide = tmp_current_slide;
      gui_update_menu3_clock(slide_info[0], state_change_flag);
      states_menu3_general_button_handler(slide_info[1]);

   }

   tmp_previous_substate = tmp_current_substate;

}


/*!
* @brief Select the current audio and draw the current image
* @param[in] NONE
* @return NONE
*
*/
void
states_portfolio_update_slide(uint32_t *tmp_slide_info)
{
   /* fobo slides */
   static const t_slide_type substate1_slides[4] =
   {
      {.image = slide_portfolio_fobo_standing, .audio = portfolio_fobo1_audio}, {.image = slide_portfolio_fobo_leg, .audio = portfolio_fobo2_audio},
      {.image = slide_portfolio_fobo_fritzing, .audio = portfolio_fobo3_audio}, {.image = slide_portfolio_fobo_code, .audio = portfolio_fobo4_audio}
   };

   /* tamagotchi slides */
   static const t_slide_type substate2_slides[5] =
   {
      {.image = slide_portfolio_tamogatchi_pcbactual, .audio = portfolio_gameboard1_audio}, {.image = slide_portfolio_tamogatchi_perf, .audio = portfolio_gameboard2_audio},
      {.image = slide_portfolio_tamogatchi_pcb3D, .audio = portfolio_gameboard3_audio},     {.image = slide_portfolio_tamogatchi_code, .audio = portfolio_gameboard4_audio},
      {.image = slide_portfolio_tamogatchi_case, .audio = portfolio_gameboard5_audio}
   };

   /* data acquisition slides */
   static const t_slide_type substate3_slides[6] =
   {
      {.image = slide_portfolio_acq_breadboard, .audio = portfolio_data1_audio}, {.image = slide_portfolio_acq_filter, .audio = portfolio_data3_audio},
      {.image = slide_portfolio_acq_bode, .audio = portfolio_data2_audio}, {.image = slide_portfolio_acq_adc, .audio = portfolio_data4_audio},
      {.image = slide_portfolio_acq_memory, .audio = portfolio_data5_audio}, {.image = slide_portfolio_acq_gui, .audio = portfolio_data6_audio}
   };

   /* other side projects slides */
   static const t_slide_type substate4_slides[5] =
   {
      {.image = slide_portfolio_other_candy_main, .audio = portfolio_candymain_audio}, {.image = slide_portfolio_other_candy_minor, .audio = portfolio_candy3D_audio},
      {.image = slide_portfolio_other_workshop, .audio = portfolio_workshop_audio}, {.image = slide_portfolio_other_UA741, .audio = portfolio_ua741_audio},
      {.image = slide_portfolio_other_recorder, .audio = portfolio_voice_audio}
   };


   const t_slide_type *p_current_slide_array = NULL; // Pointer to choose the correct slide array
   e_substate tmp_current_substate = states_get_substate();

   uint8_t array_length = 0;

   //Select the appropriate slide based on the curren tsubstate
   switch(tmp_current_substate)
   {
   case substate1:
      p_current_slide_array = substate1_slides;
      array_length = sizeof(substate1_slides) / sizeof(substate1_slides[0]);
      break;
   case substate2:
      p_current_slide_array = substate2_slides;
      array_length = sizeof(substate2_slides) / sizeof(substate2_slides[0]);
      break;
   case substate3:
      p_current_slide_array = substate3_slides;
      array_length = sizeof(substate3_slides) / sizeof(substate3_slides[0]);
      break;
   case substate4:
      p_current_slide_array = substate4_slides;
      array_length = sizeof(substate4_slides) / sizeof(substate4_slides[0]);
      break;
   default:
      break;
   }

   //Return the total number of slides in the current array
   tmp_slide_info[1] = array_length;

   uint8_t tmp_current_slide = states_get_current_slide_number();
   tmp_slide_info[0] = sd_get_wav_size((p_current_slide_array + tmp_current_slide)->audio); //Return the total audio size in blocks

   //Update image and audio based on the current slide
   states_set_current_audio(address_buffer[(p_current_slide_array + tmp_current_slide)->audio]);
   lcd_image_from_sd(0, SB_OFFSET, 320, MENU3_UTILITIES_BAR_OFFSET, address_buffer[(p_current_slide_array + tmp_current_slide)->image]);
   lcd_draw_rectangle(BLACK, 0, (MENU3_UTILITIES_BAR_OFFSET - 1), 320, MENU3_UTILITIES_BAR_OFFSET); //Black bar at top of slide menu
}


/*!
* @brief This app displays a list of bitmaps and text which represent my embedded systems skills
* @param[in] NONE
* @return NONE
*
*/
void
states_app_skills(void)
{
   if(skills_state != states_get_main_state() || (restore_context_call == states_get_main_event()))
   {

      const t_light_button menu_skills_template[7] =
       {
             {.major_text = "Arm Microcontrollers\0",  .minor_text = "Cortex M,STM32\0", .image[0] = address_buffer[skills_arm]},    //Arm

             {.major_text = "Circuit Design\0", .minor_text = "Analog,Digital\0", .image[0] = address_buffer[skills_circuit] },      //Circuit Design

             {.major_text = "Embedded C\0", .minor_text = "Keil,Eclipse IDEs\0", .image[0] = address_buffer[skills_c]},              //Embedded C

             {.major_text = "Lab Equipment\0", .minor_text = "Oscope,DMM,DLA etc.\0", .image[0] = address_buffer[skills_equipment]}, //Equipment                                    //Equipment

             {.major_text = "PCB Design\0", .minor_text = "DFM,high speed\0", .image[0] = address_buffer[skills_pcb]},               //PCB Design

             {.major_text = "SMD/THT Soldering\0", .minor_text = "Rework,fine-pitch\0", .image[0] = address_buffer[skills_solder]},   //Solder

             {.major_text = "Version Control\0", .minor_text = "Git/GitHub\0", .image[0] = address_buffer[github_logo]},           //Github
       };

         gui_create_menu_skills(menu_skills_template, (sizeof(menu_skills_template) / sizeof(*menu_skills_template)));
         states_set_main_state(skills_state);
   }
}


/*!
* @brief This app displays a profile picture of the author (Aaron Vorse) along with his contact information
* @param[in] NONE
* @return NONE
*
*/
void
states_app_contact(void)
{
   if(contact_state != states_get_main_state() || (restore_context_call == states_get_main_event()))
   {

      const t_light_button menu_contact_template[5] =
      {

            {.major_text = "(555)-555-5555\0", .image[0] = null_address,                             //Phone number
             .bmp = phone_icon_bmp},

            {.major_text = "example@gmail.com\0", .image[0] = null_address,                        //Email address
             .bmp = mail_icon_bmp},

            {.major_text = "github.com/aaronv55\0", .image[0] = address_buffer[github_logo_light],    //Github
             .bmp = NULL},

            {.major_text = "linkedin.com/in\0", .minor_text = "/example\0", .image[0] = address_buffer[linkedin_logo], //LinkedIn
             .bmp = NULL},

            {.major_text = "Aaron Vorse\0", .image[0] = address_buffer[aaron_large]},                  //Profile Picture and name
      };

      gui_create_menu_contact_info(menu_contact_template);
      states_set_main_state(contact_state);
   }

}


/*!
* @brief This app consists of a series of submenus which give details about the author (Aaron Vorse)
*        such as experience, education, interests etc.
* @param[in] NONE
* @return NONE
*
*/
void
states_app_about_me(void)
{
   //Create list of button pictures, since gui.h doesn't have access to SD card file addresses directly
   uint32_t main_menu_button_images[5] =
   {
         address_buffer[about_me_main_education] ,address_buffer[about_me_main_goals],
         address_buffer[about_me_main_hobbies], address_buffer[about_me_main_interests], address_buffer[about_me_main_experience]
   };


   if(about_me_state != states_get_main_state())
   {
      gui_set_footer_color(GRAY_MEDIUM);
      gui_set_status_bar_color(GRAY_MEDIUM);

      states_set_main_state(about_me_state);

      gui_create_aboutme_main_menu(main_menu_button_images);
   }


   else if(about_me_state == states_get_main_state() || (restore_context_call == states_get_main_event()))
   {
      states_about_me_substate_handler(main_menu_button_images);
   }

}


/*!
* @brief Uses the current substate and button information to transition to
*        the correct next state.
* @param[in] tmp_button_images List of images for the main menu buttons
* @return NONE
*
*/
void
states_about_me_substate_handler(uint32_t *tmp_button_images)
{

   //Functions to call based on the current substate
   void (*substate_functions[5])(void) =
   {
         gui_create_aboutme_education_menu,
         gui_create_aboutme_goals_menu,
         gui_create_aboutme_hobbies_menu,
         gui_create_aboutme_interests_menu,
         gui_create_aboutme_experience_menu
   };

   //Create list of each submenu picture, since gui.h doesn't have access to SD card file addresses directly
   uint32_t submenu_images[5] =
   {
         address_buffer[about_me_sub_education], address_buffer[about_me_sub_goals],
         address_buffer[about_me_sub_hobbies], address_buffer[about_me_sub_interests], address_buffer[about_me_sub_experience]
   };


   uint8_t active_button = buttons_status(about_me_menu_template, sizeof(about_me_menu_template) / sizeof(*about_me_menu_template));

   //Check if the system is restoring the context, aka it was in a pop-up menu like the settings_menu
   if(restore_context_call != states_get_main_event())
   {
      //Check if a main menu button was pressed
      if((NO_BUTTON_PRESS != active_button) && substate0 == states_get_substate())
      {
        //Go to substate screen corresponding to each button. @note Substates are offset by 1, since
         // substate0 is already the main menu
         states_set_substate(active_button + 1);
         lcd_image_from_sd(0, SB_OFFSET, 320, MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET + 1, submenu_images[active_button]);
         substate_functions[active_button]();
      }
   }

   else
   {
      e_substate tmp_substate = states_get_substate();

      //If the system is returning to a submenu recreate it, otherwise it is returning
      //to the main about_me menu
      if(substate0 < tmp_substate)
      {

         tmp_substate -= 1; //Subtract 1 to offset the main menu already taking up substate0

         lcd_image_from_sd(0, SB_OFFSET, 320,MENU_ABOUTME_SUBMENU_TEXT_Y_OFFSET + 1, submenu_images[tmp_substate]);
         substate_functions[tmp_substate]();
      }

      else
      {
         gui_create_aboutme_main_menu(tmp_button_images);
      }

   }


   //Check if the back button is pressed, and if so return to the main about_me menu
   active_button = buttons_status(user_button_template, sizeof(user_button_template) / sizeof(*user_button_template));

   if(back_button == active_button)
   {
      states_set_substate(substate0);
      gui_create_aboutme_main_menu(tmp_button_images);
   }
}


/*!
* @brief This app displays a series of slide describing how the Embedded Resume Project was created
* @param[in] NONE
* @return NONE
*
*/
void
states_app_device(void)
{
  static t_light_button substate0_buttons[4] =
   {
         {.major_text = "How it Started\0", .minor_text = "Inspiration\0", .image[0] = null_address,
          .image[1] = null_address, .bmp = NULL},
         {.major_text = "Mechanical\0", .minor_text = "PolyJet Case\0", .image[0] = null_address,
          .image[1] = null_address,.bmp = NULL},
         {.major_text = "Hardware & Firmware\0", .minor_text = "Electronics and Code\0", .image[0] = null_address,
          .image[1] = null_address,.bmp = NULL},
         {.major_text = "Productization\0", .minor_text = "Presentation\0", .image[0] = null_address,
          .image[1] = null_address, .bmp = NULL},
   };

   static uint8_t tmp_previous_slide = 0;
   uint8_t tmp_current_slide = states_get_current_slide_number();
   static e_substate previous_substate = substate0 ;
   e_substate tmp_current_substate = states_get_substate();

   //Only redraw the menu if a main state transition or a substate transition has
   //occurred. Not constantly writing helps with LCD flicker.
   if((states_get_main_state() != device_story_state) ||
      (tmp_current_substate != previous_substate) ||
      (tmp_current_slide != tmp_previous_slide) ||
      (restore_context_call == states_get_main_event()))
   {
      //Substate0 is the only one with a menu type1, all others are type 3
      if(substate0 == tmp_current_substate)
      {
         //states_set_current_slide_number(0); //Reset slides upon app entry
         gui_create_menu_type1("This Device", substate0_buttons, MENU1_MAX_BUTTONS, NO_BUTTON_PRESS);
      }

      else
      {
         gui_create_menu_type3();
      }
   }

   //If the system has already gone through the device app once, service the appropriate menu
   if(device_story_state == states_get_main_state())
   {
      states_device_update_menu3();
      gui_update_menu1_buttons(substate0_buttons, substate0, states_get_substate());
   }

   previous_substate = states_get_substate();
   tmp_previous_slide = tmp_current_slide;
   states_device_substate_handler();
   states_set_main_state(device_story_state);
}


/*!
* @brief Uses the current substate and button information to transition to
*        the correct next state.
* @param[in] NONE
* @return NONE
*
*/
void
states_device_substate_handler(void)
{

   uint8_t active_button = buttons_status(menu1_template,(sizeof(menu1_template) / sizeof(*menu1_template)));
   e_substate tmp_current_substate = states_get_substate();

   if(device_story_state == states_get_main_state())
   {
      //Service substate 0 (The main list) buttons
      if((substate0 == tmp_current_substate) && (NO_BUTTON_PRESS != active_button))
      {
         //gui_create_menu_type3();
         static const e_substate transition_table[] ={substate1, substate2, substate3, substate4};
         states_set_substate(transition_table[active_button]);
      }

      //If the back button was pressed, go back to the main list and reset the slide number
      else
      {
         active_button = buttons_status(user_button_template,(sizeof(user_button_template) / sizeof(*user_button_template)));
         if((back_button == active_button) || (home_button == active_button))
        {
            states_set_substate(substate0);
            states_set_current_slide_number(0);
        }
      }
   }
}


/*!
* @brief Monitor all menu3's in the device app, update the audio, and display
*        attributes as necessary
* @param[in] NONE
* @return NONE
*
*/
void
states_device_update_menu3(void)
{
   static e_substate tmp_previous_substate = substate0 ;
   e_substate tmp_current_substate = states_get_substate();

   //Only substate0 is not a type 3 menu
   if(substate0 < tmp_current_substate)
   {
      uint8_t state_change_flag = 0;

      static uint8_t tmp_previous_slide = 0;
      uint8_t tmp_current_slide = states_get_current_slide_number();

      static uint32_t slide_info[2] = {0}; //Total blocks, total slides

      //Update the display if there was a substate or slide change
      if((tmp_previous_substate != tmp_current_substate) ||
          (tmp_previous_slide != tmp_current_slide) ||
          (restore_context_call == states_get_main_event()))
      {
         states_device_update_slide(slide_info);
         state_change_flag = 1;
      }

      //Update the slide audio display
      tmp_previous_slide = tmp_current_slide;
      gui_update_menu3_clock(slide_info[0], state_change_flag);
      states_menu3_general_button_handler(slide_info[1]);

   }

   tmp_previous_substate = tmp_current_substate;

}



/*!
* @brief Choose the correct audio and image for the current slide
* @param[in] NONE
* @return NONE
*
*/
void
states_device_update_slide(uint32_t *tmp_slide_info)
{
   //How it started
   static const t_slide_type substate1_slides[2] =
   {
      {.image = slide_device_intro_drawing, .audio = device_start1_audio}, {.image = slide_device_intro_model, .audio = device_start2_audio}
   };

   //Mechanical
   static const t_slide_type substate2_slides[3] =
   {
      {.image = slide_device_mechanical_prototype, .audio = device_mechanical1_audio}, {.image = slide_device_mechanical_final, .audio = device_mechanical2_audio},
      {.image = slide_device_mechanical_processing, .audio = device_mechanical3_audio}
   };

   //Hardware and Firmware
   static const t_slide_type substate3_slides[6] =
   {
      {.image = slide_device_hardware_schematic, .audio = device_hwfw1_audio}, {.image = slide_device_hardware_layout, .audio = device_hwfw2_audio},
      {.image = slide_device_hardware_PCB, .audio = device_hwfw3_audio}, {.image = slide_device_hardware_solder, .audio = device_hwfw4_audio},
      {.image = slide_device_hardware_firmware, .audio = device_hwfw5_audio}, {.image = slide_device_hardware_jig, .audio = device_hwfw6_audio}
   };

   //Productization
   static const t_slide_type substate4_slides[4] =
   {
      {.image = slide_device_product_photo, .audio = device_product1_audio}, {.image = slide_device_product_box, .audio = device_product2_audio},
      {.image = slide_device_product_foam, .audio = device_product3_audio}, {.image = slide_device_product_manual, .audio = device_product4_audio}
   };


   const t_slide_type *p_current_slide_array = NULL; // Pointer to choose the correct slide array
   e_substate tmp_current_substate = states_get_substate();

   uint8_t array_length = 0;

   switch(tmp_current_substate)
   {
   case substate1:
      p_current_slide_array = substate1_slides;
      array_length = sizeof(substate1_slides) / sizeof(substate1_slides[0]);
      break;
   case substate2:
      p_current_slide_array = substate2_slides;
      array_length = sizeof(substate2_slides) / sizeof(substate2_slides[0]);
      break;
   case substate3:
      p_current_slide_array = substate3_slides;
      array_length = sizeof(substate3_slides) / sizeof(substate3_slides[0]);
      break;
   case substate4:
      p_current_slide_array = substate4_slides;
      array_length = sizeof(substate4_slides) / sizeof(substate4_slides[0]);
      break;
   default:
      break;
   }

   //Return the total number of slides in the current array
   tmp_slide_info[1] = array_length;

   uint8_t tmp_current_slide = states_get_current_slide_number();
   tmp_slide_info[0] = sd_get_wav_size((p_current_slide_array + tmp_current_slide)->audio); //Return the total audio size in blocks

   //Update image and audio based on the current slide
   states_set_current_audio(address_buffer[(p_current_slide_array + tmp_current_slide)->audio]);
   lcd_image_from_sd(0, SB_OFFSET, 320, MENU3_UTILITIES_BAR_OFFSET, address_buffer[(p_current_slide_array + tmp_current_slide)->image]);
   lcd_draw_rectangle(BLACK, 0, (MENU3_UTILITIES_BAR_OFFSET - 1), 320, MENU3_UTILITIES_BAR_OFFSET); //Black bar at top of slide menu
}



/*!
* @brief This app lets the user view a sample of the author's language skills
* @param[in] NONE
* @return NONE
*
*/
void
states_app_languages(void)
{
   static e_substate previous_substate = substate0 ;
   e_substate tmp_current_substate = states_get_substate();

   //Only redraw the menu if a main state transition or a substate transition has
   //occurred. Not constantly writing helps with LCD flicker.
   if((language_state != states_get_main_state()) ||
      (tmp_current_substate != previous_substate) ||
      (restore_context_call == states_get_main_event()))
   {
      //Substate0 is the only one with the main language menu, all others are type 3
      if(substate0 == tmp_current_substate)
      {
         gui_create_menu_languages(address_buffer[languages_main_screen]);
      }

      else
      {
         gui_create_menu_type3();
      }
   }

   if(language_state == states_get_main_state())
   {
      states_languages_update_menu3();
   }


   previous_substate = states_get_substate();
   states_languages_substate_handler();
   states_set_main_state(language_state);
}


/*!
* @brief Uses the current substate and button information to transition to
*        the correct next sub state.
* @param[in] NONE
* @return NONE
*
*/
void
states_languages_substate_handler(void)
{
   uint8_t active_button = buttons_status(languages_menu_template,(sizeof(languages_menu_template) / sizeof(*languages_menu_template)));
   e_substate tmp_current_substate = states_get_substate();

   if(language_state == states_get_main_state())
   {
      //Service substate 0 (The main list) buttons
      if((substate0 == tmp_current_substate) && (NO_BUTTON_PRESS != active_button))
      {
         static const e_substate transition_table[] ={substate1, substate2};
         states_set_substate(transition_table[active_button]);
      }

      //If the back button was pressed, go back to the main list and reset the slide number
      else
      {
         active_button = buttons_status(user_button_template,(sizeof(user_button_template) / sizeof(*user_button_template)));
         if(back_button == active_button)
         {
            states_set_substate(substate0);
         }
      }

   }

}


/*!
* @brief Monitor all menu3's in the language app and update the audio and display
*        attributes as necessary
* @param[in] NONE
* @return NONE
*
*/
void
states_languages_update_menu3(void)
{

   static e_substate tmp_previous_substate = substate0 ;
   e_substate tmp_current_substate = states_get_substate();

   //Substate0 is the only substate without a menu type 3
   if(substate0 < tmp_current_substate)
   {
      uint8_t state_change_flag = 0;

      static uint32_t slide_info[2] = {0}; //Total blocks, total slides

      if((tmp_previous_substate != tmp_current_substate) ||
         (restore_context_call == states_get_main_event()))
      {
         //update slide
         states_languages_update_slide(slide_info);
         state_change_flag = 1;
      }

      gui_update_menu3_clock(slide_info[0], state_change_flag);
      states_menu3_general_button_handler(slide_info[1]);

   }

   tmp_previous_substate = tmp_current_substate;

}


/*!
* @brief hoose the correct audio and image for the current slide
* @param[in] NONE
* @return NONE
*
*/
void
states_languages_update_slide(uint32_t *tmp_slide_info)
{
   //German Slide
   static const t_slide_type substate1_slides[1] =
   {
      {.image = german_image, .audio = german_audio}
   };

   //Spain Slide
   static const t_slide_type substate2_slides[1] =
   {
      {.image = spanish_image, .audio = spanish_audio}
   };

   const t_slide_type *p_current_slide_array = NULL; // Pointer to choose the correct slide array
   e_substate tmp_current_substate = states_get_substate();

   uint8_t array_length = 0;

   switch(tmp_current_substate)
   {
   case substate1:
      p_current_slide_array = substate1_slides;
      array_length = sizeof(substate1_slides) / sizeof(substate1_slides[0]);
      break;
   case substate2:
      p_current_slide_array = substate2_slides;
      array_length = sizeof(substate2_slides) / sizeof(substate2_slides[0]);
      break;
   default:
      break;
   }

   //Return the total number of slides in the current array
   tmp_slide_info[1] = array_length;

   uint8_t tmp_current_slide = states_get_current_slide_number();
   tmp_slide_info[0] = sd_get_wav_size((p_current_slide_array + tmp_current_slide)->audio); //Return the total audio size in blocks

   //Update image and audio based on the current slide
   states_set_current_audio(address_buffer[(p_current_slide_array + tmp_current_slide)->audio]);
   lcd_image_from_sd(0, SB_OFFSET, 320, MENU3_UTILITIES_BAR_OFFSET, address_buffer[(p_current_slide_array + tmp_current_slide)->image]);
   lcd_draw_rectangle(BLACK, 0, (MENU3_UTILITIES_BAR_OFFSET - 1), 320, MENU3_UTILITIES_BAR_OFFSET); //Black bar at top of slide menu
}


/*!
* @brief This app displays an image and plays an audio explaining why the author
*        wants to work for the recipient company
* @param[in] NONE
* @return NONE
*
*/
void
states_app_company(void)
{

   uint32_t audio_size = sd_get_wav_size(company_audio);

   if((why_company_state != states_get_main_state()) || ((restore_context_call == states_get_main_event())))
   {
      gui_create_menu_type3();
      gui_update_menu3_clock(audio_size, 1); // "1" signals that the app is being entered from the homescreen

      states_set_current_audio(address_buffer[company_audio]);
      lcd_image_from_sd(0, SB_OFFSET, 320, MENU3_UTILITIES_BAR_OFFSET, address_buffer[company_image]);
      lcd_draw_rectangle(BLACK, 0, (MENU3_UTILITIES_BAR_OFFSET - 1), 320, MENU3_UTILITIES_BAR_OFFSET); //Black bar at top of slide menu
      states_set_main_state(why_company_state);
   }

   sd_get_wav_size(address_buffer[company_audio]);
   gui_update_menu3_clock(audio_size, 0);
   states_menu3_general_button_handler(1); //1 Indicates there is only a single slide
}


/*!
* @brief An app giving a brief introductory audio of the device and its features
* @param[in] NONE
* @return NONE
*
*/
void
states_app_intro(void)
{

   e_state_main tmp_main_state = states_get_main_state();
   static uint8_t startup_status_bit = STATES_STARTUP_FLAG_SET;

   if((intro_state != tmp_main_state) || (restore_context_call == states_get_main_event()))
   {
      //If the system is coming from system_init_state, the flag must be clear
      if(system_init_state == tmp_main_state)
      {
         startup_status_bit = STATES_STARTUP_FLAG_CLEAR;
      }

      gui_create_intro_menu(startup_status_bit);
      states_set_main_state(intro_state);
   }

   else
   {
      //Return startup bit with new value
      states_intro_button_handler(&startup_status_bit);
   }
}


/*!
* @brief Takes user input and makes appropriate display changes to the intro menu
* @param[in] tmp_startup_status_flag Flag telling the button handler to display or erase a checkmark
* @return NONE
*
* @warning This function writes to the same block of the SD card whenever the user toggles the
*          "do not show this message on startup" button. For this system under normal circumstances,
*          this should never surpass the maximum FLASH write cycles. If it is used elsewhere, some
*          form of wear leveling should be implemented to avoid burning out the card.
*/
void
states_intro_button_handler(uint8_t *tmp_startup_status_flag)
{

   //Read buttons
   uint8_t active_button = buttons_status(intro_menu_template, (sizeof(intro_menu_template) / sizeof(*intro_menu_template)));

   if(NO_BUTTON_PRESS != active_button)
   {
      uint16_t x_offset = MENU_INTRO_CHECK_X_OFFSET;
      uint16_t y_offset = MENU_INTRO_CHECK_Y_OFFSET;

      switch(active_button)
      {

         case 1:
            //"Listen button
            states_set_current_audio(address_buffer[intro_audio]);
            states_set_current_audio_event(audio_play_event);
            break;

         case 2:
            //"Don't show on startup" button

            //If the user presses this button, toggle the value of the status flag and redraw the button as necessary
            if(*tmp_startup_status_flag)
            {
               *tmp_startup_status_flag = STATES_STARTUP_FLAG_CLEAR; //Toggle local flag status
               states_write_startup_flag(STATES_STARTUP_FLAG_CLEAR); //Toggle flag status in SD card
               lcd_draw_rectangle(GRAY_LIGHT, x_offset, y_offset, x_offset + 20, y_offset + 20); //Redraw button without check mark
            }

            else
            {
               *tmp_startup_status_flag = STATES_STARTUP_FLAG_SET; //Toggle local flag status
               states_write_startup_flag(STATES_STARTUP_FLAG_SET); //Toggle flag status in SD card

               //Redraw button with check mark
               lcd_send_bitmap(check_mark_bmp, MENU_INTRO_CHECK_X_OFFSET, MENU_INTRO_CHECK_Y_OFFSET, GRAY_MEDIUM_DARK, GRAY_LIGHT);
            }
            break;

         default:
            break;
      }
   }
}


/*!
* @brief This app is responsible for handling battery warning events
* @param[in] NONE
* @return NONE
*
*/
void
states_battery_handler(void)
{

   t_button button_list[2] =
   {
      {.background_color = GRAY_MEDIUM_DARK, .body_color = THEME_LIGHT_GRAY, .text_color = RED_PURE,
       .x_position = 25, .y_position = 280, .height = 50, .width = 130, .type = button_ghost},

      {.background_color = GRAY_MEDIUM_DARK, .body_color = THEME_LIGHT_GRAY,  .text_color = THEME_TEXT,
       .x_position = 165, .y_position = 280, .height = 50, .width = 130, .type = button_rounded_corners}
   };

   char *menu_text[5] =
   {
      "Shutdown\0",
      "Dismiss\0",
      "BATTERY LOW\0",
      "Please plug in USB\0",
      "to avoid auto shutdown\0"
   };

   if(battery_handler_state != states_get_main_state())
   {
       gui_create_warning_menu(button_list, menu_text);
       states_set_current_audio_event(audio_end_of_clip_event);
       states_previous_context(CONTEXT_PUSH);

       states_set_main_state(battery_handler_state);
   }

}


/*!
* @brief Present the user with a pop up screen to adjust basic system settings
* @param[in] NONE
* @return NONE
*
*/
void
states_settings_menu(void)
{
   if(settings_state != states_get_main_state())
   {
      //Create the pop-up settings menu and save/end the system context for reentrancy
      gui_create_settings_menu();
      states_set_current_audio_event(audio_end_of_clip_event);
      states_previous_context(CONTEXT_PUSH);

      states_set_substate(0); //Reset substate for the settings button handler
      states_set_main_state(settings_state);
   }


   //Only service the main menu if it is active
   if(substate0 == states_get_substate())
   {
      states_settings_main_button_handler();
   }

   //Otherwise the system is in the "set time" submenu
   else
   {
      states_settings_time_button_handler();
   }

}


/*!
* @brief Changes the system's settings and corresponding gui attributes of the settings menu
*        based on the user's input
* @param[in] NONE
* @return NONE
*
*/
void
states_settings_main_button_handler(void)
{
   e_dac_volume_type tmp_current_volume = dac_get_volume();

   uint8_t active_button = buttons_status(settings_menu_template, (sizeof(settings_menu_template) / sizeof(*settings_menu_template))); //read buttons

   if(NO_BUTTON_PRESS != active_button)
   {
      switch(active_button)
      {
      case 0:
         //Toggle button
         if(volume_low > tmp_current_volume)
         {
            tmp_current_volume = volume_low;
         }

         else
         {
            tmp_current_volume = volume_muted;
         }
         break;

      case 1:
         //Lower volume button
         if(volume_muted < tmp_current_volume)
         {
            tmp_current_volume--;
         }
         break;

      case 2:
         //Raise volume button
         if(volume_high > tmp_current_volume)
         {
            tmp_current_volume++;
         }
         break;

      case 3:
         states_set_substate(substate1);
         break;

      default:
         break;

      }

      dac_set_volume(tmp_current_volume);

      //Shutdown the audio ic if the volume is muted
      if(volume_muted == tmp_current_volume)
      {
         dac_disable_audio();
      }

      else
      {
         dac_enable_audio();
      }

      //Only update the settings menu if the "set time" button has not been pressed,
      // causing the system to transition to substate1
      if(substate0 != states_get_substate())
      {
         gui_create_settings_time_menu();

      }

      else
      {
         gui_update_settings_menu();
      }

   }

}


/*!
* @brief Update the user buttons and input time for the "set time" settings submenu
* @param[in] NONE
* @return NONE
*
*/
void
states_settings_time_button_handler(void)
{
   //Define button text for the "set time" submenu
   uint8_t active_button = buttons_status(settings_time_menu_template,
                                         (sizeof(settings_time_menu_template) / sizeof(*settings_time_menu_template))); //read buttons

   if(NO_BUTTON_PRESS != active_button)
   {
      static uint8_t user_time_buffer[3] = {12, 0, 0};
      uint8_t converted_time_buffer[4] = {0};

      static uint8_t current_place = 0; //The current digit (aka hours_tens place) being changed by the user
      uint8_t max_buffer[3] ={12, 5, 9}; //Max possible value for each time digit
      uint8_t min_buffer[3] ={1, 0, 0}; //Max possible value for each time digit

      switch(active_button)
      {
      case 0:
         //Cancel
         states_set_substate(substate0); //Go back to the main settings menu
         break;

      case 1:
         //Set
         //Reinitialize RTC to the current user_input time
         rtc_set_time(user_time_buffer[0], ((10*user_time_buffer[1]) + user_time_buffer[2]), 9);
         break;

      case 2:
         //Left Arrow
         //Scroll to edit the previous time place (aka hours_tens)
         if(0 < current_place)
         {
            current_place--;
         }
         break;

      case 3:
         //Right Arrow
         //Scroll to edit the next time place (aka hours_tens), only if it is not on minutes
         if(2 > current_place)
         {
            current_place++;
         }
         break;

      case 4:
         //Down Arrow
         //Decrement the current time place if it is not already at zero
         if(min_buffer[current_place] < user_time_buffer[current_place])
         {
            user_time_buffer[current_place]--;
         }
         break;

      case 5:
         //Up Arrow
         //Increment the current time place if it is not already at its upper limit
         if(max_buffer[current_place] > user_time_buffer[current_place])
         {
            user_time_buffer[current_place]++;
         }
         break;

      default:
         break;

      }


      //Only update the time submenu if the system is currently in it
      if(substate0 != states_get_substate())
      {
         converted_time_buffer[0] = (user_time_buffer[0] / 10); //Split hours by tens and ones place
         converted_time_buffer[1] = (user_time_buffer[0] % 10);
         converted_time_buffer[2] = user_time_buffer[1];
         converted_time_buffer[3] = user_time_buffer[2];
         gui_update_settings_time_menu(converted_time_buffer, current_place);
      }

      else
      {
         gui_create_settings_menu();
      }
   }


   //Check if the back button was pressed
   active_button = buttons_status(user_button_template, (sizeof(user_button_template) / sizeof(*user_button_template)));

   if(back_button == active_button)
   {
      states_set_substate(substate0);
      gui_create_settings_menu();
   }

}


/*!
* @brief Puts the system into an inactive state that is not a full deep sleep
* @param[in] NONE
* @return NONE
*
*@note In this state the screen and all peripherals are powered down. The
*      current context (state, substate, audio etc.) are all retained.
*      This is equivalent to briefly pressing the power button on a modern cell phone
*/
void
states_semi_sleep(void)
{
   if(semi_sleep_state != states_get_main_state())
   {
      lcd_backlight_disable();
      states_set_current_audio_event(audio_end_of_clip_event);
      states_set_main_state(semi_sleep_state);
   }
}



/*!
* @brief Puts the system into a full deep sleep
* @param[in] NONE
* @return NONE
*
*@note In this state the screen and all peripherals are powered down. The
*      current context (state, substate, audio etc.) are not retained.
*/
void
states_sleep(void)
{
   if(sleep_state != states_get_main_state())
   {
      //Display a loading screen while the processor shuts down
      lcd_draw_rectangle(BLACK, 0, SB_OFFSET, 320, FOOTER_OFFSET);
      lcd_print_string("Shutting Down",75,200,WHITE_PURE, BLACK);
      lcd_backlight_enable(); //Turn the screen on in case the system is in semi sleep
      gui_loading_circle_animation(150, 300);

      lcd_backlight_disable(); //Shut off the lcd
      states_set_current_audio_event(audio_end_of_clip_event); //Cut off any audio that may be playing
      dac_disable_audio(); //Put external audio amplifier IC to sleep

      states_set_main_state(sleep_state);
      states_mcu_to_deepsleep(); //Put the processor to sleep
   }

}


/*!
* @brief  Read the current state without touching it directly
* @param[in] NONE
* @return current_main_state
* @note
*
*/
e_state_main
states_get_main_state(void)
{
   return(current_main_state);
}


/*!
* @brief  Read the current event without touching it directly
* @param[in] NONE
* @return current_main_event
* @note
*
*/
e_event_main
states_get_main_event(void)
{
   return(current_main_event);
}


/*!
* @brief  Update the current main state without touching it directly
* @param[in] new_state State to update current state
* @return NONE
* @note
*
*/
void
states_set_main_state(e_state_main new_state)
{
   current_main_state = new_state;
}


/*!
* @brief Update the current main event without touching it directly
* @param[in] new_event Event to update current event
* @return NONE
* @note
*
*/
void
states_set_main_event(e_event_main new_event)
{
   current_main_event = new_event;
}


/*!
* @brief  Return the current submenu state without touching it directly
* @param[in] NONE
* @return current_substate
*
* @note Each "app" aka main state uses this function to determine
*       how to transition their local state machine. It also is the
*       only form of coupling between the main state machine and each
*       individual app state machine.
*/
uint8_t
states_get_substate(void)
{
   return(current_substate);
}


/*!
* @brief Update the current substate without touching it directly
* @param[in] new_state State to update current event
* @return NONE
*
*/
void
states_set_substate(uint8_t new_state)
{
   current_substate = new_state;
}


/*!
* @brief Return the current, active audio event. This is usually given by a
*        a menu option in an app menu, such as pause.
* @param[in] NONE
* @return  current_audio_event
*/
e_audio_event
states_get_current_audio_event(void)
{
   return(current_audio_event);
}


/*!
* @brief Set the current, active audio event. This is usually used by a
*        a menu option in an app menu, such as pause.
* @param[in] tmp_event
* @return  NONE
*/
void
states_set_current_audio_event(e_audio_event tmp_event)
{
   current_audio_event = tmp_event;
}


/*!
* @brief Get the most recent audio clip that is either currently playing or was the last one to play
* @param[in] NONE
* @return  current_audio_state
*/
e_audio_state
states_get_current_audio_state(void)
{
   return(current_audio_state);
}


/*!
* @brief Set the current audio state. This is what action the audio service routine is
*        currently performing, such as "start transmission".
* @param[in] tmp_state
* @return  NONE
*/
void
states_set_current_audio_state(e_audio_state tmp_state)
{
   current_audio_state = tmp_state;
}


/*!
* @brief Transition the audio's state machine based on the current state and events given
*        by the system's various menus.
* @param[in] NONE
* @return  NONE
*/
e_audio_state
states_audio_state_transition_handler(void)
{
   static e_audio_state audio_transition_table[audio_max_state][audio_max_event] =
   {
         /* no event                      play                            pause
          * user_button                   end-of-clip                     slide-change
          * substate-change                                                                  */

         //Idle
         {audio_idle_state,               audio_start_transmission_state, audio_idle_state,
          audio_idle_state,               audio_idle_state,               audio_idle_state,
          audio_substate_change_state},
         //Start transmission
         {audio_playing_state,            audio_playing_state,            audio_paused_state,
          audio_stop_transmission_state,  audio_stop_transmission_state,  audio_stop_transmission_state,
          audio_substate_change_state},
         //Playing
         {audio_playing_state,            audio_playing_state,            audio_paused_state,
          audio_stop_transmission_state,  audio_stop_transmission_state,  audio_stop_transmission_state,
          audio_substate_change_state},
         //Paused
         {audio_paused_state,             audio_playing_state,            audio_paused_state,
          audio_stop_transmission_state,  audio_stop_transmission_state,  audio_stop_transmission_state,
          audio_substate_change_state},
         //Stop Transmission
         {audio_idle_state,               audio_start_transmission_state, audio_idle_state,
          audio_idle_state,               audio_idle_state,               audio_idle_state,
          audio_idle_state},
         //Substate Change
         {audio_idle_state,               audio_idle_state,               audio_idle_state,
          audio_idle_state,               audio_idle_state,               audio_idle_state}

   };


   //Get the next state based on the current audio state and event
   e_audio_state tmp_current_audio_state = states_get_current_audio_state();
   e_audio_event tmp_current_audio_event = states_get_current_audio_event();
   e_audio_state tmp_next_audio_state = audio_transition_table[tmp_current_audio_state][tmp_current_audio_event];
   states_set_current_audio_state(tmp_next_audio_state);

   return(tmp_next_audio_state);
}


/*!
* @brief Default state for the audio state machine while no audio is playing.
* @param[in] NONE
* @return  NONE
*/
void
states_audio_idle(void)
{
   timers_reset_audio_clock();
}


/*!
* @brief This does the initial handshake with the SD card to find the desired audio file
*        then begins streaming it.
* @param[in] NONE
* @return  NONE
*/
uint32_t
states_audio_start_transmission(void)
{
   dac_enable();
   uint32_t tmp_current_audio_clip = states_get_current_audio();
   uint32_t tmp_total_blocks = dac_start_audio_transmission(tmp_current_audio_clip);

   //Enable timer11 to monitor running audio count
   timers_start_audio_clock();
   timers_delay(200);

   return(tmp_total_blocks);
}


/*!
* @brief If an audio clip is playing, load the next SD card block into the DAC's DMA buffer
* @param[in] tmp_total_blocks Number of memory blocks the file takes up in the SD card
* @return  NONE
*/
void
states_audio_playing(uint32_t tmp_total_blocks, uint8_t tmp_early_termination_flag)
{
   dac_service_audio(tmp_total_blocks, tmp_early_termination_flag);
}


/*!
* @brief This stops streaming the current audio but retains its context, meaning the audio system can start
*         back up from the same place if desired.
* @param[in] NONE
* @return  NONE
*
* @warning The latency introduced between calling the dac shutdown and timer11 shutdown
*          will cause them to drift a few clock cycles apart with every pause call. This
*          should not be a problem, as timer11 is generally set for a 1 second overflow.
*/
void
states_audio_pause(void)
{
   dac_pause_transmission();
   timers_pause_audio_clock();
   dac_disable();
}


/*!
* @brief This halts the current audio that is playing. It does not save context, and communication between the
*        SD card and the audio module is stopped.
* @param[in] NONE
* @return  NONE
*/
void
states_audio_stop_transmission(void)
{
   timers_pause_audio_clock();
   timers_reset_audio_clock();
   dac_cutoff_transmission();
}


/*!
* @brief If a system substate change is detected (usually when the user presses a
*        menu button) a brief "click" will be played.
* @param[in] NONE
* @return  NONE
*/
void
states_audio_substate_change(void)
{
   timers_pause_audio_clock();
   timers_reset_audio_clock();
   dac_cutoff_transmission();
   //dac_audio_from_sd(address_buffer[menu_button_audio]);
   states_set_current_audio_event(audio_no_event);
}


/*!
* @brief Get the current slide number of the active menu type 3
* @param[in] NONE
* @return  current_audio_state
*/
uint8_t
states_get_current_slide_number(void)
{
   return(current_slide_number);
}


/*!
* @brief Set the current slide of a menu type 3. This is used to update the
*        audio and images within a menu type 3.
* @param[in] tmp_slide_number
* @return  NONE
*/
void
states_set_current_slide_number(uint8_t tmp_slide_number)
{
   current_slide_number = tmp_slide_number;
}


/*!
* @brief Handles the audio, slide and visual changes when a menu type 3 button is pressed
* @param[in] tmp_total_slides The max number of slides in the current array.
* @return  NONE
*/
void
states_menu3_general_button_handler(uint32_t tmp_total_slides)
{
   uint8_t active_button = buttons_status(menu3_template,(sizeof(menu3_template) / sizeof(*menu3_template)));

   if(menu3_play_pause == active_button)
   {
      e_audio_state tmp_current_audio_state = states_get_current_audio_state();
      if(audio_playing_state == tmp_current_audio_state)
      {
         gui_draw_play_button();
         states_set_current_audio_event(audio_pause_event);
      }

      else
      {
         gui_draw_pause_button();
         states_set_current_audio_event(audio_play_event);
      }
   }

   else if(menu3_left_arrow == active_button)
   {
      lcd_send_bitmap(left_music_arrows_bmp, MENU3_ARROWS_LEFT_X_OFFSET, MENU3_ARROWS_Y_OFFSET, THEME_LIGHT_GRAY, BLACK);

      //Decrement current slide if the system is not in slide 0
      uint8_t tmp_slide = states_get_current_slide_number();
      if(0 != tmp_slide)
      {
         states_set_current_slide_number(--tmp_slide);
      }

   }

   else if(menu3_right_arrow == active_button)
   {
      lcd_send_bitmap(right_music_arrows_bmp, MENU3_ARROWS_RIGHT_X_OFFSET, MENU3_ARROWS_Y_OFFSET, THEME_LIGHT_GRAY, BLACK);

      //Increment the slide if the system is not in the last slide of the array
      uint8_t tmp_slide = states_get_current_slide_number();
      if((tmp_total_slides - 1) != tmp_slide) //-1 is necessary to avoid off-by-one error
      {
         states_set_current_slide_number(++tmp_slide);
      }

   }

   else if(menu3_repeat == active_button)
   {
      states_set_current_audio_event(audio_end_of_clip_event);

   }

   if(audio_stop_transmission_state == states_get_current_audio_state())
   {
      gui_draw_play_button();
   }

}


/*!
* @brief Pushes or pops the current/previous state, substate, and slide
* @param[in] action_select Either CONTEXT_PUSH or CONTEXT_POP, used to determine how the function should respond
* @return  previous_context Previous system context upon entering a given function
*/
void
states_previous_context(uint8_t action_select)
{
   static uint32_t context[3] = {0};

   if(CONTEXT_PUSH == action_select)
   {
      context[0] = states_get_main_state();            // Main state
      context[1] = states_get_substate();              // Substate
      context[2] = states_get_current_slide_number();  // Slide
   }

   else if (CONTEXT_POP == action_select)
   {
      states_set_main_state(context[0]);            // Main state
      states_set_substate(context[1]);              // Substate
      states_set_current_slide_number(context[2]);  // Slide
   }

   else
   {
      uart1_printf("\n \r WARNING states_previous_context() called with illegal request\0");
   }

}



/*!
* @brief Puts the MCU into a deep sleep (standby mode) to reduce power consumption
* @param[in] NONE
* @return  NONE
*/
void
states_mcu_to_deepsleep(void)
{
   RCC->APB1ENR |= RCC_APB1ENR_PWREN;

   //When processor receives the sleep instruction, it will enter deep sleep
   SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

   //Go to standby mode when entering deep sleep
   PWR->CR |= PWR_CR_PDDS;

   //Enable wakeup from external wakeup pin 1
   PWR->CSR |= PWR_CSR_EWUP1;

   //Give at least 2 clock cycles before wakeup pin is active
   timers_delay(100);

   //Clear wakeup flag, so the processor doesn't immediately wake up if the flag is active
   PWR->CR |=  PWR_CR_CWUF;

   //Assembly instruction to send the processor to standby mode
   __WFI();
}


/*!
* @brief Reads a flag from the SD card indicating whether to display the introduction popup at startup
* @param[in] NONE
* @return  tmp_buffer[0] This is the first byte of the SD card block and indicates the flag's status
*
* @note This function is slow and puts a large array on the stack. These aren't a concern for this
*       system, but should not be used where latency or RAM usage is a problem.
*/
uint8_t
states_read_startup_flag(void)
{
   uint8_t tmp_buffer[512] = {0};
   sd_read_block(tmp_buffer, STATES_ADDRESS_STARTUP_FLAG);

   return(tmp_buffer[0]);
}


/*!
* @brief Sets or clears a flag from the SD card indicating whether to display the introduction popup at startup
* @param[in] startup_flag_status Desired state to set the flag to
* @return NONE
*
* * @note This function is slow and puts a large array on the stack. These aren't a concern for this
*       system, but should not be used where latency or RAM usage is a problem.
*/
void
states_write_startup_flag(uint8_t startup_flag_status)
{
   //Bounds check to make sure the write value is a valid status
   if(STATES_STARTUP_FLAG_SET >= startup_flag_status)
   {
      uint8_t tmp_buffer[512] = {startup_flag_status};
      sd_write_block(tmp_buffer, STATES_ADDRESS_STARTUP_FLAG);
   }

}



#pragma GCC pop_options

/* end of file */
