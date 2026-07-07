#include "burtc.h"
#include "efr32bg24_emu.h"
#include "em_burtc.h"
#include "sl_sleeptimer.h"
#include <stdint.h>
#include "em_emu.h"
#include "em_cmu.h"
#include "SEGGER_RTT.h"
#include <stdlib.h>

void initBURTC(void) {
  //Detect Reset Cause
  uint32_t rstCause = EMU->RSTCAUSE;
  EMU->CMD = EMU_CMD_RSTCAUSECLR;

  //EM4 Reset
  if (rstCause == EMU_RSTCAUSE_EM4) {
    printf("EM4 Wakeup\n");
    initBURTC_em4wake();
  }
  //Other Reset Causes
  else {
    printf("Reset cause: 0x%08lX\n", (unsigned long)rstCause);

    if (rstCause & EMU_RSTCAUSE_POR)        { printf("Power On Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_PIN)        { printf("Pin Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_EM4)        { printf("EM4 Wakeup Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_WDOG0)      { printf("Watchdog 0 Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_WDOG1)      { printf("Watchdog 1 Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_LOCKUP)     { printf("M33 Core Lockup Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_SYSREQ)     { printf("Software System Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_DVDDBOD)    { printf("DVDD BOD Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_DVDDLEBOD)  { printf("DVDD LE BOD Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_DECBOD)     { printf("DECOUPLE BOD Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_AVDDBOD)    { printf("AVDD BOD Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_IOVDD0BOD)  { printf("IOVDD0 BOD Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_SETAMPER)   { printf("SE Tamper Event Reset\n"); }
    if (rstCause & EMU_RSTCAUSE_VREGIN)     { printf("DCDC VREGIN Comparator Reset\n"); }

    initBURTC_cold(rstCause);
  }
}

void update_time(uint32_t new_time) {
  BURTC_Stop();
  BURTC->CNT = new_time;
  BURTC_Start();
}

void initBURTC_cold(uint32_t rstCause) {
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_BURTC, true);

  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.start = false;  
  burtcInit.em4comp = true;
  burtcInit.clkDiv = 32768;
  BURTC_Init(&burtcInit);

  BURTC_Stop();
  BURTC->CNT = 1;

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

void em4_time(void) {
  GPIO_PinOutClear(gpioPortA, 8); //Turn off LED
  sl_sleeptimer_delay_millisecond(10); //Allows Prints to Work
  scheduleBURTC_em4();
  sl_power_manager_enter_em4();
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

void check_time_sync_input(void)
{
    static char time_buf[16];
    static size_t time_buf_len = 0;

    char c;
    // Non-blocking: reads whatever's available, 0 if nothing waiting
    while (SEGGER_RTT_Read(0, &c, 1) == 1) {
        if (c == '\n' || c == '\r') {
            if (time_buf_len > 0) {
                time_buf[time_buf_len] = '\0';
                uint32_t new_time = (uint32_t)strtoul(time_buf, NULL, 10);
                if (new_time > 0) {
                    update_time(new_time);
                    printf("Time synced: %lu\n", (unsigned long)new_time);
                }
                time_buf_len = 0; // reset for next entry
            }
        } else if (time_buf_len < sizeof(time_buf) - 1) {
            time_buf[time_buf_len++] = c;
        } else {
            // overflow guard: input too long, discard and reset
            time_buf_len = 0;
        }
    }
}