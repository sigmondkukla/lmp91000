/*
 * CyclicVoltammetry.h
 *
 *  Created on: Dec 28, 2025
 *      Author: Sigmond
 */

#ifndef CYCLICVOLTAMMETRY_H_
#define CYCLICVOLTAMMETRY_H_

#include "Experiment.h"

class CyclicVoltammetry : public Experiment
{
public:
  CyclicVoltammetry(
        lmp91000* lmp,
        uint32_t timestep,
        SetStatusFlagCallback status_flag_callback,

        int32_t init_e,
        int32_t vertex_1,
        int32_t vertex_2,

        uint32_t scan_rate,
        uint32_t scans,

        int32_t quiet_time,
        int32_t scan_delay):

        Experiment(lmp, timestep, status_flag_callback),

        init_e(init_e),
        vertex_1(vertex_1),
        vertex_2(vertex_2),

        scan_rate(scan_rate),

        scans(scans),
        quiet_time(quiet_time),
        scan_delay(scan_delay),

        // state variables
        current_state(QUIET_TIME),
        current_voltage((float)init_e),
        scans_remaining(scans),
        delay_remaining(quiet_time)
    {};
  virtual
  ~CyclicVoltammetry () = default;

protected:
  bool get_next_voltage(int32_t &out) override;

  int32_t init_e; // [mV], also what you return to at the end of each scan
  int32_t vertex_1; // [mV] scan goes towards this vertex first
  int32_t vertex_2; // [mV] then towards this vertex, and finally back to init_e

  uint32_t scan_rate; // [mV/s]
  uint32_t scans; // number of scans

  int32_t quiet_time; // [ms] before experiment begins
  int32_t scan_delay; // [ms] before each successive scan

  enum CVState {
    QUIET_TIME, // happens only once at beginning
    // below happens per scan
    TO_V1,
    TO_V2,
    TO_INIT,
    SCAN_DELAY
  } current_state = QUIET_TIME;

  float current_voltage;
  uint32_t scans_remaining;
  int32_t delay_remaining;
};

#endif /* CYCLICVOLTAMMETRY_H_ */
