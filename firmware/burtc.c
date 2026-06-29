#include "burtc.h"
#include "efr32bg24_emu.h"
#include "em_burtc.h"
#include <stdint.h>

void initBURTC(void)
{
  //Detect Reset Cause
  uint32_t rstCause = EMU->RSTCAUSE;
  EMU->CMD = EMU_CMD_RSTCAUSECLR;
  printf("Reset cause: 0x%08lX | ", (unsigned long)rstCause);
  
  //Reset Cause Conditions
  //EMU_RSTCAUSE_POR condition in future?
  if (rstCause & EMU_RSTCAUSE_EM4) {
    printf("EM4 wakeup\n");
    initBURTC_em4wake();
  }
  else if (rstCause & EMU_RSTCAUSE_SYSREQ) { //When Freshly Flashed
    printf("System Reset\n");
    initBURTC_cold();
  }
  else if (rstCause & EMU_RSTCAUSE_PIN) {
    printf("Pin Reset\n");
    initBURTC_cold();
  }
  else {
    printf("Other Reset\n");
    initBURTC_cold();
  }
}

void initBURTC_cold(void)
{
  CMU_ClockEnable(cmuClock_BURAM, true);

  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);
  CMU_ClockEnable(cmuClock_BURTC, true);
  CMU_ClockEnable(cmuClock_BURAM, true);

  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.start = false;  
  burtcInit.compare0Top = false;
  burtcInit.em4comp = true;     //Allow BURTC compare to wake from EM4
  BURTC_Init(&burtcInit);

  BURTC_CounterReset();
  BURTC_CompareSet(0, 0xFFFFFFFF);

  BURTC_IntClear(BURTC_IF_COMP);
  BURTC_IntEnable(BURTC_IEN_COMP);
  NVIC_EnableIRQ(BURTC_IRQn);
  BURTC_Enable(true);
  BURTC_Start();
}

void initBURTC_em4wake(void)
{
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);
  CMU_ClockEnable(cmuClock_BURTC, true);
  CMU_ClockEnable(cmuClock_BURAM, true);

  BURTC_IntClear(BURTC_IF_COMP);
  BURTC_IntEnable(BURTC_IEN_COMP);
  NVIC_EnableIRQ(BURTC_IRQn);
}

void scheduleBURTC_em4(void) {
  uint32_t next = BURTC_CounterGet() + BURTC_IRQ_PERIOD;
  BURTC_CompareSet(0, next);
}