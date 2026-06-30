#ifndef BURTC_H
#define BURTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "em_burtc.h"
#include "em_cmu.h"

//2 Seconds
#define BURTC_IRQ_PERIOD  2 * (1000 / 512)

void BURTC_IRQHandler(void);

void initBURTC(void);
void initBURTC_cold(void);
void initBURTC_em4wake(void);

void scheduleBURTC_em4(void);

static int is_leap(int y);
static int days_in_month(int m, int y);
void print_time(void);

#ifdef __cplusplus
}
#endif

#endif