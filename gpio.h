#include "LNChecker.h"

/*************** ⚡ NEOPIXELS ***************/

//Pin button
#define PIN_BUTTON_1      2

//Pin conectado al NeoPixel
#define LED_PIN           10      // Pin - leds neopixel
#define LED_COUNT         25       // Neopixel number of leds
#define COLOR_RED         0xFF0000
#define COLOR_GREEN       0x00FF00
#define COLOR_BLUE        0x0000FF
#define COLOR_ORANGE      0xFF9700
#define COLOR_YELLOW      0xFFFF00
#define COLOR_PINK        0xFF00FF
#define COLOR_CIAN        0x00FFFF
#define COLOR_NONE        0x000000
#define BLINK_Ms          500

#define MAX_COLOR         7
//Array de valores entre el color configurado/NeoPixel
//                                0             1           2 ....
long const boltArrayColors[9]={COLOR_YELLOW, COLOR_YELLOW, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_ORANGE, COLOR_PINK, COLOR_CIAN};

// setting PWM properties
#define freq         5000
#define ledChannel   0
#define resolution   8
#define initDuty     50
#define MAX          180
#define MIN          0

void LedSetup(void);
void Ledloop(void);
void showInitEffect(void);
void setLedsColor(int ledColor);
void blinkBoltLeds(uint32_t color);
void runMonitor(void *name);
void changeBoltColor(void);
void changeBoltEffect(void);
void setBoltColor(void);
void rainbowBolt(int wait);
void snakeBolt(int velocity);
void doQueryLed(void);

