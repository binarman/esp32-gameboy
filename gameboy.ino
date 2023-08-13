#include <stdio.h>
#include "timer.h"
#include "rom.h"
#include "mem.h"
#include "cpu.h"
#include "lcd.h"
#include "sdl.h"
#include "gbrom.h"


void setup() {
  // put your setup code here, to run once:
  int r = rom_init(gb_rom);

  sdl_init();

  // printf("ROM OK!\n");

  gameboy_mem_init();
  // printf("Mem OK!\n");

  cpu_init();
  // printf("CPU OK!\n");
  Serial.begin(115200);
}

void loop() {
    cpu_cycle();

    lcd_cycle();

    // timer_cycle();
}
