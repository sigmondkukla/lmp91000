/*
 * ble_protocol.h
 *
 *  Created on: Dec 28, 2025
 *      Author: Sigmond
 */

#ifndef BLE_PROTOCOL_H_
#define BLE_PROTOCOL_H_

#include <cstdint>

enum ExperimentType : uint8_t {
  CV  = 0x01,
  SWV = 0x02,
  DPV = 0x03,
  CA  = 0x04
};

#pragma pack(push, 1) // force structs to align to 1 byte

struct CVConfig {
  uint32_t  type_id; // always 0x01 = CV  // bytes 0 to 3
  int32_t  init_e;                        // 4 to 7
  int32_t  vertex_1;                      // 8 to 11
  int32_t  vertex_2;                      // 12 to 15
  uint32_t scan_rate;                     // 16 to 19
  uint32_t scans;                         // 20 to 23
  uint32_t quiet_time;                    // 24 to 27
  uint32_t scan_delay;                    // 28 to 31
};

struct SWVConfig {
  uint32_t  type_id; // always 0x02 = SWV // bytes 0 to 3
  int32_t  init_e;                        // 4 to 7
  int32_t  final_e;                       // 8 to 11
  int32_t  incr_e;                        // 12 to 15
  uint32_t amplitude;                     // 16 to 19
  uint32_t frequency;                     // 20 to 23
  uint32_t quiet_time;                    // 24 to 27
};

struct DPVConfig {
  uint32_t  type_id; // always 0x03 = DPV // bytes 0 to 3
  int32_t  init_e;                        // 4 to 7
  int32_t  final_e;                       // 8 to 11
  int32_t  incr_e;                        // 12 to 15
  uint32_t amplitude;                     // 16 to 19
  uint32_t frequency;                     // 20 to 23
  uint32_t quiet_time;                    // 24 to 27
  float    duty_cycle;   // 0.0 to 1.0    // 28 to 31
};

struct CAConfig {
  uint32_t  type_id; // always 0x04 = CA  // bytes 0 to 3
  int32_t  init_e;                        // 4 to 7
  uint32_t quiet_time;                    // 8 to 11
  int32_t  e_1;                           // 12 to 15
  uint32_t duration_1;                    // 16 to 19
  int32_t  e_2;                           // 20 to 23
  uint32_t duration_2;                    // 24 to 27
  int32_t  e_3;                           // 28 to 31
  uint32_t duration_3;                    // 32 to 35
  int32_t  final_e;                       // 36 to 39
};

#pragma pack(pop) // Restore default alignment

#endif /* BLE_PROTOCOL_H_ */
