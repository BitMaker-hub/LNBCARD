

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>
#include <OneButton.h>
#include "gpio.h"

#include "wManager.h"
#include "LNChecker.h"

#define CURRENT_VERSION "v1.0.1"

/********************** ⚡ GLOBAL Vars *******************************/
OneButton button1(PIN_BUTTON_1);
LNBCard myLNBcard;

/********* INIT *****/
void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(0);

  /******** INIT LNBCARD ************/
  Serial.println("LNBcard starting......");
  Serial.println("version " + String(CURRENT_VERSION));

  delay(200);

  //Check if need initial forcedConfig
  myLNBcard.forceConfig = !digitalRead(PIN_BUTTON_1);
  
  LedSetup(); //Initialize NeoPixel
  showInitEffect();

  // Setup the buttons
  button1.setPressMs(5000);
  button1.attachClick(changeBoltColor);
  button1.attachDoubleClick(changeBoltEffect);
  button1.attachLongPressStart(setBoltColor);
  //button1.attachMultiClick(alternateScreenState);


  /******** CREATE TASKs*****/
  Serial.println("");
  Serial.println("Initiating tasks...");
  char *name = (char*) malloc(32);
  sprintf(name, "(%s)", "Checker");
  Serial.println("Initiating [MONITOR] task...");
  BaseType_t res1 = xTaskCreatePinnedToCore(runMonitor, "Monitor", 10000, (void*)name, 4, NULL,1);
  Serial.println("Initiating [CHECKER] task...");
  BaseType_t res2 = xTaskCreatePinnedToCore(runChecker, "Checker", 5000, (void*)name, 4, NULL,1);

  /******** INIT WIFI ************/
  init_WifiManager();

  /******** Setup Checker after reading config ************/
  setupChecker();

}

void app_error_fault_handler(void *arg) {
  // Get stack errors
  char *stack = (char *)arg;

  // Print the stack errors in the console
  esp_log_write(ESP_LOG_ERROR, "APP_ERROR", "Pila de errores:\n%s", stack);

  // restart ESP32
  esp_restart();
}

void loop() {
  // keep watching the push buttons:
  button1.tick();
  
  wifiManagerProcess(); // avoid delays() in loop when non-blocking and other long running code  

  vTaskDelay(50 / portTICK_PERIOD_MS);
}

