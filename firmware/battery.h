/*
 * battery.cpp
 *
 *  Created on: Apr 22, 2026
 *      Author: Matt Whispell
 *  Functions added: July 8, 2026
 *      Author: Joseph Mire
 */
#include "em_iadc.h"

float battery_get_voltage();
void print_voltage(void);
void device_shutdown_if_bat_is_low(void);
void power_button(void);