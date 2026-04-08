/*
 * SquareWaveVoltammetry.cpp
 *
 *  Created on: Dec 28, 2025
 *      Author: Sigmond
 */

#include "SquareWaveVoltammetry.h"

bool SquareWaveVoltammetry::get_next_voltage(int32_t &voltage_out) {
    uint32_t time_ms = iteration * timestep; // track current experiment progress

    if (time_ms < quiet_time) {
        voltage_out = init_e;
        return true;
    }

    uint32_t scan_time = time_ms - quiet_time; // active experiment time

    uint32_t period_ms = 1000 / frequency; // period is 1 second / freq
    if (period_ms < 2 * timestep) period_ms = 2 * timestep; // minimum is should be two timesteps: one low and one high. nyquist

    uint32_t step_index = scan_time / period_ms; // elapsed / period gives us thru integer division the current step
    int32_t carrier = init_e + (step_index * incr_e); // carrier/base voltage that the square wave goes around

    // check for finish
    if (incr_e > 0) { // if going up
        if (carrier > final_e) return false;
    } else {
        if (carrier < final_e) return false;
    }

    uint32_t period_elapsed = scan_time % period_ms; // [ms] since period start

    if (period_elapsed < (period_ms / 2)) { // if in first half
        voltage_out = carrier + amplitude;
    } else {
        voltage_out = carrier - amplitude; // if in second half
    }

    return true;
}
