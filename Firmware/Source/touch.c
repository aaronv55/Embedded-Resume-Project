/** @file touch.c
*
* @brief  This file contains drivers for the resistive touchscreen of the LCD
* @author Aaron Vorse
* @date   7/23/2020
* @contact aaron.vorse@embeddedresume.com
*
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

#include "touch.h"

/*
****************************************************
************* File-Static Variables ****************
****************************************************
*/
static volatile uint8_t touch_detection = 0;
static uint16_t touch_position[2] = {0};

/*
****************************************************
********** Private Function Prototypes *************
****************************************************
*/
void adc_gpio_init(void);

/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/

/*!
* @brief Set up adc for multichannel readings
* @param[in] NONE
* @return  NONE
*/
void
touch_adc_init(void)
{
   //Init X+ and Y+
   adc_gpio_init();
   
   //Enable ADC clock
   RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
   
   //ADC clock prescaler /4
   ADC1_COMMON->CCR |= ADC_CCR_ADCPRE_DIV4;
   
   //Set ADC to scan through multiple channels
   ADC1->CR1 |= ADC_CR1_SCAN;
   
   //Set end of conversion flag at the end of each channel conversion
   ADC1->CR2 |= ADC_CR2_EOCS;
   
   //Set sample wait states
   ADC1->SMPR2 |= (ADC_SMPR2_SMP1_15_CYCLES | ADC_SMPR2_SMP2_15_CYCLES);
   
   //Sample two different channels
   ADC1->SQR1 |= (ADC_SQR1_2_CONVERSIONS);
   
   //Set scan order to scan PA1, then PA2
   ADC1->SQR3 |= (ADC_SQR3_SQ1_CHANNEL1 | ADC_SQR3_SQ2_CHANNEL2);
   
   //Turn ADC on
   adc_enable();
   
   touch_idle_state();
}


/*!
* @brief Sample channel 1 and 2 from the ADC
* @param[in] NONE
* @return  NONE
*/
void
touch_adc_xy_sample(uint16_t *tmp_adc_samples)
{
     //Start conversion
     ADC1->CR2 |= ADC_CR2_SWSTART;
     
     //Poll for end of conversion
     while(!((ADC1->SR & ADC_SR_EOC) >> ADC_SR_EOC_Pos));
     
     //Get data for channel 0 
     tmp_adc_samples[0] = ADC1->DR;
     
     //Clear status flags
     ADC1->SR = 0;

     while(!((ADC1->SR & ADC_SR_EOC) >> ADC_SR_EOC_Pos));
     
     //Get data for channel 1 
     tmp_adc_samples[1] = ADC1->DR;
     
     //Clear status flags
     ADC1->SR = 0;
}


/*!
* @brief Sample Y_PLUS and return the corresponding x position
* @param[in] NONE
* @return  NONE
*/
uint16_t 
touch_read_x_position(void)
{
   //Disable ADC to change settings
   adc_disable();
   
   //Sample a single channel Y+
   ADC1->SQR1 &= ~(0xFF << 20);

   //Set scan order to scan PA2 (Y+)
   ADC1->SQR3 &= ~0x1F; //Clear register
   ADC1->SQR3 |= (ADC_SQR3_SQ1_CHANNEL2);

   //set gpios to read x position
   gpio_func_init(TOUCH_Y_PLUS, GPIO_MODER_ANALOG);//Set pins to analog mode
   gpio_pupd_init(TOUCH_Y_PLUS,GPIO_PUPDR_NONE);//No pull up/pull down
   gpio_speed_init(TOUCH_Y_PLUS,GPIO_OSPEEDR_LOW);//Low speed
   
   gpio_gen_output_init(TOUCH_X_PLUS);
   gpio_gen_output_init(TOUCH_X_MINUS);
   gpio_gen_input_init(TOUCH_Y_MINUS); //high z
   gpio_set(TOUCH_X_PLUS); //X+ = 3.3v
   gpio_clear(TOUCH_X_MINUS); //X- = 0v
  
   //Enable ADC after settings change
   adc_enable();
   
   //Start conversion
   ADC1->CR2 |= ADC_CR2_SWSTART;
     
   //Poll for end of conversion
   while(!((ADC1->SR & ADC_SR_EOC) >> ADC_SR_EOC_Pos));
     
   //Get data for channel 0
   volatile uint16_t tmp_sample_value = 0;
   tmp_sample_value = ADC1->DR;
     
   //Clear status flags
   ADC1->SR = 0;
   
   //Disable ADC to change settings
   adc_disable();
     
   return(tmp_sample_value);
}


/*!
* @brief Sample X_PLUS and return the corresponding y position
* @param[in] NONE
* @return  tmp_sample_value Y value from ADC
*/
uint16_t 
touch_read_y_position(void)
{
   //Disable ADC to change settings
   adc_disable();
   
   //Sample a single channel X+
   ADC1->SQR1 &= ~(0xFF << 20);

   //Set scan order to scan PA2 (X+)
   ADC1->SQR3 &= ~0x1F; //Clear register
   ADC1->SQR3 |= (ADC_SQR3_SQ1_CHANNEL1);
   
   //set gpios to read y position
   gpio_func_init(TOUCH_X_PLUS, GPIO_MODER_ANALOG);//Set pins to analog mode
   gpio_pupd_init(TOUCH_X_PLUS,GPIO_PUPDR_NONE);//No pull up/pull down
   gpio_speed_init(TOUCH_X_PLUS,GPIO_OSPEEDR_LOW);//Low speed
 
   gpio_gen_output_init(TOUCH_Y_PLUS);
   gpio_gen_output_init(TOUCH_Y_MINUS);
   gpio_gen_input_init(TOUCH_X_MINUS); //high z
   gpio_set(TOUCH_Y_PLUS); //X+ = 3.3v
   gpio_clear(TOUCH_Y_MINUS); //X- = 0v
   
   //Enable ADC after settings change
   adc_enable();
   
   //Start conversion
   ADC1->CR2 |= ADC_CR2_SWSTART;
     
   //Poll for end of conversion
   while(!((ADC1->SR & ADC_SR_EOC) >> ADC_SR_EOC_Pos));

   //Get data for channel 0 
   volatile uint16_t tmp_sample_value = 0;
   tmp_sample_value = ADC1->DR;
     
   //Clear status flags
   ADC1->SR = 0;
   
   //Disable ADC to change settings
   adc_disable();
     
   return(tmp_sample_value);
}


/*!
* @brief Get both x and y position from the LCD
* @param[in] NONE
* @return NONE
* @note This is meant to be used in a multi-threaded system, meaning that it should be
*       called (serviced) every iteration of the containing for{} loop. The position will
*       only update if a touch occurs, triggering the ISR and setting the touch_detection
*       flag. If no touch has occurred, position (0,0) will be returned.
*/
void
touch_update_position(void)
{

   if(1 == touch_detection)
   {
      EXTI -> IMR &= ~EXTI_IMR_IM1; //Mask interrupts from touch_detection pin

      //Take samples
      uint16_t adc_y_sample = touch_read_y_position();
      uint16_t adc_x_sample = touch_read_x_position();

      /*Calculate corresponding position (x,y) from the raw ADC values
       * adc_y_sample are in the range 360-3720. This remaps them to 0 - 480
       * adc_x_sample are in the range 450-3650. This remaps them to 0 - 320 */
      touch_position[0] = (adc_x_sample - 450) / 10;
      touch_position[1] = (480 - ((adc_y_sample - 360) / 7));

      //Reset ISR flag
      touch_detection = 0;

      //Return to idle state and wait for the next touch-sense interrupt
      touch_idle_state();
    }

   else
   {
      //If no touch has occurred, reset the position to (0,0)
      touch_position[0] = 0;
      touch_position[1] = 0;
   }

   //Bounds check, the screen is 320 x 480
   if((321 < touch_position[0]) || (480 < touch_position[1]))
   {
      touch_position[0] = 0;
      touch_position[1] = 0;
   }
}


/*!
* @brief Return both x and y position from the LCD to an input buffer
* @param[in] tmp_position Pointer to two-element array which holds x and y respectively
* @return NONE
*/
void
touch_get_position(uint16_t *tmp_position)
{
   tmp_position[0] = touch_position[0]; //x
   tmp_position[1] = touch_position[1]; //y
}


/*!
* @brief Puts the LCD in idle state, which waits for a user to touch the screen and trigger an interrupt
* @param[in] NONE
* @return  NONE
*/
void
touch_idle_state(void)
{
   //Initialize gpios
   gpio_gen_input_init(TOUCH_X_PLUS); //Interrupt pin
   gpio_gen_output_init(TOUCH_Y_MINUS); //0v
   gpio_gen_input_init(TOUCH_Y_PLUS); //high z
   
   gpio_gen_output_init(TOUCH_X_MINUS); //3.3v pull up
   gpio_type_init(TOUCH_X_MINUS, GPIO_OTYPER_OPEN_DRAIN); //To work correctly with pullup
   gpio_pupd_init(TOUCH_X_MINUS, GPIO_PUPDR_PULL_UP);
   
   //Set X_PLUS as an interrupt
	EXTI -> IMR |= EXTI_IMR_IM1; //Unmask interrupt pin 1

   //Set pins to logic state
   gpio_clear(TOUCH_Y_MINUS);
   gpio_set(TOUCH_X_MINUS);
}


/*!
* @brief ISR that is called when PA1 experiences a falling edge (the screen is touched)
* @param[in] NONE
* @return  NONE
*/
void
EXTI1_IRQHandler(void) {
	
   touch_detection = 1;
   
   //Clear interrupt flag
	EXTI->PR = EXTI_PR_PR1;
}

/*
****************************************************
********** Private Function Definitions ************
****************************************************
*/

/*!
* @brief Initialize LCD_X+, LCD_Y+, and BATTERY_LEVEL pins to read from ADC
* @param[in] NONE
* @return  NONE
*/
void
adc_gpio_init(void)
{
   //Enable GPIOA clock
   gpio_clk_init(TOUCH_X_PLUS);
   
   //Init PA1 (X+) and PA2 (y+)
   gpio_func_init(TOUCH_X_PLUS, GPIO_MODER_ANALOG); //Set pins to analog mode
   gpio_func_init(TOUCH_Y_PLUS, GPIO_MODER_ANALOG);
   
   gpio_pupd_init(TOUCH_X_PLUS,GPIO_PUPDR_NONE); //No pull up/pull down
   gpio_pupd_init(TOUCH_Y_PLUS,GPIO_PUPDR_NONE);
  
   gpio_speed_init(TOUCH_X_PLUS,GPIO_OSPEEDR_LOW); //Low speed
   gpio_speed_init(TOUCH_Y_PLUS,GPIO_OSPEEDR_LOW);
   
   //Init PA4 (Y-) and PA6 (X-)
   gpio_gen_output_init(TOUCH_X_MINUS);
   gpio_gen_input_init(TOUCH_Y_MINUS);
   
   //Set X_PLUS as an interrupt
   SYSCFG -> EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA; //Tie port A to interrupt line 0 
	EXTI ->FTSR |= EXTI_FTSR_TR1; // set trigger pin 0 to falling edge
	NVIC_SetPriority(EXTI1_IRQn,2); //set interrupt priority to 2
	NVIC_EnableIRQ(EXTI1_IRQn); 
   
}

/* end of file */
