#include <stdio.h>
#include "timer.h"
#include "rom.h"
#include "mem.h"
#include "cpu.h"
#include "lcd.h"
#include "sdl.h"
#include "gbrom.h"
#include <esp32-hal.h>

void setup() {
  int r = rom_init(gb_rom);

  sdl_init();

  gameboy_mem_init();

  cpu_init();

  Serial.begin(115200);
}

#define PERF_REPORT

void loop() {
  #ifdef PERF_REPORT
  static int count = 0;
  static int total_cpu = 0;
  static int total_lcd = 0;
  static int total_timer = 0;

  uint32_t adjust_start = ESP.getCycleCount();
  uint32_t cpu_start = ESP.getCycleCount();
  #endif

  unsigned int cycles = cpu_cycle();

  #ifdef PERF_REPORT
  uint32_t lcd_start = ESP.getCycleCount();
  #endif

  lcd_cycle(cycles);

  #ifdef PERF_REPORT
  uint32_t timer_start = ESP.getCycleCount();
  #endif

  timer_cycle(cycles);

  #ifdef PERF_REPORT
  uint32_t finish = ESP.getCycleCount();

  int adjust = cpu_start - adjust_start;
  count++;
  total_cpu += lcd_start - cpu_start - adjust;
  total_lcd += timer_start - lcd_start - adjust;
  total_timer += finish - timer_start - adjust;

  if (count >= 500000) {
    Serial.print("cpu: "); Serial.print(total_cpu/count); Serial.println("");
    Serial.print("lcd: "); Serial.print(total_lcd/count); Serial.println("");
    Serial.print("timer: "); Serial.print(total_timer/count); Serial.println("");
    Serial.println("");
    count = 0;
    total_cpu = total_lcd = total_timer = 0;
  }
  #endif
}
