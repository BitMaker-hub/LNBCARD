#include <Adafruit_NeoPixel.h>
#include "gpio.h"
#include "LNCHecker.h"



// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

extern LNBCard myLNBcard;

/***********************💊 LED EFFECTS ******************************/
void LedSetup() {
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(40); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void colorWipe(int initLed, uint32_t color, int wait) {
  for(int i=initLed; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
void colorWipeBack(int initLed, uint32_t color, int wait) {
  for(int i=strip.numPixels(); i>initLed; i--) { // For each pixel in strip...
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
  
  colorWipe(0, color, 50);
  delay(100);
  colorWipe(0, COLOR_NONE, 50);
  colorWipeBack(0, color, 50);
  delay(100);
  colorWipeBack(0, COLOR_NONE, 50);
  delay(500);
  
  //Print Leds Config +  Online
  //strip.setPixelColor(0, COLOR_GREEN);
  //strip.setPixelColor(1, COLOR_GREEN);
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

  //Change current color chosen for RGB led color
  int iColor = myLNBcard.Settings.BoltColor;
  if(iColor > MAX_COLOR) iColor = 0;
  
  color = boltArrayColors[iColor]; 
  
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

/********************* Rainbow effect only on bolt ****************/
long colorWheel(int WheelPos ){
    long r,g,b;
    WheelPos = 255 - WheelPos;

    if ( WheelPos < 85 ) {
        r = 255 - WheelPos * 3;
        g = 0;
        b = WheelPos * 3;
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        r = 0;
        g = WheelPos * 3;
        b = 255 - WheelPos * 3;
    } else {
        WheelPos -= 170;
        r = WheelPos * 3;
        g = 255 - WheelPos * 3;
        b = 0;
    }
    // returns a string with the rgb value to be used as the parameter
    return ((r<<16) + (g<<8) + b);
}

void rainbowBolt(int wait) { 
  long showColor;
  int cwi = 0; // colour wheel index (current position on colour wheel)
  for(cwi=0; cwi<600;cwi++){
    for(int i = 3; i < LED_COUNT; i++) {
        showColor = colorWheel( ( cwi+(i*10)) & 255 );
        strip.setPixelColor(i, showColor );
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
  for(int i=3; i<LED_COUNT; i++) strip.setPixelColor(i, COLOR_NONE);
  strip.show();
}

/********************************* BOLt effect on/off **********************+
 *  
 */

void snakeBolt(int velocity){
  //Choose current color chosen for RGB led color
  long color = boltArrayColors[myLNBcard.Settings.BoltColor]; 
  
  colorWipe(0, color, velocity);
  delay(100);
  colorWipe(0, COLOR_NONE, velocity);
  colorWipeBack(0, color, velocity);
  delay(100);
  colorWipeBack(0, COLOR_NONE, velocity);
}
/********************************* Bolt effects & colors ***********************/

void changeBoltColor(void){
  myLNBcard.Settings.BoltColor++;
  if(myLNBcard.Settings.BoltColor>8) myLNBcard.Settings.BoltColor =1;

  //Change current color chosen for RGB led color
  long boltColor = boltArrayColors[myLNBcard.Settings.BoltColor]; 
  for(int i=3; i<LED_COUNT; i++) strip.setPixelColor(i, boltColor);
  strip.show();
  
}

int effect = 0;
void changeBoltEffect(void){

  myLNBcard.isStripBlocked = true; //Block other processes update strip leds
  switch(effect){
    case 0: blinkBoltLeds(0); break;
    case 1: rainbowBolt(6); break;
    case 2: snakeBolt(6); break;
  }
  myLNBcard.isStripBlocked = false; //Allow other processes update strip leds
  if(effect++>=2) effect=0;
}
void setBoltColor(void){}
/********************* MONITOR TASK ***********************/

bool onQuery = false;
void doQueryLed(void){
  onQuery=true;
}

void runMonitor(void *name){
  
  Serial.println("[MONITOR] started");
  unsigned long start = millis();
  unsigned long timeoutConnecting = millis();
  long tDisconnected;
  bool blink = false; 
  
  
  while(1){
    
    if((millis()-start) >= BLINK_Ms) {
      start = millis();
      blink = !blink;
    }
    switch(myLNBcard.LNBStatus){
      case LNB_waitingConfig:   //Led Config
                                if(blink) strip.setPixelColor(0, COLOR_BLUE);
                                else strip.setPixelColor(0, COLOR_NONE);
                                //Led Online
                                strip.setPixelColor(1, COLOR_NONE);
                                if(!myLNBcard.isStripBlocked) strip.show(); 
                                break;
                                
      case LNB_Connecting:      //Led Config
                                strip.setPixelColor(0, COLOR_GREEN);
                                //Led Online
                                if(blink) strip.setPixelColor(1, COLOR_BLUE);
                                else strip.setPixelColor(1, COLOR_NONE);
                                //Led effect when Wifi is lost
                                tDisconnected = millis() - timeoutConnecting;
                                if(tDisconnected > 10000) strip.setPixelColor(1, COLOR_RED);
                                if(tDisconnected > 13000) timeoutConnecting = millis();
                                if(!myLNBcard.isStripBlocked) strip.show(); 
                                break;
                                
      case LNB_Connected:       //Led Config
                                strip.setPixelColor(0, COLOR_GREEN);
                                //Led Online
                                strip.setPixelColor(1, COLOR_GREEN);
                                //Led Query
                                if(myLNBcard.isLNBitsWallet){
                                  //When LNBits method Pink led
                                  if(myLNBcard.isWSconnected){
                                    //While connecting WebSocket pink led blinking
                                    if(blink) strip.setPixelColor(2, COLOR_PINK);
                                    else strip.setPixelColor(2, COLOR_NONE);
                                  }
                                  else strip.setPixelColor(2, COLOR_PINK);
                                }else{
                                  //LNDhub method getALby Orange led
                                  if(onQuery){
                                    strip.setPixelColor(2, COLOR_NONE);
                                    onQuery = false;
                                    vTaskDelay(500 / portTICK_PERIOD_MS);
                                  }else strip.setPixelColor(2, COLOR_ORANGE);
                                }
                                if(!myLNBcard.isStripBlocked) strip.show(); 
                                break;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}




