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
#include "sl_power_manager.h"
#include "em_burtc.h"
#include "burtc.h"
#include <cstdint>
#include <cstdio>


lmp91000 lmp(I2C0,
             gpioPortA, 5,
             gpioPortA, 4,
             gpioPortA, 6,
             IADC0, iadcPosInputPortAPin0, 3350);

lsm6dsv imu(GYRO_SENSE_1000DPS, ACC_SENSE_2G);


//The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

extern "C"
{
  void app_init(void)
  {
    //Prints So Know When Device Restarts
    printf("---------Startup!\n");
    fflush(stdout);
    
    //Start GPIO CLock, Initiate GPIO modes
    CMU_ClockEnable(cmuClock_GPIO, 1);
    GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 1); //Latches Power On
    GPIO_PinModeSet(gpioPortC, 1, gpioModeInputPull, 1); //Button
    GPIO_PinModeSet(gpioPortA, 8, gpioModePushPull, 1); //LED

    //Unlatch pins retained from EM4
    EMU_UnlatchPinRetention();

    //Initiations
    lmp.init();
    initBURTC();
  }

  void app_process_action(void)
  {
    //Printouts
    uint32_t now = BURTC_CounterGet() / 1000;
    uint32_t minutes = now / 60;
    uint32_t seconds = now % 60;
    
    printf("Minutes: %lu: ", (unsigned long)minutes);
    printf("Seconds: %lu\n", (unsigned long)seconds);
    //printf("Bat V: %f\n", battery_get_voltage());

    print_time();

    //Actual Experiemnt
    notify_experiment_results();
    notify_experiment_status();

    //Check Button State to Completely Turn off Device -- TODO: Need to Add a Timer or counter
    //if (GPIO_PinInGet(gpioPortC, 1) == 0) { // button pressed
    //  printf("Button Pressed");
    //  GPIO_PinOutClear(gpioPortC, 0); //Releases Power Latch
    //}

    //sl_sleeptimer_delay_millisecond(5000);

    //EM4 Time
    GPIO_PinOutClear(gpioPortA, 8); //Turn off LED
    sl_sleeptimer_delay_millisecond(10); //Allows Prints to Work
    scheduleBURTC_em4();
    BURAM->RET[0].REG = BURTC_CounterGet();
    //sl_power_manager_enter_em4();
    EMU_EnterEM4();
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

}