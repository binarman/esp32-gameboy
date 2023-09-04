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
  #ifdef PERF_REPORT
  static uint32_t prev_loop_exit = 0;
  uint32_t loop_start = ESP.getCycleCount();
  static int count = 0;
  static int total_cpu = 0;
  static int total_lcd = 0;
  static int total_sdl = 0;
  static int total_timer = 0;
  static int total_outside_loop = 0;
  static int sdl_count = 0;
  static int eumlator_cpu_cycle_begin = 0;
  uint32_t adjust_start = ESP.getCycleCount();
  uint32_t cpu_start = ESP.getCycleCount();
  #endif

  while (!screen_updated) {
    unsigned int cycles = cpu_cycle();

    #ifdef PERF_REPORT
    uint32_t lcd_start = ESP.getCycleCount();
    #endif

    screen_updated = lcd_cycle(cycles);

    #ifdef PERF_REPORT
    uint32_t sdl_start = ESP.getCycleCount();
    #endif

    if (screen_updated) {
      sdl_update();
      #ifdef PERF_REPORT
      sdl_count += 1;
      #endif
    }

    #ifdef PERF_REPORT
    uint32_t timer_start = ESP.getCycleCount();
    #endif

    timer_cycle(cycles);

    #ifdef PERF_REPORT
    uint32_t finish = ESP.getCycleCount();

    int adjust = cpu_start - adjust_start;
    count++;
    total_cpu += lcd_start - cpu_start - adjust;
    total_lcd += sdl_start - lcd_start - adjust;
    total_sdl += timer_start - sdl_start - adjust;
    total_timer += finish - timer_start - adjust;
    total_outside_loop += loop_start - prev_loop_exit - adjust;

    if (count >= 500000) {
      Serial.print("cpu avg: "); Serial.print(total_cpu/count); Serial.println("");
      Serial.print("lcd avg: "); Serial.print(total_lcd/count); Serial.println("");
      Serial.print("sdl avg: "); Serial.print(total_sdl/sdl_count); Serial.println("");
      Serial.print("timer avg: "); Serial.print(total_timer/count); Serial.println("");
      Serial.print("outside loop avg: "); Serial.print(total_outside_loop/count); Serial.println("");
      float perf_ratio = (1050000.0f/240000000.0f) * (total_cpu+total_lcd+total_sdl+total_timer)/(cycles - eumlator_cpu_cycle_begin);
      Serial.print("emulator/real hardware ratio: "); Serial.print(perf_ratio); Serial.println("");
      Serial.println("");
      count = 0;
      sdl_count = 0;
      eumlator_cpu_cycle_begin = cycles;
      total_cpu = total_lcd = total_timer = total_sdl = total_outside_loop = 0;
    }
    prev_loop_exit = ESP.getCycleCount();
    #endif
  }
}
