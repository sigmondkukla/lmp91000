/*
 * lsm6dsv.cpp
 *
 *  Created on: April 15, 2026
 *      Author: Zachary Thornton
 */

#include "lsm6dsv.h"

/*TODO: TRY CHANGING TO USART SEE https://docs.silabs.com/gecko-platform/4.0/emlib/api/efr32xg24/group-usart */

#include <cstddef>
#include <stdio.h>

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_eusart.h"
#include "sl_sleeptimer.h"

 void lsm6dsv::init(void){
    spi_init();
    sl_sleeptimer_delay_millisecond(10);
    
    uint8_t whoami = read_reg(WHOAMI);
    if (whoami != 0x70){
        // failed to initialize
        printf("FAILED TO INITIALIZE\r\n");
        printf("%x", whoami);
        // TODO: add error handling
    }
    
    // set SW_RESET
    write_reg(CTR3, 0x01);
    sl_sleeptimer_delay_millisecond(10); // wait for reset to complete

    // set BDU
    write_reg(CTR3, 0x40);

    // configure accelerometer ODR
    write_reg(CTR1, 0x77);

    // configure gyro ODR
    write_reg(CTR2, 0x77); // 0b01110111 for normal mode (see user guide pages 5, 17, 18)

    if (whoami == 0x70){
      printf("INITIALIZATION LIKELY SUCCESFULL\r\n");
    }


 }

/* ------ getter function implementation ------ */

uint16_t lsm6dsv::read_AccX(void){
   return 1;
}
uint16_t lsm6dsv::read_AccY(void){
   return 1;
}
uint16_t lsm6dsv::read_AccZ(void){
   return 1;
}

uint16_t lsm6dsv::read_GyroX(void){
   return 1;
}
uint16_t lsm6dsv::read_GyroY(void){
   return 1;
}
uint16_t lsm6dsv::read_GyroZ(void){
   return 1;
}

/* ------ helper function implementation ------ */

 void lsm6dsv::spi_init(void){

    EUSART_SpiAdvancedInit_TypeDef adv = EUSART_SPI_ADVANCED_INIT_DEFAULT;
    adv.msbFirst = true; // SPI standard MSB first

    EUSART_SpiInit_TypeDef init_master = EUSART_SPI_MASTER_INIT_DEFAULT_HF;
    init_master.bitRate = baudrate;
    init_master.clockMode = eusartClockMode3;
    init_master.advancedSettings = &adv;

    // Enable clocks
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_EUSART1,true);

    // Configure GPIO pins
    GPIO_PinModeSet(gpio_MOSI_port, gpio_MOSI_pin, gpioModePushPull, 0);
    GPIO_PinModeSet(gpio_MISO_port, gpio_MISO_pin, gpioModeInput,    0);
    GPIO_PinModeSet(gpio_SCLK_port, gpio_SCLK_pin, gpioModePushPull, 0);
    GPIO_PinModeSet(gpio_CS_port,   gpio_CS_pin,   gpioModePushPull, 1); // CS start HIGH, active LOW

    // connect eusart to ports
    GPIO->EUSARTROUTE[EUSART_NUM(EUSART1)].TXROUTE = (gpio_MOSI_port << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
       | (gpio_MOSI_pin << _GPIO_EUSART_TXROUTE_PIN_SHIFT);

    GPIO->EUSARTROUTE[EUSART_NUM(EUSART1)].RXROUTE = (gpio_MISO_port << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
      | (gpio_MISO_pin << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
    
    GPIO->EUSARTROUTE[EUSART_NUM(EUSART1)].SCLKROUTE = (gpio_SCLK_port << _GPIO_EUSART_SCLKROUTE_PORT_SHIFT)
      | (gpio_SCLK_pin << _GPIO_EUSART_SCLKROUTE_PIN_SHIFT);

    GPIO->EUSARTROUTE[EUSART_NUM(EUSART1)].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN | GPIO_EUSART_ROUTEEN_RXPEN | GPIO_EUSART_ROUTEEN_SCLKPEN;
    
    // initialize the eusart
    EUSART_SpiInit(EUSART1, &init_master);
   
 }

 void lsm6dsv::spi_cs_assert(void){
    GPIO_PinOutClear(gpio_CS_port, gpio_CS_pin); // active low
 }

 void lsm6dsv::spi_cs_deassert(void){
    GPIO_PinOutSet(gpio_CS_port, gpio_CS_pin);
 }

 uint16_t lsm6dsv::spi_transfer(uint8_t data){
   //  // Wait for the TXFL bit of the STATUS register to be set
   //  while (!(EUSART1->STATUS & EUSART_STATUS_TXFL));

   //  // Write the data to the TXDATA register
   //  EUSART1->TXDATA = data;

   //  // Wait for the RXFL bit of the STATUS register to be set
   //  while (!(EUSART1->STATUS & EUSART_STATUS_RXFL));

   //  // Return the data that was written to the RXDATA register
   //  return EUSART1->RXDATA;
   return EUSART_Spi_TxRx(EUSART1, data);
 }

 void lsm6dsv::write_reg(uint8_t reg, uint8_t data){
    spi_cs_assert();
    spi_transfer(reg & ~(RW));
    spi_transfer(data);
    spi_cs_deassert();
 }

 uint16_t lsm6dsv::read_reg(uint8_t reg){
    uint16_t data;
    spi_cs_assert();
    spi_transfer(reg | RW);
    data = spi_transfer(0x00);
    spi_cs_deassert();
    return data;
 }