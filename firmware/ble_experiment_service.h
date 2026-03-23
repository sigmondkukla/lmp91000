/* ble_experiment_service.h */
#ifndef BLE_EXPERIMENT_SERVICE_H
#define BLE_EXPERIMENT_SERVICE_H

#include "sl_bt_api.h"
#include <Experiment.h>
#include <CyclicVoltammetry.h>
#include <SquareWaveVoltammetry.h>
#include <DifferentialPulseVoltammetry.h>
#include <Chronoamperometry.h>

// global state made externally visible
extern Experiment* currentExperiment;
extern uint8_t active_connection_handle;
extern bool notification_enabled;

void handle_ble_write(sl_bt_evt_gatt_server_user_write_request_t *data); // called from sl_bt_on_event
void handle_ble_connection_status(sl_bt_msg_t *evt); // called from sl_bt_on_event
void notify_experiment_results(void); // called from app_process_action
void notify_experiment_status(void); // called from app_process_action

#endif
