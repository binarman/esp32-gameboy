#include "SPI.h"
#include <Arduino_GFX_Library.h>

#define _cs   22   // 3 goes to TFT CS
#define _dc   21   // 4 goes to TFT DC
#define _mosi 23  // 5 goes to TFT MOSI
#define _sclk 19  // 6 goes to TFT SCK/CLK
#define _rst  18  // ESP RST to TFT RESET
#define _miso 25    // Not connected
#define _led   5
//       3.3V     // Goes to TFT LED  
//       5v       // Goes to TFT Vcc
//       Gnd      // Goes to TFT Gnd        

Arduino_DataBus *bus = new Arduino_ESP32SPI(_dc, _cs, _sclk, _mosi, _miso);
Arduino_GFX *tft = new Arduino_ILI9341(bus, _rst, 3 /* rotation */);

void backlighting(bool state) {
  if (!state) {
    digitalWrite(_led, LOW);
  }
  else {
    digitalWrite(_led, HIGH);
  }
}

#define GAMEBOY_HEIGHT 144
#define GAMEBOY_WIDTH 160
#define DRAW_HEIGHT 144
#define DRAW_WIDTH 160
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320

#define TILES 4

byte pixels[GAMEBOY_HEIGHT * GAMEBOY_WIDTH/4];

static uint16_t frame_buffer[DRAW_HEIGHT * DRAW_HEIGHT / TILES];

static int button_start, button_select, button_a, button_b, button_down, button_up, button_left, button_right;

byte getColorIndexFromFrameBuffer(int x, int y) {
  int offset = x + y * GAMEBOY_WIDTH;
  return (pixels[offset >> 2] >> ((offset & 3) << 1)) & 3;
}
const int color[] = {0x0000, 0x5555, 0xAAAA, 0xFFFF};


void SDL_Flip(byte *screen){
  uint16_t row[DRAW_WIDTH];
  int h_offset = (SCREEN_WIDTH-DRAW_WIDTH)/2;
  int v_offset = (SCREEN_HEIGHT-DRAW_HEIGHT)/2;
  for (int tile = 0; tile < TILES; tile++) {
    for (int j = 0; j < DRAW_HEIGHT/TILES; j++) {
      int orig_y = tile * GAMEBOY_HEIGHT/TILES + j * GAMEBOY_HEIGHT / DRAW_HEIGHT;
      for (int i = 0; i < DRAW_WIDTH; i++) {
        int orig_x = i * GAMEBOY_WIDTH / DRAW_WIDTH;
        frame_buffer[j*DRAW_WIDTH + i] = color[getColorIndexFromFrameBuffer(orig_x, orig_y)];
      }
    }
    tft->draw16bitRGBBitmap(h_offset, v_offset + tile*DRAW_HEIGHT/TILES, frame_buffer, DRAW_WIDTH, DRAW_HEIGHT/TILES);
  }
}

void sdl_init(void)
{
  // GFX_EXTRA_PRE_INIT();
  tft->begin();
  pinMode(_led, OUTPUT);
  backlighting(true);
  tft->fillScreen(RED);

  gpio_num_t gpios [] = {GPIO_NUM_2, GPIO_NUM_14, GPIO_NUM_27, GPIO_NUM_26, GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_15, GPIO_NUM_35};
  for (gpio_num_t pin: gpios) {
    gpio_pad_select_gpio(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    // uncomment to use builtin pullup resistors
//    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
  }
}

int sdl_update(void){
  button_up = !gpio_get_level(GPIO_NUM_2);
  button_left = !gpio_get_level(GPIO_NUM_14);
  button_down = !gpio_get_level(GPIO_NUM_27);
  button_right = !gpio_get_level(GPIO_NUM_26);

  button_start = !gpio_get_level(GPIO_NUM_15);
  button_select = !gpio_get_level(GPIO_NUM_35);

  button_a = !gpio_get_level(GPIO_NUM_33);
  button_b = !gpio_get_level(GPIO_NUM_32);
	return 0;
}

unsigned int sdl_get_buttons(void)
{
	unsigned int buttons = (button_start*8) | (button_select*4) | (button_b*2) | button_a;
  return buttons;
}

unsigned int sdl_get_directions(void)
{
	return (button_down*8) | (button_up*4) | (button_left*2) | button_right;
}

byte* sdl_get_framebuffer(void)
{
	return pixels;
}

void sdl_frame(void)
{
  /*
	if(frames == 0)
		gettimeofday(&tv1, NULL);
	
	frames++;
	if(frames % 1000 == 0)
	{
		gettimeofday(&tv2, NULL);
		printf("Frames %d, seconds: %d, fps: %d\n", frames, tv2.tv_sec - tv1.tv_sec, frames/(tv2.tv_sec - tv1.tv_sec));
	}
 */
	SDL_Flip(pixels);
}
