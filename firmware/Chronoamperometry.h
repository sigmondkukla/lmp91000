/*
 * Chronoamperometry.h
 *
 *  Created on: Dec 28, 2025
 *      Author: Sigmond
 */

#ifndef CHRONOAMPEROMETRY_H_
#define CHRONOAMPEROMETRY_H_

#include "Experiment.h"

class Chronoamperometry : public Experiment
{
public:
  Chronoamperometry(
      lmp91000* lmp,
      uint32_t timestep,
      SetStatusFlagCallback status_flag_callback,

      int32_t init_e,
      uint32_t quiet_time,
      int32_t e_1,
      uint32_t duration_1,
      int32_t e_2,
      uint32_t duration_2,
      int32_t e_3,
      uint32_t duration_3,
      int32_t final_e):

      Experiment(lmp, timestep, status_flag_callback),

      init_e(init_e),
      quiet_time(quiet_time),
      e_1(e_1),
      duration_1(duration_1),
      e_2(e_2),
      duration_2(duration_2),
      e_3(e_3),
      duration_3(duration_3),
      final_e(final_e)
    {};
  virtual
  ~Chronoamperometry () = default;

protected:
  bool get_next_voltage(int32_t &voltage_out) override;

  int32_t init_e;
  uint32_t quiet_time;
  int32_t e_1;
  uint32_t duration_1;
  int32_t e_2;
  uint32_t duration_2;
  int32_t e_3;
  uint32_t duration_3;
  int32_t final_e;
};

#endif /* CHRONOAMPEROMETRY_H_ */
