/*
 * Chronoamperometry.cpp
 *
 *  Created on: Dec 28, 2025
 *      Author: Sigmond
 */

#include <Chronoamperometry.h>

bool Chronoamperometry::get_next_voltage(int32_t &voltage_out) {
    uint32_t t = iteration * timestep;

    // Find boundaries for each new voltage
    uint32_t t1 = quiet_time;
    uint32_t t2 = t1 + duration_1;
    uint32_t t3 = t2 + duration_2;
    uint32_t t4 = t3 + duration_3;

    if (t < t1) {
        voltage_out = init_e;
    } else if (t < t2) {
        voltage_out = e_1;
    } else if (t < t3) {
        voltage_out = e_2;
    } else if (t < t4) {
        voltage_out = e_3;
    } else {
        voltage_out = final_e; // otherwise, the experiment is done
        return false;
    }

    return true; // otherwise, the experiment continues
}

