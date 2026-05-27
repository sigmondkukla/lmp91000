/*
 * battery.cpp
 *
 *  Created on: Apr 22, 2026
 *      Author: Matt Whispell
 */
#include "battery.h"

float battery_get_voltage()
{
  IADC_command(IADC0, iadcCmdStartScan);
  IADC_Result_t sample = IADC_pullScanFifoResult(IADC0);
  float bat_voltage = sample.data * 3.3 / 0xFFF * 2; // multiply by 2 because of voltage divider
  return bat_voltage;
}