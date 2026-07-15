/***************************************************************************//**
 * @file ble_debug.h
 * @brief Wireless BLE Logging utility header.
 ******************************************************************************/
#ifndef BLE_DEBUG_H
#define BLE_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

// Including this inside the extern "C" block solves the linkage conflict!
#include "sl_bluetooth.h"

/**
 * @brief Processes incoming BLE stack events to track the debugger's connection handle.
 * @param[in] evt Event coming from the Bluetooth stack.
 */
void ble_debug_handle_event(sl_bt_msg_t *evt);

/**
 * @brief Format and send a string directly over BLE notifications to the subscriber.
 * @param[in] format Formatted string print statement (printf style).
 */
void ble_printf(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // BLE_DEBUG_H