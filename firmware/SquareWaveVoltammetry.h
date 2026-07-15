/*
 * SquareWaveVoltammetry.h
 *
 *  Created on: Dec 28, 2025
 *      Author: Sigmond
 */

#ifndef SQUAREWAVEVOLTAMMETRY_H_
#define SQUAREWAVEVOLTAMMETRY_H_

#include "Experiment.h"

class SquareWaveVoltammetry : public Experiment
{
public:
  SquareWaveVoltammetry(
    lmp91000* lmp,
    uint32_t timestep,
    SetStatusFlagCallback status_flag_callback,
  
    int32_t init_e,
    int32_t final_e,
    int32_t incr_e,

    uint32_t amplitude,
    uint32_t frequency,

    uint32_t quiet_time):

    Experiment(lmp, timestep, status_flag_callback),

    init_e(init_e),
    final_e(final_e),
    incr_e(incr_e),

    amplitude(amplitude),
    frequency(frequency),

    quiet_time(quiet_time)
  {};

  virtual
  ~SquareWaveVoltammetry () = default;

protected:
  bool get_next_voltage(int32_t &voltage_out) override;

  int32_t init_e; // [mV]
  int32_t final_e; // [mV]
  int32_t incr_e; // [mV]

  uint32_t amplitude; // [mV]
  uint32_t frequency; // [Hz]

  uint32_t quiet_time; // [ms] before experiment begins
};

#endif /* SQUAREWAVEVOLTAMMETRY_H_ */
