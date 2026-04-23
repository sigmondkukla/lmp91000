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

// for printing floats (I can't get linker flags working)
#include <string>
#include <cstdio>
#include <cmath>

lmp91000 lmp(I2C0,
             gpioPortB, 12,
             gpioPortB, 11,
             gpioPortB, 13,
             IADC0, iadcPosInputPortAPin0, 3350);

lsm6dsv imu(GYRO_SENSE_1000DPS, ACC_SENSE_2G);


// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

extern "C"
{
  void printFloat(float f, int precision = 2) {
    std::string result = (f < 0) ? "-" : "";
    f = std::abs(f);

    long intPart = (long)f;
    result += std::to_string(intPart) + ".";

    float fraction = f - (float)intPart;
    for (int i = 0; i < precision; i++) {
        fraction *= 10;
        int digit = (int)fraction;
        result += std::to_string(digit);
        fraction -= digit;
    }

    printf("%s\n", result.c_str());
  }

  // Application Init.
  void app_init(void)
  {
    /////////////////////////////////////////////////////////////////////////////
    // Put your additional application init code here!                         //
    // This is called once during start-up.                                    //
    /////////////////////////////////////////////////////////////////////////////
    // lmp.init();
    imu.init();
  }

  // Application Process Action.
  void app_process_action(void)
  {
    float accX = imu.read_AccX();
    float accY = imu.read_AccY();
    float accZ = imu.read_AccZ();

    float gyroX = imu.read_GyroX();
    float gyroY = imu.read_GyroY();
    float gyroZ = imu.read_GyroZ();

    printf("------------------------\n");
    printf("accX: ");
    printFloat(accX, 2);
    printf("accY: ");
    printFloat(accY, 2);
    printf("accZ: ");
    printFloat(accZ, 2);

    printf("gyroX: ");
    printFloat(gyroX, 2);
    printf("gyroY: ");
    printFloat(gyroY, 2);
    printf("gyroZ: ");
    printFloat(gyroZ, 2);

    sl_sleeptimer_delay_millisecond(1000);

    if (app_is_process_required())
    {
      /////////////////////////////////////////////////////////////////////////////
      // Put your additional application code here!                              //
      // This is will run each time app_proceed() is called.                     //
      // Do not call blocking functions from here!                               //
      /////////////////////////////////////////////////////////////////////////////
      // notify_experiment_results();
      // notify_experiment_status();
    }
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
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
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

    // -------------------------------
    // Default event handler.
    default:
      break;
    }
  }

} // end extern "C"