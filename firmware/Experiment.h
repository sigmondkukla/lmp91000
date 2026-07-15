/*
 * experiment.h
 *
 *  Created on: May 28, 2025
 *      Author: Sigmond
 */

#include "lmp91000.h"
#include "sl_sleeptimer.h"
#include "em_cmu.h"
#include <vector>
#include <cstdint>
#include <string>
#include "RingBuffer.h"

#ifndef EXPERIMENT_EXPERIMENT_H_
#define EXPERIMENT_EXPERIMENT_H_

// macros for choosing correct output voltage function
#define USE_DYNAMIC_BIAS_OUTPUT true // for CA
#define USE_STATIC_BIAS_OUTPUT false // for CV, DPV, SWV

struct DataPoint {
  uint32_t timestamp; // [ms] 4 bytes
  int32_t voltage; // [mV] 4 bytes
  float current; // [A] 4 bytes, subject to change
};

typedef void (*SetStatusFlagCallback)(uint8_t, uint8_t);

class Experiment {
public:

  Experiment(lmp91000* lmp, uint32_t timestep, SetStatusFlagCallback status_flag_callback, bool output_voltage, int32_t user_max_voltage_mag): lmp(lmp), timestep(timestep), status_flag_callback(status_flag_callback), output_voltage(output_voltage), user_max_voltage_mag(user_max_voltage_mag) {};
  virtual ~Experiment();

  void init(void);
  void begin(void);
  void end(void);

  RingBuffer<DataPoint, 128> results_buffer;

protected:

  static void timerCallback(sl_sleeptimer_timer_handle_t *handle, void *data);
  void tickHandler(void);

  virtual bool get_next_voltage(int32_t &out) = 0; // true if continues, out written to next voltage

  lmp91000 *lmp;

  uint32_t iteration = 0; // tracks progress, ticks = ms / timestep
  const uint32_t timestep; // [ms] should be 10 ideally

  sl_sleeptimer_timer_handle_t experiment_timer;

  SetStatusFlagCallback status_flag_callback = nullptr;

  // added logic by zach for choosing correct output voltage funttion
  bool output_voltage = false;
  int32_t user_max_voltage_mag;
};

#endif /* EXPERIMENT_EXPERIMENT_H_ */
