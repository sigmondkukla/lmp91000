/***************************************************************************//**
 * @file ble_debug.cpp
 * @brief Wireless BLE Logging utility implementation.
 ******************************************************************************/
#include "ble_debug.h"
#include "gatt_db.h"
#include <stdio.h>
#include <stdarg.h>

// Track which connection is subscribed to debug notifications (0xFF = none)
static uint8_t debug_connection_handle = 0xff;

extern "C" void ble_debug_handle_event(sl_bt_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    // Reset our handle if the logging client disconnects
    case sl_bt_evt_connection_closed_id:
      if (evt->data.evt_connection_closed.connection == debug_connection_handle) {
        debug_connection_handle = 0xff;
        printf("[BLE_DEBUG] Logging client disconnected.\n");
      }
      break;

    // Monitor GATT subscriptions to find when notifications are enabled/disabled
    case sl_bt_evt_gatt_server_characteristic_status_id: {
      auto *status = &evt->data.evt_gatt_server_characteristic_status;

      if (status->characteristic == gattdb_debug_log_char) {
        if (status->status_flags & sl_bt_gatt_server_client_config) {
          if (status->client_config_flags & sl_bt_gatt_server_notification) {
            // Client turned notifications ON: register this connection
            debug_connection_handle = status->connection;
            printf("[BLE_DEBUG] Log subscriptions enabled on connection %d\n", debug_connection_handle);
          } else {
            // Client turned notifications OFF
            if (debug_connection_handle == status->connection) {
              debug_connection_handle = 0xff;
              printf("[BLE_DEBUG] Log subscriptions disabled.\n");
            }
          }
        }
      }
      break;
    }

    default:
      break;
  }
}

extern "C" void ble_printf(const char *format, ...)
{
  // If no device is listening to our logs, exit early to save RAM cycles & radio airtime
  if (debug_connection_handle == 0xff) {
    return;
  }

  char log_buffer[64]; // Adjust buffer size here if you run out of string space
  va_list args;
  va_start(args, format);
  int length = vsnprintf(log_buffer, sizeof(log_buffer), format, args);
  va_end(args);

  if (length > 0) {
    sl_bt_gatt_server_send_notification(
        debug_connection_handle,
        gattdb_debug_log_char,
        length,
        (uint8_t *)log_buffer
    );
  }
}