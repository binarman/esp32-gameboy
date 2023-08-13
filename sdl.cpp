

/*
#include <SDL/SDL.h>
#include <sys/time.h>
static SDL_Surface *screen;
static unsigned int frames;
static struct timeval tv1, tv2;



static int button_start, button_select, button_a, button_b, button_down, button_up, button_left, button_right;

void sdl_init(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
}

int sdl_update(void)
{
	SDL_Event e;

	while(SDL_PollEvent(&e))
	{
		if(e.type == SDL_QUIT)
			return 1;

		if(e.type == SDL_KEYDOWN)
		{
			switch(e.key.keysym.sym)
			{
				case SDLK_a:
					button_a = 1;
				break;
				case SDLK_s:
					button_b = 1;
				break;
				case SDLK_d:
					button_select = 1;
				break;
				case SDLK_f:
					button_start = 1;
				break;
				case SDLK_LEFT:
					button_left = 1;
				break;
				case SDLK_RIGHT:
					button_right = 1;
				break;
				case SDLK_DOWN:
					button_down = 1;
				break;
				case SDLK_UP:
					button_up = 1;
				break;
			}
		}

		if(e.type == SDL_KEYUP)
		{
			switch(e.key.keysym.sym)
			{
				case SDLK_a:
					button_a = 0;
				break;
				case SDLK_s:
					button_b = 0;
				break;
				case SDLK_d:
					button_select = 0;
				break;
				case SDLK_f:
					button_start = 0;
				break;
				case SDLK_LEFT:
					button_left = 0;
				break;
				case SDLK_RIGHT:
					button_right = 0;
				break;
				case SDLK_DOWN:
					button_down = 0;
				break;
				case SDLK_UP:
					button_up = 0;
				break;
			}
		}

	}
	return 0;
}

unsigned int sdl_get_buttons(void)
{
	return (button_start*8) | (button_select*4) | (button_b*2) | button_a;
}

unsigned int sdl_get_directions(void)
{
	return (button_down*8) | (button_up*4) | (button_left*2) | button_right;
}

unsigned int *sdl_get_framebuffer(void)
{
	return screen->pixels;
}

void sdl_frame(void)
{
	if(frames == 0)
  		gettimeofday(&tv1, NULL);
	
	frames++;
	if(frames % 1000 == 0)
	{
		gettimeofday(&tv2, NULL);
		printf("Frames %d, seconds: %d, fps: %d\n", frames, tv2.tv_sec - tv1.tv_sec, frames/(tv2.tv_sec - tv1.tv_sec));
	}
	SDL_Flip(screen);
}

void sdl_quit()
{
	SDL_Quit();
}
*/
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
#define DRAW_HEIGHT 216
#define DRAW_WIDTH 240
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320

byte pixels[GAMEBOY_HEIGHT * GAMEBOY_WIDTH / 4];

static int button_start, button_select, button_a, button_b, button_down, button_up, button_left, button_right;

byte getColorIndexFromFrameBuffer(int x, int y) {
  int offset = x + y * GAMEBOY_WIDTH;
  return (pixels[offset >> 2] >> ((offset & 3) << 1)) & 3;
}
const int color[] = {0x000000, 0x555555, 0xAAAAAA, 0xFFFFFF};

void SDL_Flip(byte *screen){
  uint16_t row[DRAW_WIDTH];
  int h_offset = (SCREEN_WIDTH-DRAW_WIDTH)/2;
  int v_offset = (SCREEN_HEIGHT-DRAW_HEIGHT)/2;
  for (int j = 0; j < DRAW_HEIGHT; j++) {
    int orig_y = j * GAMEBOY_HEIGHT / DRAW_HEIGHT;
    for (int i = 0; i < DRAW_WIDTH; i++) {
      int orig_x = i*GAMEBOY_WIDTH / DRAW_WIDTH;
      row[i] = color[getColorIndexFromFrameBuffer(orig_x, orig_y)];
    }
    tft->draw16bitRGBBitmap(h_offset, v_offset + j, row, DRAW_WIDTH, 1);
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
