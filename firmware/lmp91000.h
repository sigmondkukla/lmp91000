/*
 * lmp91000.h
 *
 *  Created on: Mar 26, 2025
 *      Author: Sigmond
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "em_emu.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "em_adc.h"
#include "em_gpio.h"

#include "sl_sleeptimer.h"

#ifndef LMP91000_LMP91000_H_
#define LMP91000_LMP91000_H_

#define LMP91000_ADC_FREQ 16000000

// LMP91000 registers
#define LMP91000_REG_STATUS 0x00 // Status register
#define LMP91000_REG_LOCK 0x01 // Lock register
// 0x02 to 0x09 reserved
#define LMP91000_REG_TIACN 0x10 // TIA Control Register
#define LMP91000_REG_REFCN 0x11 // Reference Control Register
#define LMP91000_REG_MODECN 0x12 // Mode Control Register

#define LMP91000_I2C_ADDR 0x48

#define LMP91000_MIN_VREF 1500 // min input from DAC

// LMP91000 TIA
const double TIA_GAIN[] = {2750,3500,7000,14000,35000,120000,350000}; // ohms
const double TIA_BIAS[] = {0, 0.01, 0.02, 0.04, 0.06, 0.08, 0.1, 0.12, 0.14, 0.16, 0.18, 0.2, 0.22, 0.24}; // bias percentages, 0 means 100
const uint8_t NUM_TIA_BIAS = 14; // length of above array (for iterating)
const double TIA_ZERO[] = {0.2, 0.5, 0.67};

const float v_tolerance = 0.008; // for voltage generation
//const float v_tolerance = 0.01;

class lmp91000
{
public:
  lmp91000 (I2C_TypeDef* i2c,
            GPIO_Port_TypeDef i2c_scl_port,
            uint32_t i2c_scl_pin,
            uint32_t i2c_routeloc0_sclloc,
            GPIO_Port_TypeDef i2c_sda_port,
            uint32_t i2c_sda_pin,
            uint32_t i2c_routeloc0_sdaloc,

            GPIO_Port_TypeDef menb_port,
            unsigned int menb_pin,

            ADC_TypeDef* adc,
            ADC_PosSel_TypeDef pos_sel,
            uint32_t vref
            ):
            i2c(i2c),
            i2c_scl_port(i2c_scl_port),
            i2c_scl_pin(i2c_scl_pin),
            i2c_routeloc0_sclloc(i2c_routeloc0_sclloc),
            i2c_sda_port(i2c_sda_port),
            i2c_sda_pin(i2c_sda_pin),
            i2c_routeloc0_sdaloc(i2c_routeloc0_sdaloc),

            menb_port(menb_port),
            menb_pin(menb_pin),

            adc(adc),
            pos_sel(pos_sel),

            vref(vref)
  {};

  void init();

  virtual ~lmp91000 ();

  void set_fet_enable(bool enabled);
  void set_gain(uint8_t gain);
  void set_rload(uint8_t load);
  void set_ref_source(uint8_t source);
  void set_internal_zero(uint8_t internal_zero);
  void set_mode(uint8_t mode);
  void set_bias_sign(uint8_t sign);
  void set_bias_magnitude(uint8_t magnitude);
  void output_voltage(int32_t voltage);
  void set_outputs_to_zero(void);
  float get_current();

private:
  // init
  void initCMU();
  void initI2C();
  void initADC();
  void initDAC();

  void enable(bool enabled);
  void write(uint8_t reg, uint8_t value);
  uint8_t read(uint8_t reg);

  void unlock(bool lock = false);

  void DAC_write(const uint16_t value);

  uint32_t sample_adc();

//  double LMP91000_get_current(uint32_t adc_voltage);
  uint32_t get_vdac_value(uint32_t mv);
//  int8_t convert_mv_to_c(uint32_t mv);

  I2C_TypeDef* i2c;
  GPIO_Port_TypeDef i2c_scl_port;
  uint32_t i2c_scl_pin;
  uint32_t i2c_routeloc0_sclloc;
  GPIO_Port_TypeDef i2c_sda_port;
  uint32_t i2c_sda_pin;
  uint32_t i2c_routeloc0_sdaloc;

  uint8_t current_tia_zero = 0;
  uint8_t current_tia_gain = 0;

  GPIO_Port_TypeDef menb_port;
  uint32_t menb_pin;

  ADC_TypeDef* adc;
  uint32_t clk_adc_freq = 16000000;
  ADC_PosSel_TypeDef pos_sel;

  uint32_t vref; // [mV]
};

#endif /* LMP91000_LMP91000_H_ */
