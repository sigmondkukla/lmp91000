#include "burtc.h"
#include "efr32bg24_emu.h"
#include "em_burtc.h"
#include "sl_sleeptimer.h"
#include <stdint.h>
#include "em_emu.h"
#include "em_cmu.h"

void initBURTC(void) {
  //Detect Reset Cause
  uint32_t rstCause = EMU->RSTCAUSE;
  EMU->CMD = EMU_CMD_RSTCAUSECLR;
  //printf("Reset cause: ");

  if (rstCause == EMU_RSTCAUSE_POR) {
    //Power On Reset
    printf("Power On\n");
    initBURTC_cold(rstCause);
  }
  else if (rstCause == EMU_RSTCAUSE_EM4) {
    //EM4 Reset
    printf("EM4 Wakeup\n");
    initBURTC_em4wake();
  }
  else if (rstCause == EMU_RSTCAUSE_SYSREQ) {
    //System Reset - not tested
    printf("System Reset\n");
    initBURTC_em4wake();
  }
  else if (rstCause == 0x00000042) {
    //Fresh Flash
    printf("Fresh Flash\n");
    initBURTC_cold(rstCause);
  }
  else {
    //Opening the SWO terminal without the --noreset options causes a pin reset which clears the BURTC counter
    printf("Reset cause: 0x%08lX\n", (unsigned long)rstCause);
    printf("Reconfiguring BURTC...\n");
    initBURTC_cold(rstCause);
  }
}

void initBURTC_cold(uint32_t rstCause) {
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_BURTC, true);

  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.start = false;  
  burtcInit.em4comp = true;
  burtcInit.clkDiv = 32768;
  BURTC_Init(&burtcInit);

  BURTC->CNT = CURRENT_UNIX_TIMESTAMP;

  BURTC_IntClear(BURTC_IF_COMP);
  BURTC_IntEnable(BURTC_IEN_COMP);
  NVIC_EnableIRQ(BURTC_IRQn);
  BURTC_Enable(true);
  BURTC_Start();
}

void initBURTC_em4wake(void) {
  CMU_ClockEnable(cmuClock_BURTC, true);
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFRCO);
  
  BURTC_IntClear(BURTC_IF_COMP);
  BURTC_IntEnable(BURTC_IEN_COMP);
  NVIC_EnableIRQ(BURTC_IRQn);
}

void scheduleBURTC_em4(void) {
  uint32_t next = BURTC_CounterGet() + BURTC_IRQ_PERIOD;
  BURTC_CompareSet(0, next);
}

static int is_leap(int y) {
  return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

static int days_in_month(int m, int y) {
  static const int days[] =
    {31,28,31,30,31,30,31,31,30,31,30,31};

  if (m == 2) return days[m-1] + is_leap(y);
  return days[m-1];
}

void print_time(void) {
  int year = 1970;
  int month = 1;
  uint32_t total_seconds = BURTC_CounterGet();

  // 1. Calculate Year
  while (1) {
    uint32_t seconds_in_year = (is_leap(year) ? 366 : 365) * 86400UL;
    if (total_seconds < seconds_in_year) break;
    total_seconds -= seconds_in_year;
    year++;
  }

  // 2. Calculate Month
  for (month = 1; month <= 12; month++) {
    uint32_t seconds_in_month = days_in_month(month, year) * 86400UL;
    if (total_seconds < seconds_in_month) break;
    total_seconds -= seconds_in_month;
  }

  // 3. Calculate Days, Hours, Mins, Secs
  int day = (total_seconds / 86400UL) + 1;
  uint32_t remaining = total_seconds % 86400UL;

  uint32_t hour = remaining / 3600UL;
  uint32_t min  = (remaining % 3600UL) / 60UL;
  uint32_t sec  = remaining % 60UL;

  printf("Current Date: %04d-%02d-%02d | Time: %02lu:%02lu:%02lu UTC\r\n", year, month, day, hour, min, sec);
}