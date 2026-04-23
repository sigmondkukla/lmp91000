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

#define OUTX_L_G 0x22
#define OUTX_H_G 0x23
#define OUTY_L_G 0x24
#define OUTY_H_G 0x25
#define OUTZ_L_G 0x26
#define OUTZ_H_G 0x27

#define OUTX_L_A 0x28
#define OUTX_H_A 0x29
#define OUTY_L_A 0x2a
#define OUTY_H_A 0x2b
#define OUTZ_L_A 0x2c
#define OUTZ_H_A 0x2d

#define CTRL1 0x10 // for accelerometer config
#define CTRL2 0x11 // for gyroscope config
#define CTRL3 0x12 // for software reset
#define CTRL4 0x13 
#define CTRL5 0x14
#define CTRL6 0x15 // for gyro sensitivity
#define CTRL8 0x17 // for acc sensitivity
#define CTRL9 0x18 // power down overides

// read-write bit
#define RW 0x80

#define RESOLUTION 32768 // 2^15
#define ACC_SENSE_2G 0x00
#define ACC_SENSE_4G 0x01
#define ACC_SENSE_8G 0x02
#define ACC_SENSE_16G 0x03
#define GYRO_SENSE_125DPS 0x00
#define GYRO_SENSE_250DPS 0x01
#define GYRO_SENSE_500DPS 0x02
#define GYRO_SENSE_1000DPS 0x03
#define GYRO_SENSE_2000DPS 0x04
#define GYRO_SENSE_4000DPS 0x0c

// #define BAUDRATE 1000000 // 1 Mhz

class lsm6dsv
{
    public:
        lsm6dsv(uint8_t gyro_sense, uint8_t accel_sense): gyro_sense(gyro_sense), accel_sense(accel_sense){};
        //~lsm6dsv();

        void init(void);

        // getters
        float read_AccX(void);
        float read_AccY(void);
        float read_AccZ(void);
        float read_GyroX(void);
        float read_GyroY(void);
        float read_GyroZ(void);
        
    private:
        // variables
        uint8_t gyro_sense;
        uint8_t accel_sense;
        float gyro_dps;
        float accel_mg;

        // helpers
        void spi_init(void);
        void spi_cs_assert(void); 
        void spi_cs_deassert(void); 
        uint8_t spi_transfer(uint8_t data);

        void write_reg(uint8_t reg, uint8_t data);
        uint8_t read_reg(uint8_t reg);
        float gyro_sensitivity();
        float accel_sensitivity();
};

#endif /* LSM6DSV_LSM6DSV_H */