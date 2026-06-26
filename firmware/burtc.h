#ifndef BURTC_H
#define BURTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "em_burtc.h"
#include "em_cmu.h"

//2 Seconds
#define BURTC_IRQ_PERIOD  2000

void BURTC_IRQHandler(void);

void initBURTC(void);
void initBURTC_cold(void);
void initBURTC_em4wake(void);

#ifdef __cplusplus
}
#endif

#endif