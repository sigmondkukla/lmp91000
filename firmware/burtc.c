#include "burtc.h"

void BURTC_IRQHandler(void)
{
  BURTC_IntClear(BURTC_IF_COMP);
}

void initBURTC_cold(void)
{
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);
  CMU_ClockEnable(cmuClock_BURTC, true);
  CMU_ClockEnable(cmuClock_BURAM, true);

  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.compare0Top = true;
  burtcInit.em4comp = true;     // Allow BURTC compare to wake from EM4
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

  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.compare0Top = true;
  burtcInit.em4comp = true;     // Allow BURTC compare to wake from EM4
  BURTC_Init(&burtcInit);

  BURTC_CounterReset();
  BURTC_CompareSet(0, BURTC_IRQ_PERIOD);

  BURTC_IntClear(BURTC_IF_COMP);
  BURTC_IntEnable(BURTC_IEN_COMP);
  NVIC_EnableIRQ(BURTC_IRQn);
  BURTC_Enable(true);
}

void initBURTC(void)
{
  uint32_t rstCause = EMU->RSTCAUSE;
    EMU->CMD = EMU_CMD_RSTCAUSECLR;

    printf("Reset cause: 0x%08lX\n", (unsigned long)rstCause);

    if (rstCause & EMU_RSTCAUSE_POR) {
        printf("Cold start\n");
        initBURTC_cold();
        
    } else if (rstCause & EMU_RSTCAUSE_EM4) {
        printf("EM4 wakeup\n");
        initBURTC_em4wake();
        
    } else {
        // Pin reset, software reset, etc.
        // BURAM already holds last saved total from app_process_action()
        printf("Cold Start or Other Reset\n");
        CMU_ClockEnable(cmuClock_BURAM, true);
        BURAM->RET[0].REG = 0;  // Clear accumulated time on fresh power
        initBURTC_cold();
        
    }
}