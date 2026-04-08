/*
 * DifferentialPulseVoltammetry.cpp
 *
 *  Created on: Dec 28, 2025
 *      Author: Sigmond
 */

#include "DifferentialPulseVoltammetry.h"

bool DifferentialPulseVoltammetry::get_next_voltage(int32_t &voltage_out) {
    uint32_t time = iteration * timestep; // [ms]

    if (time < quiet_time) {
        voltage_out = init_e;
        return true;
    }

    uint32_t scan_time = time - quiet_time; // [ms] elapsed since scan started
    uint32_t period = 1000 / frequency; // [ms]
    if (period < 2 * timestep) period = 2 * timestep; // still need at least time for a low and high pulse. maybe we should calculate based on duty cycle though?

    uint32_t step_index = scan_time / period;
    int32_t carrier = init_e + (step_index * incr_e);

    if (incr_e > 0) { // finish condition, see SWV
        if (carrier > final_e) return false;
    } else {
        if (carrier < final_e) return false;
    }

    uint32_t period_elapsed = scan_time % period;

    uint32_t pulse_width = (uint32_t)((float)period * duty_cycle);

    if (pulse_width < timestep) pulse_width = timestep; // ensure pulse width is at least one timestep wide
    if (pulse_width > (period - timestep)) pulse_width = period - timestep; // also that the base is at least one

    voltage_out = carrier;
    if (period_elapsed >= (period - pulse_width)) // if near end of period
        voltage_out += amplitude;

    return true;
}
