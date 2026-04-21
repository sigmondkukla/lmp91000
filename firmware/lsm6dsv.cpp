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
   
   uint8_t whoami = read_reg(WHOAMI);
   if (whoami != 0x70){
      // failed to initialize
      printf("DID NOT GET CORRECT WHOAMI: ");
      printf("%x", whoami); printf("\r\n");
   }
    
   // set SW_RESET
   write_reg(CTR3, 0x01);
   sl_sleeptimer_delay_millisecond(100); // wait for reset to complete

   // set BDU
   write_reg(CTR3, 0x40);

   // configure accelerometer ODR
   write_reg(CTR1, 0x77);

   // configure gyro ODR
   write_reg(CTR2, 0x77); // 0b01110111 for normal mode (see user guide pages 5, 17, 18)

   if (whoami == 0x70){
      printf("IMU INITIALIZATION LIKELY SUCCESFULL\r\n");
   }


 }

/* ------ getter function implementation ------ */

uint16_t lsm6dsv::read_AccX(void){
   uint8_t tx_buf[3] = { (uint8_t)(0x28 | 0x80), 0x00, 0x00 };
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

   return acc_x;
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