/*
 * battery.cpp
 *
 *  Created on: Apr 22, 2026
 *      Author: Matt Whispell
 */
#include "battery.h"
#include <cstdio>

float battery_get_voltage()
{
  IADC_command(IADC0, iadcCmdStartScan);

  // Wait until scan conversion is complete and FIFO has data
  while ((IADC0->STATUS & IADC_STATUS_SCANFIFODV) == 0);

  IADC_Result_t sample = IADC_pullScanFifoResult(IADC0);
  float bat_voltage = sample.data * 3.3 / 0xFFF * 2.180f; // multiply by 2 because of voltage divider
  return bat_voltage;
}

void print_voltage()
{
  printf("Bat V: %f\n\n", battery_get_voltage());
}

void device_shutdown_if_bat_is_low(void) {
  //Fully Shutdown Device if Battery Voltage is too Low
  float current_voltage = battery_get_voltage();
  if (current_voltage <= 3.35f) {
    GPIO_PinOutClear(gpioPortC, 0); //Releases Power Latch
  }
}

void power_button(void) {
  //Check Button State to Completely Turn off Device
  static uint32_t power_off_counter = 0;
  if (GPIO_PinInGet(gpioPortC, 1) == 0) { // button pressed
    power_off_counter++;
    if (power_off_counter >= 10) { // button held for 1 second
      GPIO_PinOutClear(gpioPortC, 0); //Releases Power Latch
    }
  } else {
    power_off_counter = 0;
  }
}