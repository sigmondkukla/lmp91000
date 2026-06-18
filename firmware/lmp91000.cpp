/*
 * lmp91000.cpp
 *
 *  Created on: Mar 26, 2025
 *      Author: Sigmond
 */

#include "lmp91000.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "em_emu.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "em_iadc.h"
#include "em_gpio.h"
#include "em_vdac.h"

#include "sl_sleeptimer.h"

lmp91000::~lmp91000()
{
  // TODO Auto-generated destructor stub
}

void lmp91000::init()
{
  //  initEMU();
  initCMU();
  initI2C();
  initADC();
  initDAC();

  printf("rebooted\n");

  unlock();
}

void lmp91000::initCMU(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_I2C0, true);
  CMU_ClockEnable(cmuClock_IADC0, true);
  CMU_ClockEnable(cmuClock_VDAC0, true);
}

void lmp91000::initI2C(void)
{
  // Use default settings
  I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;
  i2cInit.freq = I2C_FREQ_STANDARD_MAX;

  // Configure GPIO pin for i2c enable (active low)
  GPIO_PinModeSet(menb_port, menb_pin, gpioModePushPull, 1);

  // Using PC4 (SDA) and PC5 (SCL)
  GPIO_PinModeSet(i2c_sda_port, i2c_sda_pin, gpioModeWiredAndPullUpFilter, 1);
  GPIO_PinModeSet(i2c_scl_port, i2c_scl_pin, gpioModeWiredAndPullUpFilter, 1);

  // Route I2C pins to GPIO
  GPIO->I2CROUTE[0].SDAROUTE = (GPIO->I2CROUTE[0].SDAROUTE & ~_GPIO_I2C_SDAROUTE_MASK) |
                               (i2c_sda_port << _GPIO_I2C_SDAROUTE_PORT_SHIFT |
                                (i2c_sda_pin << _GPIO_I2C_SDAROUTE_PIN_SHIFT));

  GPIO->I2CROUTE[0].SCLROUTE = (GPIO->I2CROUTE[0].SCLROUTE & ~_GPIO_I2C_SCLROUTE_MASK) |
                               (i2c_scl_port << _GPIO_I2C_SCLROUTE_PORT_SHIFT |
                                (i2c_scl_pin << _GPIO_I2C_SCLROUTE_PIN_SHIFT));

  GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;

  I2C_Init(I2C0, &i2cInit); // Initialize the I2C
}

void lmp91000::initADC(void)
{
  uint32_t clk_src_adc_freq = CMU_ClockFreqGet(cmuClock_IADC0); // Get actual frequency of IADC clock source
  uint32_t clk_adc_freq = 16000000;                             // target conversion clock frequency is 16 MHz

  IADC_Init_t init = IADC_INIT_DEFAULT;
  IADC_AllConfigs_t initAllConfigs = IADC_ALLCONFIGS_DEFAULT;

  // InitSingle for LMP_VOUT
  IADC_InitSingle_t initSingle = IADC_INITSINGLE_DEFAULT;
  IADC_SingleInput_t singleInput = IADC_SINGLEINPUT_DEFAULT;

  // InitScan for BATT_MEAS
  IADC_InitScan_t initScan = IADC_INITSCAN_DEFAULT;
  IADC_ScanTable_t scanTable = IADC_SCANTABLE_DEFAULT;

  // Clock configuration
  init.srcClkPrescale = IADC_calcSrcClkPrescale(IADC0, clk_src_adc_freq, 0);
  initAllConfigs.configs[0].reference = iadcCfgReferenceVddx;
  initAllConfigs.configs[0].vRef = vref; // [mV] TODO: measure actual voltage and update
  initAllConfigs.configs[0].adcClkPrescale = IADC_calcAdcClkPrescale(IADC0, clk_adc_freq, 0, iadcCfgModeNormal, init.srcClkPrescale);

  // Analog bus allocation
  GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AEVEN0_ADC0;    // Port A even for PA00
  GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN0_ADC0; // Port CD even for PD00

  // LMP_VOUT on PA00
  singleInput.posInput = iadcPosInputPortAPin0;
  singleInput.negInput = iadcNegInputGnd;

  // BATT_MEAS on PD00
  scanTable.entries[0].posInput = iadcPosInputPortDPin0;
  scanTable.entries[0].negInput = iadcNegInputGnd;
  scanTable.entries[0].includeInScan = true;

  IADC_init(IADC0, &init, &initAllConfigs);
  IADC_initSingle(IADC0, &initSingle, &singleInput);
  IADC_initScan(IADC0, &initScan, &scanTable);
}

void lmp91000::initDAC(void)
{
  VDAC_Init_TypeDef init = VDAC_INIT_DEFAULT;
  VDAC_InitChannel_TypeDef initChannel = VDAC_INITCHANNEL_DEFAULT;

  init.prescaler = VDAC_PrescaleCalc(VDAC0, 1000000); // get prescaler for 1 MHz VDAC clock
  init.reference = vdacRefAvdd;

  VDAC_Init(VDAC0, &init);
  VDAC_InitChannel(VDAC0, &initChannel, 0); // Using channel 0

  VDAC_Enable(VDAC0, 0, true);

  DAC_write(0); // start with 0 output voltage
}

void lmp91000::enable(bool enabled)
{
  if (enabled)
    GPIO_PinOutClear(menb_port, menb_pin);
  else 
    GPIO_PinOutSet(menb_port, menb_pin);
}

void lmp91000::DAC_write(const uint16_t value)
{
  VDAC_ChannelOutputSet(VDAC0, 0, value); // write to channel 0 DATA register the value to output
  //  printf("DAC write %u\n", value);
}

void lmp91000::write(uint8_t reg, uint8_t value)
{
  printf("LMP91000_write to reg 0x%x value 0x%x\n", reg, value);
  I2C_TransferSeq_TypeDef i2cTransfer;
  I2C_TransferReturn_TypeDef result;
  uint8_t i2c_write_data[2];

  i2cTransfer.addr = LMP91000_I2C_ADDR; // not bit shifted because done already
  i2cTransfer.flags = I2C_FLAG_WRITE_WRITE;

  i2c_write_data[0] = reg;
  i2c_write_data[1] = value;
  i2cTransfer.buf[0].data = i2c_write_data;
  i2cTransfer.buf[0].len = 2;

  // enable this lmp
  enable(true);

  result = I2C_TransferInit(I2C0, &i2cTransfer);
  while (result == i2cTransferInProgress)
  {
    //printf("transfer in progress...\n");
    result = I2C_Transfer(I2C0);
  }
  // return result;

  enable(false);
}

uint8_t lmp91000::read(uint8_t reg)
{
  // Transfer structure
  I2C_TransferSeq_TypeDef i2cTransfer;
  I2C_TransferReturn_TypeDef result;

  // I2C Buffers
  uint8_t i2c_rxBuffer[10]; // maybe it can be smaller

  // Initialize I2C transfer
  i2cTransfer.addr = LMP91000_I2C_ADDR;
  i2cTransfer.flags = I2C_FLAG_WRITE_READ; // must write target address before reading
  i2cTransfer.buf[0].data = &reg;
  i2cTransfer.buf[0].len = 1;
  i2cTransfer.buf[1].data = i2c_rxBuffer;
  i2cTransfer.buf[1].len = 1;

  enable(true);

  result = I2C_TransferInit(I2C0, &i2cTransfer);

  // Read data
  while (result == i2cTransferInProgress)
  {
    //printf("transfer in progress...\n");
    result = I2C_Transfer(I2C0);
  }
  
  enable(false);
  printf("LMP91000_read from reg 0x%x value 0x%x\n", reg, i2c_rxBuffer[0]);
  return i2c_rxBuffer[0];

}

void lmp91000::set_fet_enable(bool enabled)
{
  uint8_t modecn = read(LMP91000_REG_MODECN);

  modecn &= ~(0xF << 3); // clear reserved bits 6:3

  modecn &= ~(1 << 7);                // clear FET_SHORT bit 7
  modecn |= ((enabled ? 1 : 0) << 7); // set bit 7 based on enabled value

  write(LMP91000_REG_MODECN, modecn);
}

void lmp91000::set_gain(uint8_t gain)
{
  uint8_t tiacn = read(LMP91000_REG_TIACN);
  tiacn &= ~(7 << 5);   // clear reserved bits 7:5
  tiacn &= ~(7 << 2);   // clear gain bits 4:2
  tiacn |= (gain << 2); // set gain bits 4:2 with new gain
  write(LMP91000_REG_TIACN, tiacn);
  current_tia_gain = gain;
  printf("set gain to %f\n", TIA_GAIN[gain]);
}

void lmp91000::set_rload(uint8_t load)
{
  uint8_t tiacn = read(LMP91000_REG_TIACN);
  tiacn &= ~(7 << 5); // clear reserved bits 7:5
  tiacn &= ~3;        // clear bits 1:0
  tiacn |= load;      // set bits 1:0 with new load
  write(LMP91000_REG_TIACN, tiacn);
}

// 0 for internal, 1 for external
void lmp91000::set_ref_source(uint8_t source)
{
  uint8_t refcn = read(LMP91000_REG_REFCN);
  refcn &= ~(1 << 7);     // clear 7th bit
  refcn |= (source << 7); // sets 1 or 0 on 7th bit
  write(LMP91000_REG_REFCN, refcn);
}

// 0 - 20%
// 1 - 50%
// 2 - 67%
// 3 - bypassed
// see datasheet page 22
void lmp91000::set_internal_zero(uint8_t internal_zero)
{
  uint8_t refcn = read(LMP91000_REG_REFCN);
  refcn &= ~(3 << 5);            // clear bits 5 and 6
  refcn |= (internal_zero << 5); // sets internal zero on 5 and 6
  write(LMP91000_REG_REFCN, refcn);
  current_tia_zero = internal_zero;
  printf("set internal zero to %f\n", TIA_ZERO[internal_zero]);
}

// mode:
// 0 - 000 deep sleep
// 1 - 001 2-lead
// 2 - 010 Standby
// 3 - 011 3-lead amperometric
// 6 - 110 Temperature TIA OFF
// 7 - 111 Temperature TIA ON (output at vout)
void lmp91000::set_mode(uint8_t mode)
{
  uint8_t modecn = read(LMP91000_REG_MODECN);
  modecn &= ~(0xF << 3); // clear reserved bits 6:3
  modecn &= ~7;          // clear bits 2:0
  modecn |= mode;
  write(LMP91000_REG_MODECN, modecn);
}

// 0 for negative, 1 for positive
void lmp91000::set_bias_sign(uint8_t sign)
{
  //  printf("Set bias sign: %u\n", sign);
  uint8_t refcn = read(LMP91000_REG_REFCN);
  refcn &= ~(1 << 4); // clear bit 4
  refcn |= sign << 4;
  write(LMP91000_REG_REFCN, refcn);
}

// See LMP91000 datasheet page 22
void lmp91000::set_bias_magnitude(uint8_t magnitude)
{
  uint8_t refcn = read(LMP91000_REG_REFCN);
  refcn &= ~(0xF); // clear bits 3:0
  refcn |= magnitude;
  // refcn |= 0x00;
  write(LMP91000_REG_REFCN, refcn);
}

void lmp91000::unlock(bool lock)
{
  write(LMP91000_REG_LOCK, lock ? 1 : 0); // 0 for unlock, 1 for lock
}

void lmp91000::output_voltage(int32_t voltage)
{
  int32_t original_voltage = voltage;
  uint32_t target_magnitude = abs(voltage);

  // Clamp minimum physical capability (1.5V VREF * 1% lowest bias = 15mV)
  if (target_magnitude < 15) {
    target_magnitude = 15;
  }

  uint32_t best_dacVout = LMP91000_MIN_VREF;
  uint8_t best_bias_setting = 1; // Avoid 0% bias if it causes division by zero
  int32_t best_error = 999999;

  // Scan through available hardware bias settings deterministically 
  // Replacing the dangerous 'while' loop
  for (uint8_t b = 1; b < NUM_TIA_BIAS; b++) {
    if (TIA_BIAS[b] <= 0) continue; 

    // Calculate required DAC VREF for this hardware step percentage
    uint32_t calculated_dac = target_magnitude / TIA_BIAS[b];

    // If it fits within your DAC physical limits (e.g., 1.5V to 3.3V)
    if (calculated_dac >= LMP91000_MIN_VREF && calculated_dac <= 3300) {
      int32_t actual_achieved = calculated_dac * TIA_BIAS[b];
      int32_t error = abs((int32_t)target_magnitude - actual_achieved);

      // Keep the setting that gets closest to target voltage
      if (error < best_error) {
        best_error = error;
        best_dacVout = calculated_dac;
        best_bias_setting = b;
      }
    }
  }

  // Handle your escape hatch scenario safely if voltage is completely out of normal range
  if (target_magnitude > (3300 * 0.24)) {
    best_bias_setting = 0; 
    best_dacVout = LMP91000_MIN_VREF;
  }

  // FORCE write registers if the target changes, ensuring guards never lock up
  bool current_bias_sign = (original_voltage >= 0);

  // the following commeted code logic worked perfectly for CA, it doesn't work for CV because the voltage is always changing
  // if (previousVoltage == 9999 || previousVoltage != original_voltage) {
  //   printf("Changing bias sign to %u\n", current_sign ? 1 : 0);
  //   set_bias_sign(current_sign);
  // }

  if (previous_bias_sign == -1 || previous_bias_sign != current_bias_sign) {
    printf("Changing bias sign to %u\n", current_bias_sign ? 1 : 0);
    set_bias_sign(current_bias_sign);
    previous_bias_sign = current_bias_sign;
  }

  if (previousBias == 9999 || previousBias != best_bias_setting) {
    printf("Changing bias magnitude to %u\n", best_bias_setting);
    set_bias_magnitude(best_bias_setting);
  }

  // Write out to the actual DAC hardware
  DAC_write(get_vdac_value(best_dacVout));

  // Save the state cleanly
  previous_bias_sign = current_bias_sign;
  previousVoltage = original_voltage;
  previousBias = best_bias_setting;
}

void lmp91000::output_voltage_static_bias(int32_t voltage, int32_t user_max_target_voltage)
{
  int32_t original_voltage = voltage;
  uint32_t target_magnitude = abs(voltage);
  uint32_t max_magnitude = abs(user_max_target_voltage);

  // Clamp absolute minimum capability (1.5V VREF * 1% lowest bias = 15mV)
  if (target_magnitude < 15) {
    target_magnitude = 15;
  }
  if (max_magnitude < 15) {
    max_magnitude = 15;
  }

  // Static tracking variable to freeze the hardware configuration mid-sweep
  static uint8_t locked_bias_setting = 0;

  // 1. DYNAMIC LOCK-IN PHASE: Computes the best hardware window at the start of the CV/DPV/SWV
  if (previousBias == 127) {
    uint32_t best_dacVout = LMP91000_MIN_VREF;
    uint32_t best_error = 999999;
    locked_bias_setting = 1; // Fallback to 1%

    /* * We iterate through the available hardware steps to find a percentage index 'b' 
     * where the user's maximum requested voltage can be safely generated by the VDAC.
     * * EFR32 VDAC range: LMP91000_MIN_VREF (usually ~1500mV) up to 3300mV.
     */
    for (uint8_t b = 1; b < NUM_TIA_BIAS; b++) {
      if (TIA_BIAS[b] <= 0) continue;

      // Calculate what VDAC needs to output to hit the peak experiment voltage at this hardware step
      uint32_t calculated_dac_at_max = (uint32_t)((float)max_magnitude / TIA_BIAS[b]);

      // If the required VDAC voltage fits within the EFR32's hardware rail limits
      if (calculated_dac_at_max >= LMP91000_MIN_VREF && calculated_dac_at_max <= 3300) {
        float actual_achieved_at_max = (float)calculated_dac_at_max * TIA_BIAS[b];
        uint32_t error = abs((int32_t)max_magnitude - (int32_t)actual_achieved_at_max);

        /*
         * Tie-Breaker Logic: We favor smaller bias percentages for lower voltages 
         * because it increases VDAC resolution. For larger voltages, the loop automatically 
         * scales up to higher bias settings (e.g., 12%, 18%, 24%) to prevent clipping.
         */
        if (error < best_error) {
          best_error = error;
          locked_bias_setting = b;
        }
      }
    }

    // Hardware Limit Safety Check: 3300mV VDAC * 24% max hardware bridge = 792mV single polarity.
    // Differential sweeps using the sign bit can span double that overall window.
    if (max_magnitude > (3300 * 0.24)) {
      // Out of bounds for normal scaling; clamp to the maximum hardware scaling step (24%)
      locked_bias_setting = 13; 
    }

    printf("[AUTO-SCALE] Selected static hardware bias magnitude index: %u (%.2f%%) for max target %ld mV\n", 
           locked_bias_setting, TIA_BIAS[locked_bias_setting] * 100.0, user_max_target_voltage);
    
    set_bias_magnitude(locked_bias_setting);
    previousBias = locked_bias_setting;
  }

  // 2. SMOOTH SWEEP PHASE: Use the optimized frozen bias setting to map out VDAC outputs
  uint32_t best_dacVout = LMP91000_MIN_VREF;
  if (TIA_BIAS[locked_bias_setting] > 0) {
    best_dacVout = (uint32_t)((float)target_magnitude / TIA_BIAS[locked_bias_setting]);
  }

  // Ensure VDAC never tries to over/under-drive microcontroller pins
  if (best_dacVout < LMP91000_MIN_VREF) {
    best_dacVout = LMP91000_MIN_VREF;
  } else if (best_dacVout > 3300) {
    best_dacVout = 3300;
  }

  // Handle cross-zero polarity shifts (sign transitions) seamlessly
  bool current_bias_sign = (original_voltage >= 0);

  if (previous_bias_sign == -1 || previous_bias_sign != current_bias_sign) {
    printf("Changing bias sign to %u\n", current_bias_sign ? 1 : 0);
    set_bias_sign(current_bias_sign);
    previous_bias_sign = current_bias_sign;
  }

  // 3. WRITE TO THE EFR32 VDAC (Zero register modifications to the LMP91000 = zero transient noise)
  DAC_write(get_vdac_value(best_dacVout));

  // Save state tracking cleanly
  previousVoltage = original_voltage;
}

uint32_t lmp91000::get_vdac_value(uint32_t mv)
{
  return mv * 0xFFF / vref; // 12-bit instead of 16-bit DAC, and vref is in mV so it cancels out factor of 1000
}

uint32_t lmp91000::sample_adc(void)
{
  IADC_command(IADC0, iadcCmdStartSingle); // start single queue conversion (LMP_VOUT)

  while ((IADC_getStatus(IADC0) & IADC_STATUS_SINGLEFIFODV) == 0)
  {
    // wait for conversion to complete
  }

  IADC_Result_t sample = IADC_pullSingleFifoResult(IADC0);
  uint32_t single_result = sample.data * vref / 0xFFF;

  return single_result;
}

float lmp91000::get_current(void)
{
  uint32_t adc_voltage_mv = sample_adc();
  
  // 1. Convert millivolt numbers strictly into Volts (float)
  float v_adc = (float)adc_voltage_mv / 1000.0f;
  float v_ref_volts = (float)vref / 1000.0f;
  
  // 2. Compute the precise zero-current offset baseline in Volts
  float v_zero = v_ref_volts * TIA_ZERO[current_tia_zero];
  
  // 3. (Volts - Volts) / Ohms = Amperes
  float current_amperes = (v_adc - v_zero) / (TIA_GAIN[current_tia_gain]);
  
  // return current_amperes;
  return adc_voltage_mv; // for debugging
}


void lmp91000::set_outputs_to_zero(void)
{
  set_bias_magnitude(0); // return bias to 100% of vref
  set_bias_sign(1);      // positive bias sign
  DAC_write(0);
}

void lmp91000::Reset_Previous_Values(void) {
  previousVoltage = 9999;
  previousBias = 127;
  previous_bias_sign = -1;
}