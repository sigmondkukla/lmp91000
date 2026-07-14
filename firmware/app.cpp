/***************************************************************************
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "sl_bt_api.h"
#include "sl_main_init.h"
#include "app_assert.h"
#include "app.h"
#include "ble_experiment_service.h"
#include "em_iadc.h"
#include "battery.h"

// EVERYTHING THAT I DID
// - added logic for choosing correct output voltage function in Experiment.cpp
// - added get_current_DAC_output_mv function to lmp91000.h and lmp91000.cpp for reporting the actual voltage being output by the DAC (for debugging and for user information in future iterations)
// - added logic to lmp91000.cpp output_voltage_static_bias function to prevent unnecessary writes to the DAC and bias registers which was causing issues with stability and reliability. Now it only changes the bias settings when necessary, and it always allows changes to the output voltage, even if the bias settings are the same as before (this is important for CV where the voltage is always changing).
// - reset previous values and set outputs to zero at the end of each experiment for safety and stability
// - added more debug prints in the output_voltage_static_bias function to report the internal calculations

// THINGS TO WORK ON
// - figure out the weird behaviour with CV (always getting same general shape even without electrode)
// - make the current_DAC_output_mv value that actuall DAC out not the requested DAC out

lmp91000 lmp(I2C0,
             gpioPortA, 5,
             gpioPortA, 4,
             gpioPortA, 6,
             IADC0, iadcPosInputPortAPin0, 3350);

lsm6dsv imu(GYRO_SENSE_1000DPS, ACC_SENSE_2G);


// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

extern "C"
{
  // Application Init.
  void app_init(void)
  {
    /////////////////////////////////////////////////////////////////////////////
    // Put your additional application init code here!                         //
    // This is called once during start-up.                                    //
    /////////////////////////////////////////////////////////////////////////////
    // GPIO_PinModeSet()
    lmp.init();

    // lmp.set_mode(0x03); // three lead amperometric cell
    // lmp.set_fet_enable(false); // disable FET
    // lmp.set_gain(3); // initially had it at 3 for 7k ohms
    // lmp.set_rload(0); // 10 ohms
    // lmp.set_ref_source(1); // external
    // lmp.set_internal_zero(1); // internal zero 20% (used to have it on 50%)
    // lmp.set_bias_magnitude(3);

    // printf("LOCK: %02x\n", lmp.read(LMP91000_REG_LOCK));
    // printf("REFCN: %02x\n", lmp.read(LMP91000_REG_REFCN));
    // printf("MODECN: %02x\n", lmp.read(LMP91000_REG_MODECN));
    // printf("STATUS: %02x\n", lmp.read(LMP91000_REG_STATUS));

    // lmp.set_outputs_to_zero();

    // sl_sleeptimer_delay_millisecond(100);
    // lmp.output_voltage_test();
    // lmp.set_bias_magnitude(0);
    // lmp.DAC_write(0xfff);
    // sl_sleeptimer_delay_millisecond(1000);
    //imu.init();
    GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 1);
    GPIO_PinModeSet(gpioPortA, 8, gpioModePushPull, 1);    
  }

  // Application Process Action.
  void app_process_action(void)
  {
    
    //printf("battery voltage: %f\n", battery_get_voltage());
    //printf("app process required: %d\n", app_is_process_required());
    // if (app_is_process_required())
    // {
    //   /////////////////////////////////////////////////////////////////////////////
    //   // Put your additional application code here!                              //
    //   // This is will run each time app_proceed() is called.                     //
    //   // Do not call blocking functions from here!                               //
    //   /////////////////////////////////////////////////////////////////////////////
    //   printf("app process action running...\n");
    //   notify_experiment_status();
    //   notify_experiment_results();
    // }
    notify_experiment_results();
    notify_experiment_status();
  }

  /**************************************************************************
   * Bluetooth stack event handler.
   * This overrides the default weak implementation.
   *
   * @param[in] evt Event coming from the Bluetooth stack.
   *****************************************************************************/
  void sl_bt_on_event(sl_bt_msg_t *evt)
  {
    sl_status_t sc;

    switch (SL_BT_MSG_ID(evt->header))
    {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
          advertising_set_handle,
          160, // min. adv. interval (milliseconds * 1.6)
          160, // max. adv. interval (milliseconds * 1.6)
          0,   // adv. duration
          0);  // max. num. adv. events
      app_assert_status(sc);
      // Start advertising and enable connections.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_legacy_advertiser_connectable);
      app_assert_status(sc);
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      printf("BLE Connection opened\n");
      handle_ble_connection_status(evt);
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      handle_ble_connection_status(evt);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Restart advertising after client has disconnected.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_legacy_advertiser_connectable);
      app_assert_status(sc);
      break;

      ///////////////////////////////////////////////////////////////////////////
      // Add additional event handlers here as your application requires!      //
      ///////////////////////////////////////////////////////////////////////////

    case sl_bt_evt_gatt_server_characteristic_status_id:
      // checks when phone enables notify
      printf("GATT Characteristic status event\n");
      handle_ble_connection_status(evt);
      break;

    case sl_bt_evt_gatt_server_attribute_value_id: {

      sl_bt_evt_gatt_server_user_write_request_t *write_req = &evt->data.evt_gatt_server_user_write_request;

      // handles config and start/stop
      sl_bt_evt_gatt_server_attribute_value_t *attr_val = &evt->data.evt_gatt_server_attribute_value;
      printf("GATT Attribute Value - Characteristic: %u, Data length: %u, Data: ", attr_val->attribute, attr_val->value.len);
      for (uint32_t i = 0; i < attr_val->value.len; i++) {
        printf("%02X ", attr_val->value.data[i]);
      }
      printf("\n");
      //handle_ble_connection_status(evt);
      handle_ble_write(write_req);
      break;
    }

    // -------------------------------
    // Default event handler.
    default:
      printf("BLE Event ID: 0x%x\n", SL_BT_MSG_ID(evt->header));
      break;
    }
  }

} // end extern "C"