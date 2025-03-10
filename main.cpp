#include <stdio.h>

#include "cpu.h"
#include "lcd.h"
#include "mem.h"
#include "rom.h"
#include "sdl.h"
#include "timer.h"

int main(int argc, char *argv[]) {
#ifdef BUILD_FOR_PC
  int r;
  const char usage[] = "Usage: %s <rom>\n";

  if (argc != 2) {
    fprintf(stderr, usage, argv[0]);
    return 0;
  }

  r = rom_load(argv[1]);
  if (!r) return 0;

  sdl_init();

  printf("ROM OK!\n");

  gameboy_mem_init();
  printf("Mem OK!\n");

  cpu_init();
  printf("CPU OK!\n");

  while (1) {
    if (!cpu_cycle()) break;

    if (!lcd_cycle()) break;

    timer_cycle();
  }

  sdl_quit();
#endif
  return 0;
}
