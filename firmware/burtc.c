#include "burtc.h"
#include "efr32bg24_emu.h"
#include "em_burtc.h"
#include "sl_sleeptimer.h"
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
  burtcInit.clkDiv = 512;
  BURTC_Init(&burtcInit);

  BURTC_CounterReset();
  BURTC_CompareSet(0, 0xFFFFFFFF);

  BURTC_Stop();
  //BURTC->CNT = 1782833100 * (1000/512);
  BURTC->CNT = (uint32_t)(((uint64_t)1782849300 * 1000) / 512);
  BURTC_Start();

  BURTC_IntClear(BURTC_IF_COMP);
  BURTC_IntEnable(BURTC_IEN_COMP);
  NVIC_EnableIRQ(BURTC_IRQn);
  BURTC_Enable(true);
  BURTC_Start();
}

void initBURTC_em4wake(void)
{
  CMU_ClockEnable(cmuClock_BURAM, true);
  CMU_ClockEnable(cmuClock_BURTC, true);

  //Address the Phantom Ticks on EM4 Wakeup
  BURTC_Stop();
  BURTC->CNT = BURAM->RET[0].REG + BURTC_IRQ_PERIOD; // + 80; //77 - 3 seconds too slow
  BURTC_Start();

  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);
  
  BURTC_IntClear(BURTC_IF_COMP);
  BURTC_IntEnable(BURTC_IEN_COMP);
  NVIC_EnableIRQ(BURTC_IRQn);

  // printf("hi");
}

void scheduleBURTC_em4(void) {
  uint32_t next = BURTC_CounterGet() + BURTC_IRQ_PERIOD;
  BURTC_CompareSet(0, next);
}

static int is_leap(int y)
{
  return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

static int days_in_month(int m, int y)
{
  static const int days[] =
    {31,28,31,30,31,30,31,31,30,31,30,31};

  if (m == 2) return days[m-1] + is_leap(y);
  return days[m-1];
}

void print_time(void)
{
  //uint32_t total_seconds = BURTC_CounterGet() / 1000;
  //uint32_t total_seconds = BURTC_CounterGet() * 512 / 1000;
  uint32_t total_seconds = ((uint64_t)BURTC_CounterGet() * 512) / 1000;
  //total_seconds = 1782833100;

  //if (unix_epoch_offset == 0) {
  //  uart_send_string("Clock not set. Please enter date/time.\r\n");
  //  return;
  //}

  int year = 1970;
  int month = 1;

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

  char buffer[64]; // Make sure buffer is large enough

  sprintf(buffer, "Current Date: %04d-%02d-%02d | Time: %02lu:%02lu:%02lu UTC\r\n",
          year, month, day, hour, min, sec);

  //uart_send_string(buffer);
  printf(buffer, sizeof(buffer),
         "Current Date: %04d-%02d-%02d | Time: %02lu:%02lu:%02lu UTC\r\n",
         year, month, day, hour, min, sec);
}