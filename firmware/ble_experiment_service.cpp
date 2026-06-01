/*
 * ble_experiment_service.cpp
 *
 *  Created on: Dec 28, 2025
 *      Author: Sigmond
 */

/* ble_experiment_service.cpp */
#include "ble_experiment_service.h"
#include "ble_protocol.h"
#include "gatt_db.h"
#include "app.h" // For access to lmp1 / lmp2
#include "experiment_status.h"
#include <cstdio>

#define TIMESTEP 10 // [ms] let's keep it at 100 Hz for now

// Helper function to print data in hex format
static void print_hex_data(const char* label, const uint8_t* data, size_t len) {
    printf("%s [%zu bytes]: ", label, len);
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// bring in global instances
Experiment* currentExperiment = nullptr;
uint8_t active_connection_handle = 0xFF;
bool results_notification_enabled = false;
bool status_notification_enabled = false;

static uint8_t status_register = 0;
volatile uint8_t new_status_register = 0;
volatile bool status_pending = false;

void set_status_flag(uint8_t flag, uint8_t value) {
  new_status_register &= ~(1 << flag);
  new_status_register |= (value & 1) << flag;
  status_pending = true;
}

static void create_new_experiment(const uint8_t* data, size_t len) {
    printf("Creating new experiment with len: %d\n", (int)len);
    // cleanup prior experiment if exists
    if (currentExperiment != nullptr) {
        currentExperiment->end();
        delete currentExperiment;
        currentExperiment = nullptr;
    }


    if (len < 4) return; // must be able to read at least 4 bytes to get the ID
    uint32_t type_id = *reinterpret_cast<const uint32_t*>(data); // assume its little endian and peek at those first four bytes
    printf("Received config for experiment type ID: %lu\n", type_id);

    switch (type_id) {
        case CV: {
            if (len != sizeof(CVConfig)) return; // check for size mismatch
            const CVConfig* cfg = reinterpret_cast<const CVConfig*>(data); // if all's well convert it

            currentExperiment = new CyclicVoltammetry(
                &lmp, TIMESTEP, set_status_flag,
                cfg->init_e, cfg->vertex_1, cfg->vertex_2,
                cfg->scan_rate, cfg->scans,
                cfg->quiet_time, cfg->scan_delay
            );
            break;
        }
        case SWV: {
            if (len != sizeof(SWVConfig)) return;
            const SWVConfig* cfg = reinterpret_cast<const SWVConfig*>(data);

            currentExperiment = new SquareWaveVoltammetry(
                &lmp, TIMESTEP, set_status_flag,
                cfg->init_e, cfg->final_e, cfg->incr_e,
                cfg->amplitude, cfg->frequency, cfg->quiet_time
            );
            break;
        }
        case DPV: {
            if (len != sizeof(DPVConfig)) return;
            const DPVConfig* cfg = reinterpret_cast<const DPVConfig*>(data);

            currentExperiment = new DifferentialPulseVoltammetry(
                &lmp, TIMESTEP, set_status_flag,
                cfg->init_e, cfg->final_e, cfg->incr_e,
                cfg->amplitude, cfg->frequency,
                cfg->quiet_time, cfg->duty_cycle
            );
            break;
        }
        case CA: {
            if (len != sizeof(CAConfig)) return;
            const CAConfig* cfg = reinterpret_cast<const CAConfig*>(data);
            printf("Creating CA with: init_e=%ld, quiet_time=%ld, e1=%ld, duration1=%ld, e2=%ld, duration2=%ld, e3=%ld, duration3=%ld, final_e=%ld\n",
                cfg->init_e, cfg->quiet_time, cfg->e_1, cfg->duration_1, cfg->e_2, cfg->duration_2, cfg->e_3, cfg->duration_3, cfg->final_e
            );

            currentExperiment = new Chronoamperometry(
                &lmp, TIMESTEP, set_status_flag,
                cfg->init_e, cfg->quiet_time,
                cfg->e_1, cfg->duration_1,
                cfg->e_2, cfg->duration_2,
                cfg->e_3, cfg->duration_3,
                cfg->final_e
            );
            break;
        }
        default:
          return;
    }

    set_status_flag(STATUS_CONFIGURED, 1);
}

void handle_ble_connection_status(sl_bt_msg_t *evt) {
    printf("Handling BLE connection status event with ID: %x\n", SL_BT_MSG_ID(evt->header));
    switch (SL_BT_MSG_ID(evt->header)) {
        case sl_bt_evt_connection_opened_id:
        printf("sl_bt_evt_connection_opened_id\n");
            active_connection_handle = evt->data.evt_connection_opened.connection;
            sl_bt_gatt_server_set_max_mtu(247, NULL); // request max MTU of 247 bytes. maybe we can go higher in future?
            break;

        case sl_bt_evt_connection_closed_id:
        printf("sl_bt_evt_connection_closed_id\n");
            active_connection_handle = 0xFF;
            results_notification_enabled = false;
            status_notification_enabled = false;
            if (currentExperiment) { // if we lose connection stop the experiment. better than pretending alls well I think
                currentExperiment->end();
            }
            break;

        case sl_bt_evt_gatt_server_characteristic_status_id:
        printf("sl_bt_evt_gatt_server_characteristic_status_id char is %u\n", evt->data.evt_gatt_server_characteristic_status.characteristic);
            if (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_experiment_results) {
                 if (evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config) {

                     results_notification_enabled = (evt->data.evt_gatt_server_characteristic_status.client_config_flags & sl_bt_gatt_notification); // Check if notification bit 1 is set
                     printf("Results notification enabled: %s\n", results_notification_enabled ? "true" : "false");
                 }
            }
            if (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_experiment_status) {
                 if (evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config) {

                     status_notification_enabled = (evt->data.evt_gatt_server_characteristic_status.client_config_flags & sl_bt_gatt_notification);
                     printf("Status notification enabled: %s\n", status_notification_enabled ? "true" : "false");
                 }
            }
            break;
    }
}

void handle_ble_write(sl_bt_evt_gatt_server_user_write_request_t *req) {
    print_hex_data("BLE RX", req->value.data, req->value.len);
    
    printf("Write request for characteristic handle: %u\n", req->characteristic);
    // If the incoming write is for characteristic 27, reply with a configured status byte
    if (req->characteristic == 27) {
        uint8_t cfg_status = (1 << STATUS_CONFIGURED);
        // update GATT db so reads see the status
        sl_bt_gatt_server_write_attribute_value(gattdb_experiment_status, 0, 1, &cfg_status);
        // send notification if enabled and connected
        if (active_connection_handle != 0xFF && status_notification_enabled) {
            sl_bt_gatt_server_send_notification(active_connection_handle, gattdb_experiment_status, 1, &cfg_status);
            printf("Sent CONFIGURED status notification (0x%02X)\n", cfg_status);
        } else {
            printf("CONFIGURED status prepared but not notified: conn=0x%02X notif=%d\n", active_connection_handle, status_notification_enabled);
        }
    }
    if (req->characteristic == gattdb_experiment_config) {
        printf("Received write to experiment config characteristic\n");
        create_new_experiment(req->value.data, req->value.len);
        sl_bt_gatt_server_send_user_write_response(req->connection, gattdb_experiment_config, 0);
        printf("request connection: %d", req->connection);
    }
    else if (req->characteristic == gattdb_experiment_status) {
        if (req->value.len == 1) {
            uint8_t data = req->value.data[0];

            if (data & 0x01) { // experiment start
                printf("experiment starting...\n");
                if (currentExperiment) {
                    printf("starting experiment instance at %p\n", (void*)currentExperiment);
                    currentExperiment->init();
                    currentExperiment->begin();
                }
            } else { // experiment stop
                printf("experiment stopping...\n");
                currentExperiment->end();
            }
        }
    }
}

#define BYTES_PER_SAMPLE sizeof(DataPoint) // 12 // sizeof should be safe though because it's aligned (3 * 4 bytes)
#define SAMPLES_PER_PACKET 10
#define PACKET_SIZE (SAMPLES_PER_PACKET * BYTES_PER_SAMPLE)

void notify_experiment_results(void) {
    //printf("current Experiment: %p, active connection: 0x%02X, results notif enabled: %d\n", (void*)currentExperiment, active_connection_handle, results_notification_enabled);
    if (!currentExperiment || active_connection_handle == 0xFF || !results_notification_enabled) {
        return;
    }

    if (currentExperiment->results_buffer.count() >= SAMPLES_PER_PACKET) { // if the buffer has filled enough for a whole packet
        printf("packet filled...\n");
        uint8_t packet[PACKET_SIZE];
        int offset = 0;

        for (int i = 0; i < SAMPLES_PER_PACKET; i++) {
            DataPoint dp;
            if (currentExperiment->results_buffer.pop(dp)) { // pop from ring buffer into dp
                memcpy(&packet[offset], &dp, BYTES_PER_SAMPLE); // DataPoint is {uint32_t, int32_t, float} which is 12 bytes packed and little endian so I think we can copy it
                offset += BYTES_PER_SAMPLE;
            }
            printf("Popped DataPoint %d: timestamp=%u, voltage=%d, current=%f\n", i, dp.timestamp, dp.voltage, dp.current);
        }

        sl_bt_gatt_server_send_notification(
            active_connection_handle,
            gattdb_experiment_results,
            sizeof(packet),
            packet
        );
    }
}

void notify_experiment_status(void) {
    if (!currentExperiment || active_connection_handle == 0xFF) {
            return;
    }

    if (status_pending) {
        // clear flag and latch register
        status_pending = false;
        status_register = new_status_register;

        // Sync with GATT Database (So 'Read' works)
        sl_bt_gatt_server_write_attribute_value(gattdb_experiment_status, 0, 1, &status_register); // sync GATT db for read operations

        if (status_notification_enabled) { // send notification if enabled
            sl_bt_gatt_server_send_notification(
                active_connection_handle,
                gattdb_experiment_status,
                1,
                &status_register
            );
        }
    }
}
