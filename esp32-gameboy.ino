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
  bool screen_updated = false;
  while (!screen_updated) {
    #ifdef PERF_REPORT
    static uint32_t prev_loop_exit = 0;
    uint32_t loop_start = ESP.getCycleCount();
    static int count = 0;
    static int total_cpu = 0;
    static int total_lcd = 0;
    static int total_timer = 0;
    static int total_outside_loop = 0;

    uint32_t adjust_start = ESP.getCycleCount();
    uint32_t cpu_start = ESP.getCycleCount();
    #endif
    unsigned int cycles = cpu_cycle();

    #ifdef PERF_REPORT
    uint32_t lcd_start = ESP.getCycleCount();
    #endif

    screen_updated = lcd_cycle(cycles);

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
    total_outside_loop += loop_start - prev_loop_exit - adjust;

    if (count >= 500000) {
      Serial.print("cpu: "); Serial.print(total_cpu/count); Serial.println("");
      Serial.print("lcd: "); Serial.print(total_lcd/count); Serial.println("");
      Serial.print("timer: "); Serial.print(total_timer/count); Serial.println("");
      Serial.print("outside loop: "); Serial.print(total_outside_loop/count); Serial.println("");
      Serial.println("");
      count = 0;
      total_cpu = total_lcd = total_timer = total_outside_loop = 0;
    }
    prev_loop_exit = ESP.getCycleCount();
    #endif
  }
}
