/*
 * CyclicVoltammetry.cpp
 *
 *  Created on: Dec 28, 2025
 *      Author: Sigmond
 */

#include <CyclicVoltammetry.h>

bool CyclicVoltammetry::get_next_voltage(int32_t &voltage_out) {
    if (current_state == QUIET_TIME || current_state == SCAN_DELAY) {
      // quiet time and scan delay both act the same, creating a delay before the next scan
      delay_remaining -= timestep;
      voltage_out = init_e;
      if (delay_remaining <= 0) current_state = TO_V1;
      return true;
    }

    int32_t target;
    switch(current_state){
      case TO_V1:
        target = vertex_1;
        break;
      case TO_V2:
        target = vertex_2;
        break;
      case TO_INIT:
        target = init_e;
        break;
      default: return false;
    }

    float step = (float)scan_rate * timestep / 1000.0f; // step is a float, can be less than 1 mv/timestep
    float direction = (target > current_voltage) ? 1.0f : -1.0f; // wasteful to use a float but easy multiplication then
    current_voltage += (step * direction); // current voltage is a float for same reasons as step
    voltage_out = (int32_t)current_voltage; // gets downsampled to nearest mV tho afterwards

    bool reached_target = (direction == 1.0f && current_voltage >= target) ||
                          (direction == -1.0f && current_voltage <= target);
    if (reached_target) {
        // Clamp both voltages to target to prevent cascading small errors
        current_voltage = (float)target;
        voltage_out = target;

        switch(current_state){
          case TO_V1:
            current_state = TO_V2;
            break;
          case TO_V2:
            current_state = TO_INIT;
            break;
          case TO_INIT:
            scans_remaining--;
            if (scans_remaining == 0) return false;
            else {
                if (scan_delay != 0) {
                    current_state = SCAN_DELAY;
                    delay_remaining = scan_delay;
                }
                else current_state = TO_V1;
            }
            break;
          default:
            return false; // error
        }
    }
    return true;
}

