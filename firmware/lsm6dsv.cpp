/*
 * lsm6dsv.cpp
 *
 *  Created on: April 15, 2026
 *      Author: Zachary Thornton
 */

#include "lsm6dsv.h"

#include <cstddef>

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_eusart.h"
#include "sl_sleeptimer.h"

 void lsm6dsv::init(void){
    spi_init();
    sl_sleeptimer_delay_millisecond(10);
    
    if (read_reg(WHOAMI) != 0x70){
        // failed to initialize
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


 }

/* ------ helper function implementation ------*/

 void lsm6dsv::spi_init(void){
    EUSART_SpiInit_TypeDef init_master = EUSART_SPI_MASTER_INIT_DEFAULT_HF;
    init_master.clockMode = eusartClockMode3;

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

 uint8_t lsm6dsv::spi_transfer(uint8_t data){
    // Wait for the TXFL bit of the STATUS register to be set
    while (!(EUSART1->STATUS & EUSART_STATUS_TXFL));

    // Write the data to the TXDATA register
    EUSART1->TXDATA = data;

    // Wait for the RXFL bit of the STATUS register to be set
    while (!(EUSART1->STATUS & EUSART_STATUS_RXFL));

    // Return the data that was written to the RXDATA register
    return EUSART1->RXDATA;
 }

 void lsm6dsv::write_reg(uint8_t reg, uint8_t data){
    spi_cs_assert();
    spi_transfer(reg & ~(RW));
    spi_transfer(data);
    spi_cs_deassert();
 }

 uint8_t lsm6dsv::read_reg(uint8_t reg){
    spi_cs_assert();
    spi_transfer(reg | RW);
    uint8_t data = spi_transfer(0x00);
    spi_cs_deassert();
    return data;
 }