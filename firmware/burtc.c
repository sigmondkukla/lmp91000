#include "burtc.h"
#include "em_burtc.h"

void initBURTC(void)
{
  //Detect Reset Cause
  uint32_t rstCause = EMU->RSTCAUSE;
  EMU->CMD = EMU_CMD_RSTCAUSECLR;
  printf("Reset cause: 0x%08lX | ", (unsigned long)rstCause);

  //EM4 Wakeup
  if (rstCause & EMU_RSTCAUSE_EM4) {
    printf("EM4 wakeup\n");
    initBURTC_em4wake();
  }
  //Startup or Other
  else {
    printf("Cold Start or Other Reset\n");
    initBURTC_cold();
  }
}

void initBURTC_cold(void)
{
  CMU_ClockEnable(cmuClock_BURAM, true);
  BURAM->RET[0].REG = 0;

  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);
  CMU_ClockEnable(cmuClock_BURTC, true);
  CMU_ClockEnable(cmuClock_BURAM, true);

  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.compare0Top = false;
  burtcInit.em4comp = true;     //Allow BURTC compare to wake from EM4
  BURTC_Init(&burtcInit);

  BURTC_CounterReset();
  BURTC_CompareSet(0, BURTC_IRQ_PERIOD);

  BURTC_IntClear(BURTC_IF_COMP);
  BURTC_IntEnable(BURTC_IEN_COMP);
  NVIC_EnableIRQ(BURTC_IRQn);
  BURTC_Enable(true);
}

void initBURTC_em4wake(void)
{
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);
  CMU_ClockEnable(cmuClock_BURTC, true);
  CMU_ClockEnable(cmuClock_BURAM, true);

  //BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  //burtcInit.compare0Top = false;
  //burtcInit.em4comp = true;     //Allow BURTC compare to wake from EM4
  //BURTC_Init(&burtcInit);
  //BURTC_IntClear(BURTC_IF_COMP);
  //BURTC_Enable(false);

  //BURTC_CounterReset();
  BURAM->RET[0].REG += BURTC_IRQ_PERIOD;
  
  //BURTC_CompareSet(0, BURTC_IRQ_PERIOD);
  // Schedule next wakeup relative to current counter value
  uint32_t next = BURTC_CounterGet() + BURTC_IRQ_PERIOD;
  BURTC_CompareSet(0, next);


  BURTC_IntClear(BURTC_IF_COMP);
  BURTC_IntEnable(BURTC_IEN_COMP);
  NVIC_EnableIRQ(BURTC_IRQn);
  //BURTC_Enable(true);

  printf("Finished this function\n");
}

//if (rstCause & EMU_RSTCAUSE_POR) {