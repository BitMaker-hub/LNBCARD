#include <Adafruit_NeoPixel.h>
#include "gpio.h"
#include "configs.h"



// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

/***********************💊 LED EFFECTS ******************************/
void LedSetup() {
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(40); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
void colorWipeBack(uint32_t color, int wait) {
  for(int i=strip.numPixels(); i>0; i--) { // For each pixel in strip...
    strip.setPixelColor(i-1, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 1*65536; firstPixelHue += 256) {
    strip.rainbow(firstPixelHue);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

//Init efect on leds
void showInitEffect(){
                            //RED-GREEN-BLUE
  //uint32_t color = strip.Color(0,   255,   0); //Green
  rainbow(6); 
  setLedsColor(COLOR_NONE);
  delay(100);

  uint32_t color = COLOR_YELLOW;
  /*switch(myPill.State){
     case STATE_VIRGIN:  color = COLOR_GREEN; break;
     case STATE_LOCKED: color = COLOR_BLUE; break;
     case STATE_UNLOCKED: color = COLOR_RED; break;
  }*/
  
  colorWipe(color, 50);
  delay(100);
  colorWipe(COLOR_NONE, 50);
  colorWipeBack(color, 50);
  delay(100);
  colorWipeBack(COLOR_NONE, 50);
  delay(500);
  
  //Print Leds Config +  Online
  strip.setPixelColor(0, COLOR_GREEN);
  strip.setPixelColor(1, COLOR_GREEN);
  strip.show(); 
}

void setLedsColor(int ledColor) {
  for(int i=0; i<LED_COUNT; i++){
    strip.setPixelColor(i, ledColor);
  }
  strip.show();
}

//Blink all 3 leds 
void blinkBoltLeds(uint32_t color){
    
  //First BLINK
  for(int i=3; i<LED_COUNT; i++) strip.setPixelColor(i, color);
  strip.show();
  delay(100);
  for(int i=3; i<LED_COUNT; i++) strip.setPixelColor(i, COLOR_NONE);
  strip.show();
  delay(100);

  //Second BLINK
  for(int i=3; i<LED_COUNT; i++) strip.setPixelColor(i, color);
  strip.show();
  delay(100);
  for(int i=3; i<LED_COUNT; i++) strip.setPixelColor(i, COLOR_NONE);
  strip.show();
  delay(100);
  
  //Third BLINK
  for(int i=3; i<LED_COUNT; i++) strip.setPixelColor(i, color);
  strip.show();
  delay(200);
  for(int i=3; i<LED_COUNT; i++) strip.setPixelColor(i, COLOR_NONE);
  strip.show();
  delay(200);

  //Fourth BLINK
  for(int i=3; i<LED_COUNT; i++) strip.setPixelColor(i, color);
  strip.show();
  delay(100);
  for(int i=3; i<LED_COUNT; i++) strip.setPixelColor(i, COLOR_NONE);
  strip.show();
  delay(100);
  delay(500);
}




