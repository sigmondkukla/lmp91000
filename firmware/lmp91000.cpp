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
#include "config/pin_config.h"

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

  // Configure the analog input pins used by the ADC front-end.
  GPIO_PinModeSet(IADC0_POS_PORT, IADC0_POS_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(BATT_MEAS_PORT, BATT_MEAS_PIN, gpioModeInput, 0);

  // Allocate the ADC analog buses for the configured input pins.
  GPIO->ABUSALLOC |= GPIO_ABUSALLOC_AEVEN0_ADC0;    // Port A even for PA00
  GPIO->CDBUSALLOC |= GPIO_CDBUSALLOC_CDEVEN0_ADC0; // Port CD even for PD00

  // LMP_VOUT on the configured ADC input pin.
  singleInput.posInput = pos_input;
  singleInput.negInput = iadcNegInputGnd;

  // BATT_MEAS on PD00.
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
  VDAC_InitChannel(VDAC0, &initChannel, 0);

  // Leave the DAC output pad in analog mode so the VDAC main-out path is not driven digitally.
  GPIO_PinModeSet(VDAC0_CH0_MAIN_OUT_PORT, VDAC0_CH0_MAIN_OUT_PIN, gpioModeDisabled, 0);

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
  // Allow a small settling time for the DAC output to stabilise
  for (volatile uint32_t i = 0; i < 1000; ++i)
    ;
}

void lmp91000::write(uint8_t reg, uint8_t value)
{
  //printf("LMP91000_write to reg 0x%x value 0x%x\n", reg, value);
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
    result = I2C_Transfer(I2C0);
  }

  enable(false);

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
  //    printf("Requested %ld mV\n", voltage);
  // Minimum DAC voltage that can be set
  // The LMP91000 accepts a minium value of 1.5V, adding the
  // additional 20 mV for the sake of a bit of a buffer
  //    const uint32_t minDACVoltage = 1500;

  uint32_t dacVout = LMP91000_MIN_VREF;
  uint8_t bias_setting = 0;

  // voltage cannot be set to less than 15mV because the LMP91000
  // accepts a minium of 1.5V at its VREF pin and has 1% as its
  // lowest bias option 1.5V*1% = 15mV
  if (voltage == 0)
  {
    voltage = 15; // clamp 0 to minimum 15mV
  }
  else if (abs(voltage) < 15)
  {
    voltage = 15 * (voltage / abs(voltage)); // clamp voltage to 15mV min preserving sign
  }

  // Allows setting voltage above 792mV
  if (abs(voltage) > 3300 * 0.24)
  {
    set_bias_sign(voltage >= 0); // will write 0 for neg and 1 for pos
    set_bias_magnitude(0);
    //        VDAC_ChannelOutputSet(vdac, vdac_channel, get_vdac_value(dacVout));
    DAC_write(get_vdac_value(dacVout));
    return;
  }

  int32_t setV = dacVout * TIA_BIAS[bias_setting];
  int32_t original_voltage = voltage;
  voltage = abs(voltage);

  //    printf("setV: %ld\n", setV);

  while (setV > voltage * (1 + v_tolerance) || setV < voltage * (1 - v_tolerance)) // while setV is out of tolerance
  {
    //      printf("setV: %ld\n", setV);
    if (bias_setting == 0)
      bias_setting = 1;

    dacVout = voltage / TIA_BIAS[bias_setting];

    if (dacVout > 3300)
    {
      bias_setting++;
      dacVout = LMP91000_MIN_VREF;

      if (bias_setting > NUM_TIA_BIAS)
        bias_setting = 0;
    }

    setV = dacVout * TIA_BIAS[bias_setting];
  }

  //    printf("Selected bias %d * DAC vout: %lu = Actual: %ld\n", bias_setting, dacVout, setV);

  set_bias_sign(original_voltage >= 0); // will write 0 for neg and 1 for pos
  set_bias_magnitude(bias_setting);
  //    VDAC_ChannelOutputSet(vdac, vdac_channel, get_vdac_value(dacVout));
  DAC_write(get_vdac_value(dacVout));
}

uint32_t lmp91000::get_vdac_value(uint32_t mv)
{
  return mv * 0xFFF / vref; // 12-bit instead of 16-bit DAC, and vref is in mV so it cancels out factor of 1000
}

uint32_t lmp91000::sample_adc(void)
{
  // Drain any stale single-conversion FIFO entries before starting a new sample.
  while ((IADC_getStatus(IADC0) & IADC_STATUS_SINGLEFIFODV) != 0)
  {
    (void)IADC_pullSingleFifoResult(IADC0);
  }

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
  uint32_t adc_voltage = sample_adc();
  float current = (adc_voltage - (vref * TIA_ZERO[current_tia_zero])) / (TIA_GAIN[current_tia_gain]);
  //  printf("get_current current: %.3f\n", current);
  return current;
}

void lmp91000::set_outputs_to_zero(void)
{
  set_bias_magnitude(0); // return bias to 100% of vref
  set_bias_sign(1);      // positive bias sign
  DAC_write(0);
}
