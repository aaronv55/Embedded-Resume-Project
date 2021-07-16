/** @file spi.c
*
* @brief  This file contains drivers for SPI, namely SPI2
* @author Aaron Vorse
* @date   7/8/2020
* @contact aaron.vorse@embeddedresume.com
*
* @warning: Timing errors have been seen with this module if compiler optimization levels are set to high. It
*           is recommended that optimization be turned off for this module. This can be done with GCC by adding the following:
*
*           //Very start of file
*           #pragma GCC push_options
*           #pragma GCC optimize ("O0")
*
*           ...
*
*           //Very end of file
*           #pragma GCC pop_options
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

#include "spi.h"

/*
****************************************************
********** Private Function Prototypes *************
****************************************************
*/
void spi_spi2_gpio_init(void);

/*
****************************************************
********** Public Function Definitions *************
****************************************************
*/

/*!
* @brief Initializes spi gpio and transfer settings
* @param[in] NONE
* @return  NONE
*/
void
spi_spi2_init(void)
{ 
   //Disable SPI2 while changing parameters
   SPI2->CR1 &= ~(SPI_CR1_SPE);
   
   //Enable system config clock
   RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
   
   //Enable SPI2 peripheral clock
   RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
   
   //Initialize spi2 GPIOs
   spi_spi2_gpio_init();
   
   //Full Duplex, polarity low, phase is edge1, MSB first, 8-bit
   SPI2->CR1 &= ~(SPI_CR1_RXONLY | SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_LSBFIRST | SPI_CR1_DFF);
   
   //Master, software slave management
   //Warning, if SSI is not set before SPE is enabled, MSTR will reset
   SPI2->CR1 |= (SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI);
   
   //Clock divisor,0x00 = fclk/2, 0x01 = fclk/4, 0x03 = fclk/16, 0x07 = fclk/256
   SPI2->CR1 |= (0x00 << SPI_CR1_BR_Pos); //50MHz/2 = 25MHz

   //SPI mode not I2S
   SPI2->I2SCFGR &= ~(SPI_I2SCFGR_I2SMOD);
   
   //Motorola frame format
   SPI2->CR2 &= ~(SPI_CR2_FRF);
   
    //Enable SPI2 
   SPI2->CR1 |= (SPI_CR1_SPE);
}


/*!
* @brief Sends one byte over SPI2 MOSI
* @param[in] tmp_byte byte to send
* @return  NONE
* @note CS line must be managed outside of this function
*/
void
spi_send_byte(uint8_t tmp_byte)
{
   //Wait until the SPI bus is not in use
   //Warning, this locks the processor, increasing latency.
   //It could be replaced with an interrupt-upon-completion-based scheme */
   while((SPI2->SR & SPI_SR_BSY));
   
   //Send single byte
   SPI2->DR = tmp_byte;
}


/*!
* @brief Receives one byte from SPI2 MISO
* @param[in] dummy_byte (usually 0xFF or 0x00) facilitates transfer from peripheral device
* @return  This returns the contents received in the SPI2 DR register
*/
uint8_t
spi_receive_byte(uint8_t dummy_byte)
{
   //Wait until the SPI bus is not in use
   while(SPI2->SR & SPI_SR_BSY);

   //Send dummy byte
   SPI2->DR = dummy_byte;
   
   /* Wait for the receive register to fill
      @warning This locks the processor, increasing latency.
      It could be replaced with an interrupt-upon-completion-based scheme */
  while(!(SPI2->SR & SPI_SR_RXNE))
  {
  }
   
   return((uint8_t)(SPI2->DR));
}

/*
****************************************************
********** Private Function Definitions ************
****************************************************
*/


/*!
* @brief Initializes SPI2's gpio 
* @param[in] NONE
* @return  NONE
* @note SPI2_CS is software controlled, not hardware controlled
*/
void
spi_spi2_gpio_init(void)
{
   //Enable GPIOC
   gpio_clk_init(SPI2_MOSI); //All SPI2 pins are in PORTC
   
   uint8_t pin_init_list[] = {2,3,7};

   //Initialize SPI2 GPIOs
   for(uint8_t pin_number = 0; pin_number < sizeof(pin_init_list); pin_number++)
   {
      gpio_func_init(GPIOC,pin_init_list[pin_number],GPIO_MODER_ALTERNATE_FUNCTION);
      gpio_type_init(GPIOC,pin_init_list[pin_number],GPIO_OTYPER_PUSH_PULL);
      gpio_speed_init(GPIOC,pin_init_list[pin_number],GPIO_OSPEEDR_VERY_HIGH);//
      gpio_pupd_init(GPIOC,pin_init_list[pin_number],GPIO_PUPDR_NONE);
      GPIOC->AFR[0] &= ~(0x0F << (4 * pin_init_list[pin_number])); //Clear AFRLx[3:0] of current pin
      GPIOC->AFR[0] |= (0x05 << (4 * pin_init_list[pin_number])); //Set current pin to AF5
   }
   
   gpio_gen_output_init(SPI2_CS); //CS is a software NSS, so it is not alternate function
   gpio_speed_init(SPI2_CS, GPIO_OSPEEDR_VERY_HIGH);
}


/* end of file */
