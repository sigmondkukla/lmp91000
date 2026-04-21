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
#include "em_usart.h"
#include "spidrv.h"
#include "sl_spidrv_instances.h"

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
        lsm6dsv(){};
        //~lsm6dsv();

        void init(void);

        // getters
        uint16_t read_AccX(void);
        uint16_t read_AccY(void);
        uint16_t read_AccZ(void);

        uint16_t read_GyroX(void);
        uint16_t read_GyroY(void);
        uint16_t read_GyroZ(void);
        
    private:
        // helpers
        void spi_init(void);
        void spi_cs_assert(void); 
        void spi_cs_deassert(void); 
        uint8_t spi_transfer(uint8_t data);

        void write_reg(uint8_t reg, uint8_t data);
        uint8_t read_reg(uint8_t reg);
        
};

#endif /* LSM6DSV_LSM6DSV_H */