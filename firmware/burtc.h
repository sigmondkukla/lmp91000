#ifndef BURTC_H
#define BURTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "em_cmu.h"

#define BURTC_IRQ_PERIOD 1
#define CURRENT_UNIX_TIMESTAMP 0

void initBURTC(void);
void initBURTC_cold(uint32_t rstCause);
void initBURTC_em4wake(void);
void update_time(uint32_t new_time);
void scheduleBURTC_em4(void);
void print_time(void);

#ifdef __cplusplus
}
#endif

#endif