/*
 * lsm6dsv.h
 *
 *  Created on: April 15, 2026
 *      Author: Zachary Thornton
 */

 // data sheet: https://www.st.com/resource/en/datasheet/lsm6dsv.pdf
 // user guide: https://www.st.com/resource/en/application_note/an5922-lsm6dsv-6axis-imu-with-embedded-sensor-fusion-i3c-oiseis-for-smart-applications-stmicroelectronics.pdf

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_eusart.h"

#ifndef LSM6DSV_LSM6DSV_H
#define LSM6DSV_LSM6DSV_H

// see page 49 of data sheet of spi-accesable register mapping
#define WHOAMI 0x0F
#define CTR1 0x10 // for accelerometer config
#define CTR2 0x11 // for gyroscope config
#define CTR3 0x12 // for software reset

// read-write bit
#define RW 0x80

// #define BAUDRATE 1000000 // 1 Mhz

class lsm6dsv
{
    public:
        lsm6dsv(USART_TypeDef *spi_inst,

                GPIO_Port_TypeDef gpio_MOSI_port, uint8_t gpio_MOSI_pin,
                GPIO_Port_TypeDef gpio_MISO_port, uint8_t gpio_MISO_pin,
                GPIO_Port_TypeDef gpio_SCLK_port, uint8_t gpio_SCLK_pin,
                GPIO_Port_TypeDef gpio_CS_port,   uint8_t gpio_CS_pin,

                uint32_t baudrate) : spi_inst(spi_inst),
                                     gpio_MOSI_port(gpio_MOSI_port), gpio_MOSI_pin(gpio_MOSI_pin),
                                     gpio_MISO_port(gpio_MISO_port), gpio_MISO_pin(gpio_MISO_pin),
                                     gpio_SCLK_port(gpio_SCLK_port), gpio_SCLK_pin(gpio_SCLK_pin),
                                     gpio_CS_port(gpio_SCLK_port),   gpio_CS_pin(gpio_SCLK_pin),

                                     baudrate(baudrate) {};
        ~lsm6dsv();

        void init(void);


        

    private:
        // variables
        USART_TypeDef *spi_inst;

        GPIO_Port_TypeDef gpio_MOSI_port;
        uint8_t gpio_MOSI_pin;

        GPIO_Port_TypeDef gpio_MISO_port;
        uint8_t gpio_MISO_pin;

        GPIO_Port_TypeDef gpio_SCLK_port;
        uint8_t gpio_SCLK_pin;

        GPIO_Port_TypeDef gpio_CS_port;
        uint8_t gpio_CS_pin;

        uint32_t baudrate;

        // helpers
        void spi_init(void);
        void spi_cs_assert(void); 
        void spi_cs_deassert(void); 
        uint8_t spi_transfer(uint8_t data);

        void write_reg(uint8_t reg, uint8_t data);
        uint8_t read_reg(uint8_t reg);
        
};

#endif /* LSM6DSV_LSM6DSV_H */