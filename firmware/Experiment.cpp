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
// sl_sleeptimer_init();
  lmp->set_mode(0x03); // three lead amperometric cell
  lmp->set_fet_enable(false); // disable FET
  lmp->set_gain(3); // initially had it at 3 for 7k ohms
  lmp->set_rload(0); // 10 ohms
  lmp->set_ref_source(1); // external
  lmp->set_internal_zero(1); // internal zero 20% (used to have it on 50%)

  lmp->set_outputs_to_zero();
  printf("intialized experiment\n");
  printf("LOCK: %02x\n", lmp->read(LMP91000_REG_LOCK));
  printf("REFCN: %02x\n", lmp->read(LMP91000_REG_REFCN));
  printf("MODECN: %02x\n", lmp->read(LMP91000_REG_MODECN));
  printf("STATUS: %02x\n", lmp->read(LMP91000_REG_STATUS));
  //LMP91000_set_bias_magnitude(0x5); // TODO see if we can delete this
}

void Experiment::begin(void)
{
  iteration = 0;
  printf("[TIMER] Starting periodic timer with timestep=%u ms\n", timestep);
  sl_sleeptimer_start_periodic_timer_ms(&experiment_timer, timestep, timerCallback, this, 0, 0);

  if(status_flag_callback) // callback if it exists
    {
        status_flag_callback(STATUS_RUNNING, 1);
        printf("status flag set\n");
    }
    printf("experiment begun\n");
}

void Experiment::end(void)
{
  sl_sleeptimer_stop_timer(&experiment_timer);
  printf("experiment timer stopped - ending experiment...\n");
  lmp->Reset_Previous_Values();
  lmp->set_outputs_to_zero(); // set outputs to zero for safety
  lmp->set_mode(0);

  if(status_flag_callback) // callback if it exists
  {
      status_flag_callback(STATUS_RUNNING, 0);
  }
  results_buffer.clear();
}

void Experiment::timerCallback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)handle;
  static uint32_t callback_count = 0;
  callback_count++;
  // if (callback_count % 100 == 0) printf("[TIMER] Callback fired (count=%lu)\n", callback_count);

  Experiment *instance = static_cast<Experiment *>(data); // cast the void back to experiment instance pointer

  if (instance) {
      instance->tickHandler();
  }
}

void Experiment::tickHandler(void)
{
  // if (iteration % 100 == 0) printf("[TICK] Handler called (iteration=%lu)\n", iteration);
  
  int32_t voltage = 0;
  bool continue_experiment = get_next_voltage(voltage);
  float current = lmp->get_current();

  static int b = 0;
  static int current_b = 0;

  if (!continue_experiment) {
      end();
      printf("[TICK] Experiment ended at iteration=%lu\n", iteration);
      app_log("Ended\n");
      return;
  }

  // if true use the output voltage function (for CA), if false use the output voltage with static bias function (for CV, DPV, SWV)
  if (output_voltage) {
    lmp->output_voltage(voltage);
  } else {
    lmp->output_voltage(voltage);
    // VDAC_ChannelOutputSet(VDAC0, 0, 0xfff);
    // if (b != current_b){
    //   lmp->set_bias_magnitude(b);
    //   current_b = b;
    //   printf("REFCN: %02x\n", lmp->read(LMP91000_REG_REFCN));
    // }
    
      //lmp->output_voltage_static_bias(voltage, user_max_voltage_mag);
      //printf("static function, max target voltage: %d\n", user_max_voltage_mag);
  }

  // lmp->output_voltage(voltage);
  
  b = (int)(iteration/100) % 13;

  DataPoint dp;
  dp.timestamp = iteration * timestep; // [ms]
  dp.voltage = voltage; // [mV] applied voltage
  dp.current = current; // [A] resulting current

  if (!results_buffer.push(dp)) {
      // error
  }

  iteration++;
};
