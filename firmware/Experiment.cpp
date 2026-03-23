/*
 * experiment.cpp
 *
 *  Created on: Jun 2, 2025
 *      Author: Sigmond
 */

#include "Experiment.h"
#include "sl_bluetooth.h"
#include "ble_experiment_service.h"
#include "lmp91000.h"
#include "app_log.h"
#include "experiment_status.h"

// Abstract Experiment parent class

Experiment::~Experiment()
{
}

void Experiment::init(void)
{
//  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
//  CMU_ClockEnable(cmuClock_RTCC, true);
//
//  sl_sleeptimer_init();

  lmp->set_mode(0x3); // three lead amperometric cell
  lmp->set_fet_enable(false); // disable FET
  lmp->set_gain(3);
  lmp->set_rload(0); // 10 ohms
  lmp->set_ref_source(1); // external
  lmp->set_internal_zero(1); // internal zero 50%
  lmp->set_outputs_to_zero();
  //LMP91000_set_bias_magnitude(0x5); // TODO see if we can delete this
}

void Experiment::begin(void)
{
  iteration = 0;
  sl_sleeptimer_start_periodic_timer_ms(&experiment_timer, timestep, timerCallback, this, 0, 0);

  if(status_flag_callback) // callback if it exists
    {
        status_flag_callback(STATUS_RUNNING, 1);
    }
}

void Experiment::end(void)
{
  sl_sleeptimer_stop_timer(&experiment_timer);

  if(status_flag_callback) // callback if it exists
  {
      status_flag_callback(STATUS_RUNNING, 0);
  }
}

void Experiment::timerCallback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)handle;

  Experiment *instance = static_cast<Experiment *>(data); // cast the void back to experiment instance pointer

  if (instance) {
      instance->tickHandler();
  }
}

void Experiment::tickHandler(void)
{
  int32_t voltage = 0;
  bool continue_experiment = get_next_voltage(voltage);

  if (!continue_experiment) {
      end();
      app_log("Ended\n");
      return;
  }

  lmp->output_voltage(voltage);
  float current = lmp->get_current();

  DataPoint dp;
  dp.timestamp = iteration * timestep; // [ms]
  dp.voltage = voltage; // [mV] applied voltage
  dp.current = current; // [A] resulting current

  if (!results_buffer.push(dp)) {
      // error
  }

  iteration++;
};
