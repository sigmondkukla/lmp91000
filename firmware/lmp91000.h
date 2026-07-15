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
#include "em_iadc.h"
#include "em_vdac.h"
#include "em_gpio.h"

#include "sl_sleeptimer.h"

#ifndef LMP91000_LMP91000_H_
#define LMP91000_LMP91000_H_

// LMP91000 registers
#define LMP91000_REG_STATUS 0x00 // Status register
#define LMP91000_REG_LOCK 0x01   // Lock register
// 0x02 to 0x09 reserved
#define LMP91000_REG_TIACN 0x10  // TIA Control Register
#define LMP91000_REG_REFCN 0x11  // Reference Control Register
#define LMP91000_REG_MODECN 0x12 // Mode Control Register

#define LMP91000_I2C_ADDR 0x90

#define LMP91000_MIN_VREF 1500 // min input from DAC

// LMP91000 TIA
// must have 0 in TIA_GAIN for correct indexing
const double TIA_GAIN[] = {0, 2750, 3500, 7000, 14000, 35000, 120000, 350000};                             // ohms
const double TIA_BIAS[] = {0, 0.01, 0.02, 0.04, 0.06, 0.08, 0.1, 0.12, 0.14, 0.16, 0.18, 0.2, 0.22, 0.24}; // bias percentages, 0 means 100
const uint8_t NUM_TIA_BIAS = 14;                                                                           // length of above array (for iterating)
const double TIA_ZERO[] = {0.2, 0.5, 0.67};

const float v_tolerance = 0.05; // for voltage generation
// const float v_tolerance = 0.01;

class lmp91000
{
public:
  lmp91000(I2C_TypeDef *i2c,
           GPIO_Port_TypeDef i2c_scl_port,
           uint32_t i2c_scl_pin,
           GPIO_Port_TypeDef i2c_sda_port,
           uint32_t i2c_sda_pin,

           GPIO_Port_TypeDef menb_port,
           unsigned int menb_pin,

           IADC_TypeDef *iadc,
           IADC_PosInput_t pos_input,
           uint32_t vref) : i2c(i2c),
                            i2c_scl_port(i2c_scl_port),
                            i2c_scl_pin(i2c_scl_pin),
                            i2c_sda_port(i2c_sda_port),
                            i2c_sda_pin(i2c_sda_pin),

                            menb_port(menb_port),
                            menb_pin(menb_pin),

                            iadc(iadc),
                            pos_input(pos_input),

                            vref(vref) {};
  

  void init();

  virtual ~lmp91000();

  void set_fet_enable(bool enabled);
  void set_gain(uint8_t gain);
  void set_rload(uint8_t load);
  void set_ref_source(uint8_t source);
  void set_internal_zero(uint8_t internal_zero);
  void set_mode(uint8_t mode);
  void set_bias_sign(uint8_t sign);
  void set_bias_magnitude(uint8_t magnitude);
  void output_voltage(int32_t voltage); // use this function for static experiments such as CA
  void output_voltage_static_bias(int32_t voltage, int32_t user_max_target_voltage); // use this function during constinuously changing experiments such as CV
  void output_voltage_test(void);
  void set_outputs_to_zero(void);
  void Reset_Previous_Values(void);
  float get_current();
  int32_t get_current_DAC_output_mv(uint32_t);
  uint8_t read(uint8_t reg);

  void unlock(bool lock = false);

  void DAC_write(const uint16_t value);
  uint32_t get_vdac_value(uint32_t mv);

private:
  // init
  void initCMU();
  void initI2C();
  void initADC();
  void initDAC();

  void enable(bool enabled);
  void write(uint8_t reg, uint8_t value);

  // void unlock(bool lock = false);

  // void DAC_write(const uint16_t value);

  uint32_t sample_adc();

  //  double LMP91000_get_current(uint32_t adc_voltage);
  // uint32_t get_vdac_value(uint32_t mv);
  //  int8_t convert_mv_to_c(uint32_t mv);

  I2C_TypeDef *i2c;
  GPIO_Port_TypeDef i2c_scl_port;
  uint32_t i2c_scl_pin;
  GPIO_Port_TypeDef i2c_sda_port;
  uint32_t i2c_sda_pin;

  uint8_t current_tia_zero = 0;
  uint8_t current_tia_gain = 0;

  GPIO_Port_TypeDef menb_port;
  uint32_t menb_pin;

  IADC_TypeDef *iadc = IADC0;
  IADC_PosInput_t pos_input;

  uint32_t vref; // [mV]

  // prevent i2c flooding by only changing bias settings when necessary
  // must be signed integers
  int16_t previousVoltage = 9999;
  int8_t previousBias = 127;

  // this one might be the only one we need?
  int8_t previous_bias_sign = -1;

  // for dynamic Vref calcuation
  int32_t current_DAC_output_mv = 0;

  int32_t previous_dacVout;
};

#endif /* LMP91000_LMP91000_H_ */
