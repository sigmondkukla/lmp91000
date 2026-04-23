/*
 * lsm6dsv.cpp
 *
 *  Created on: April 15, 2026
 *      Author: Zachary Thornton
 */

#include "lsm6dsv.h"

/*TODO: TRY CHANGING TO USART0 SEE https://docs.silabs.com/gecko-platform/4.0/emlib/api/efr32xg24/group-usart */

#include <cstddef>
#include <stdio.h>

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "spidrv.h"
#include "sl_spidrv_instances.h"

void lsm6dsv::init(void){
   spi_init();
   
   // Reset
   write_reg(CTRL3, 0x01); // SW_RESET
   sl_sleeptimer_delay_millisecond(50);

   uint8_t out = read_reg(0x1e);
   printf("%d\r\n", out);

   // Enable BDU and Address Auto-Increment (CRITICAL)
   // 0x44 = 0b01000100 (BDU=1, IF_INC=1)
   write_reg(CTRL3, 0x44);

   // Power up Accelerometer
   // High-Performance mode + 240Hz
   write_reg(CTRL1, 0x46); 

   // Power up Gyroscope
   // High-Performance mode + 240Hz
   write_reg(CTRL2, 0x06);

   // Setup Scales
   write_reg(CTRL8, accel_sense); // Bits 1:0 are FS_XL
   write_reg(CTRL6, 0x20 | gyro_sense);  // Bits 3:0 are FS_G
   
   // Force clean state on unused registers
   write_reg(CTRL4, 0x00);
   write_reg(CTRL5, 0x00);
   write_reg(CTRL9, 0x00); // CTRL9: Ensure all power-down overrides are 0
   
   // Update your sensitivity constants using FLOATS
   accel_mg = accel_sensitivity();
   gyro_dps = gyro_sensitivity();

   // TODO:
   // add selectable perfomance modes (right now it is stuck at 240Hz and high performance)
   // use INT2 with DRDY flags to get data
}

/* ------ getter function implementation ------ */

float lsm6dsv::read_AccX(void){
   uint8_t tx_buf[3] = { (uint8_t)(OUTX_L_A | RW), 0x00, 0x00 };
   uint8_t rx_buf[3] = { 0, 0, 0 };

   // Perform a 3-byte transfer: [Address + RW] -> [Data LSB] -> [Data MSB]
   Ecode_t status = SPIDRV_MTransferB(sl_spidrv_imu_handle, tx_buf, rx_buf, 3);

   if (status != ECODE_EMDRV_SPIDRV_OK) {
      return 0; // Or handle error appropriately
   }

   // rx_buf[0] is garbage (received during address phase)
   // rx_buf[1] is OUTX_L_A
   // rx_buf[2] is OUTX_H_A
   
   // Combine into a signed 16-bit integer
   int16_t acc_x = (int16_t)((rx_buf[2] << 8) | rx_buf[1]);

   return acc_x * accel_mg;
}
float lsm6dsv::read_AccY(void){
   uint8_t tx_buf[3] = { (uint8_t)(OUTY_L_A | RW), 0x00, 0x00 };
   uint8_t rx_buf[3] = { 0, 0, 0 };

   // Perform a 3-byte transfer: [Address + RW] -> [Data LSB] -> [Data MSB]
   Ecode_t status = SPIDRV_MTransferB(sl_spidrv_imu_handle, tx_buf, rx_buf, 3);

   if (status != ECODE_EMDRV_SPIDRV_OK) {
      return 0; // Or handle error appropriately
   }

   // rx_buf[0] is garbage (received during address phase)
   // rx_buf[1] is OUTX_L_A
   // rx_buf[2] is OUTX_H_A
   
   // Combine into a signed 16-bit integer
   int16_t acc_y = (int16_t)((rx_buf[2] << 8) | rx_buf[1]);

   return acc_y * accel_mg;
}
float lsm6dsv::read_AccZ(void){
   uint8_t tx_buf[3] = { (uint8_t)(OUTZ_L_A | RW), 0x00, 0x00 };
   uint8_t rx_buf[3] = { 0, 0, 0 };

   // Perform a 3-byte transfer: [Address + RW] -> [Data LSB] -> [Data MSB]
   Ecode_t status = SPIDRV_MTransferB(sl_spidrv_imu_handle, tx_buf, rx_buf, 3);

   if (status != ECODE_EMDRV_SPIDRV_OK) {
      return 0; // Or handle error appropriately
   }

   // rx_buf[0] is garbage (received during address phase)
   // rx_buf[1] is OUTX_L_A
   // rx_buf[2] is OUTX_H_A
   
   // Combine into a signed 16-bit integer
   int16_t acc_z = (int16_t)((rx_buf[2] << 8) | rx_buf[1]);

   return acc_z * accel_mg;
}

float lsm6dsv::read_GyroX(void){
   uint8_t tx_buf[3] = { (uint8_t)(OUTX_L_G | RW), 0x00, 0x00 };
   uint8_t rx_buf[3] = { 0, 0, 0 };

   // Perform a 3-byte transfer: [Address + RW] -> [Data LSB] -> [Data MSB]
   Ecode_t status = SPIDRV_MTransferB(sl_spidrv_imu_handle, tx_buf, rx_buf, 3);

   if (status != ECODE_EMDRV_SPIDRV_OK) {
      return 0; // Or handle error appropriately
   }

   // rx_buf[0] is garbage (received during address phase)
   // rx_buf[1] is OUTX_L_A
   // rx_buf[2] is OUTX_H_A
   
   // Combine into a signed 16-bit integer
   int16_t gyro_x = (int16_t)((rx_buf[2] << 8) | rx_buf[1]);

   return gyro_x * gyro_dps;
}

float lsm6dsv::read_GyroY(void){
   uint8_t tx_buf[3] = { (uint8_t)(OUTY_L_G | RW), 0x00, 0x00 };
   uint8_t rx_buf[3] = { 0, 0, 0 };

   // Perform a 3-byte transfer: [Address + RW] -> [Data LSB] -> [Data MSB]
   Ecode_t status = SPIDRV_MTransferB(sl_spidrv_imu_handle, tx_buf, rx_buf, 3);

   if (status != ECODE_EMDRV_SPIDRV_OK) {
      return 0; // Or handle error appropriately
   }

   // rx_buf[0] is garbage (received during address phase)
   // rx_buf[1] is OUTX_L_A
   // rx_buf[2] is OUTX_H_A
   
   // Combine into a signed 16-bit integer
   int16_t gyro_y = (int16_t)((rx_buf[2] << 8) | rx_buf[1]);

   return gyro_y * gyro_dps;
}
float lsm6dsv::read_GyroZ(void){
   uint8_t tx_buf[3] = { (uint8_t)(OUTZ_L_G | RW), 0x00, 0x00 };
   uint8_t rx_buf[3] = { 0, 0, 0 };

   // Perform a 3-byte transfer: [Address + RW] -> [Data LSB] -> [Data MSB]
   Ecode_t status = SPIDRV_MTransferB(sl_spidrv_imu_handle, tx_buf, rx_buf, 3);

   if (status != ECODE_EMDRV_SPIDRV_OK) {
      return 0; // Or handle error appropriately
   }

   // rx_buf[0] is garbage (received during address phase)
   // rx_buf[1] is OUTX_L_A
   // rx_buf[2] is OUTX_H_A
   
   // Combine into a signed 16-bit integer
   int16_t gyro_z = (int16_t)((rx_buf[2] << 8) | rx_buf[1]);

   return gyro_z * gyro_dps;
}

/* ------ helper function implementation ------ */
void lsm6dsv::spi_init(void){

   sl_spidrv_init_instances();

   sl_sleeptimer_delay_millisecond(100);
}

uint8_t lsm6dsv::spi_transfer(uint8_t data){
   uint8_t rxbuffer;
   SPIDRV_MTransferB(sl_spidrv_imu_handle, &data, &rxbuffer, 1);
   return rxbuffer;
}

void lsm6dsv::write_reg(uint8_t reg, uint8_t data){
   uint8_t tx_buffer[2];
   
   tx_buffer[0] = reg & ~RW; // set MSB for write
   tx_buffer[1] = data;

   SPIDRV_MTransmitB(sl_spidrv_imu_handle, tx_buffer, 2);
}

uint8_t lsm6dsv::read_reg(uint8_t reg){
   uint8_t tx_buffer[2];
   uint8_t rx_buffer[2];
   
   tx_buffer[0] = reg | RW; // set MSB for read
   tx_buffer[1] = 0x00;     // dummy byte
   
   SPIDRV_MTransferB(sl_spidrv_imu_handle, tx_buffer, rx_buffer, 2);
   
   return rx_buffer[1]; 
}

float lsm6dsv::accel_sensitivity(){
   switch (accel_sense)
   {
   case ACC_SENSE_2G:
      return 0.061f;
      break;
   case ACC_SENSE_4G:
      return 0.122f;
      break;
   case ACC_SENSE_8G:
      return 0.244f;
      break;
   case ACC_SENSE_16G:
      return 0.488f;
      break;
   
   default:
      return 0;
      break;
   }
}

float lsm6dsv::gyro_sensitivity(){
   switch (gyro_sense)
   {
   case GYRO_SENSE_125DPS:
      return 4.375f/1000.0f;
      break;
   case GYRO_SENSE_250DPS:
      return 8.75f/1000.0f;
      break;
   case GYRO_SENSE_500DPS:
      return 17.5f/1000.0f;
      break;
   case GYRO_SENSE_1000DPS:
      return 35.0f/1000.0f;
      break;
   case GYRO_SENSE_2000DPS:
      return 70.0f / 1000.0f;
      break;
   case GYRO_SENSE_4000DPS:
      return 140.0f/1000.0f;
      break;
   default:
      return 0;
      break;
   }
}