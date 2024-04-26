#define ESP_DRD_USE_SPIFFS true

// Include Libraries
#include <WiFi.h>

#include <WiFiManager.h>

#include "wManager.h"
#include "gpio.h"
#include "LNChecker.h" 

// Flag for saving data
bool shouldSaveConfig = false;

// Variables to hold data from custom textboxes
TSettings Settings;

// Define WiFiManager Object
WiFiManager wm;

nvMemory nvMem;
extern LNBCard myLNBcard;

void saveConfigCallback()
// Callback notifying us of the need to save configuration
{
    Serial.println("Should save config");
    shouldSaveConfig = true;
    //wm.setConfigPortalBlocking(false);
}

void configModeCallback(WiFiManager* myWiFiManager)
// Called when config mode launched
{
    Serial.println("Entered Configuration Mode");

    Serial.print("Config SSID: ");
    Serial.println(myWiFiManager->getConfigPortalSSID());

    Serial.print("Config IP Address: ");
    Serial.println(WiFi.softAPIP());
}

void reset_configuration()
{
    Serial.println("Erasing Config, restarting");
    nvMem.deleteConfig();
    wm.resetSettings();
    ESP.restart();
}

void init_WifiManager()
{

    Serial.begin(115200);

    // Change to true when testing to force configuration every time we run
    //bool forceConfig = false;


    // Check if button2 is pressed to enter configMode with actual configuration
    if (!digitalRead(PIN_BUTTON_1) || myLNBcard.forceConfig) {
        Serial.println(F("Button pressed to force start config mode"));
        myLNBcard.forceConfig = true;
        wm.setBreakAfterConfig(true); //Set to detect config edition and save
    }

    // Explicitly set WiFi mode
    WiFi.mode(WIFI_STA);

    if (!nvMem.loadConfig(&myLNBcard.Settings))
    {
        //No config file on internal flash.
        myLNBcard.forceConfig = true;
    };

    // Reset settings (only for development)
    //wm.resetSettings();

    //Set dark theme
    //wm.setClass("invert"); // dark theme

    // Set config save notify callback
    wm.setSaveConfigCallback(saveConfigCallback);

    // Set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wm.setAPCallback(configModeCallback);

    //Advanced settings
    wm.setConfigPortalBlocking(false); //Hacemos que el portal no bloquee el firmware
    //wm.setConnectTimeout(50); // how long to try to connect for before continuing
    
    // Custom elements

    WiFiManagerParameter custom_Title1("<strong>Connection method 1 - LNBits (default)</strong><br><br>");

    // Text box (String) - 80 characters maximum
    WiFiManagerParameter LNURL_text_box("LNURLServer", "legend.lnbits.com", myLNBcard.Settings.LNURLServer.c_str(), 150);
    
    // Text box (String) - 80 characters maximum
    WiFiManagerParameter LNWallet_text_box("LNURLWallet", "Your LNBITs LNURLWallet", myLNBcard.Settings.LNURLWallet.c_str(), 150);

    WiFiManagerParameter custom_Title2("<hr><br><strong>Connection method 2 - LNDHUB GetAlby</strong><br><i>Set a [0] on LNBits wallet to use this connectivity method</i><br><br>");
    
    // Text box (String) - 80 characters maximum
    WiFiManagerParameter LNURI_text_box("LNDHubURI", "Your LNDHUBuri used by getAlby wallet", myLNBcard.Settings.LNDHUBuri.c_str(), 200);

    WiFiManagerParameter custom_Title3("<hr><br>⚡<strong>Bolt Color/Effects</strong>⚡<br><br>");  \
    
    
    // Need to convert numerical input to string to display the default value.
    char convertedValue[6];
    sprintf(convertedValue, "%d", myLNBcard.Settings.BoltColor);

    // Text box (Number) - 7 characters maximum
    WiFiManagerParameter BoltColor_box_num("BoltColor", "Bolt notification color", convertedValue, 2);
    WiFiManagerParameter custom_legend1("<i>[1]Yellow, [2]Red, [3]Green, [4]Blue, [5]Orange, [6]Pink, [7]Cian</i><br><br>");

    // Need to convert numerical input to string to display the default value.
    sprintf(convertedValue, "%d", myLNBcard.Settings.BoltEffect);

    // Text box (Number) - 7 characters maximum
    WiFiManagerParameter BoltEffect_box_num("BoltEffect", "Bolt notification effect", convertedValue, 2);
    WiFiManagerParameter custom_legend2("<i>[1]Blink, [2]Rainbow, [3]Flash</i><br><br>");

    WiFiManagerParameter boltColors_html(strHtmlBoltColors);
  
    // Add all defined parameters
    wm.addParameter(&custom_Title1);
    wm.addParameter(&LNURL_text_box);
    wm.addParameter(&LNWallet_text_box);
    wm.addParameter(&custom_Title2);
    wm.addParameter(&LNURI_text_box);
    wm.addParameter(&custom_Title3);
    wm.addParameter(&BoltColor_box_num);
    wm.addParameter(&custom_legend1);
    wm.addParameter(&BoltEffect_box_num);
    wm.addParameter(&custom_legend2);
    //wm.addParameter(&boltColors_html);

    Serial.println("AllDone: ");
    if (myLNBcard.forceConfig)    
    {
        myLNBcard.LNBStatus = LNB_waitingConfig;
        
        // Run if we need a configuration
        //No configuramos timeout al modulo
        wm.setConfigPortalBlocking(true); //Hacemos que el portal si bloquee el firmware
        wm.setBreakAfterConfig(true); //Hacemos que cierre el portal tras guardar configuración
        
        wm.startConfigPortal(DEFAULT_SSID, DEFAULT_WIFIPW); //Open portal and wait for config
       
        //Could be break forced after edditing, so save new config
        Serial.println("Saving new config...");
        myLNBcard.Settings.LNURLServer = LNURL_text_box.getValue();
        myLNBcard.Settings.LNURLWallet = LNWallet_text_box.getValue();
        myLNBcard.Settings.LNDHUBuri = LNURI_text_box.getValue();
        myLNBcard.Settings.BoltColor = atoi(BoltColor_box_num.getValue());
        myLNBcard.Settings.BoltEffect = atoi(BoltEffect_box_num.getValue());

        nvMem.saveConfig(&myLNBcard.Settings);
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        Serial.println("Restarting LNBCard...");
        ESP.restart();
        delay(5000);
    }
    else
    {
        //Tratamos de conectar con la configuración inicial ya almacenada
        myLNBcard.LNBStatus = LNB_Connecting;
        wm.setCaptivePortalEnable(false); // disable captive portal redirection
        if (!wm.autoConnect(myLNBcard.Settings.WifiSSID.c_str(), myLNBcard.Settings.WifiPW.c_str()))
        {
            Serial.println("Failed to connect and hit timeout");
            //delay(3000);
            // if we still have not connected restart and try all over again
            //ESP.restart();
            //delay(5000);
        }
    }

    myLNBcard.LNBStatus = LNB_Connecting;

    //Conectado a la red Wifi
    if (WiFi.status() == WL_CONNECTED) {
        myLNBcard.LNBStatus = LNB_Connected;
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        // Lets deal with the user config values

        // Copy the string value
        myLNBcard.Settings.LNURLServer = LNURL_text_box.getValue();
        Serial.print("LNURLServer: ");
        Serial.println(myLNBcard.Settings.LNURLServer);
        
        // Copy the string value
        myLNBcard.Settings.LNURLWallet = LNWallet_text_box.getValue();
        Serial.print("LNURLWallet: ");
        Serial.println(myLNBcard.Settings.LNURLWallet);

        // Copy the string value
        myLNBcard.Settings.LNDHUBuri = LNURI_text_box.getValue();
        Serial.print("LNDHUBuri: ");
        Serial.println(myLNBcard.Settings.LNDHUBuri);

        //Convert the number value
        myLNBcard.Settings.BoltColor = atoi(BoltColor_box_num.getValue());
        Serial.print("BoltColor: ");
        Serial.println(myLNBcard.Settings.BoltColor);

        //Convert the number value
        myLNBcard.Settings.BoltEffect = atoi(BoltEffect_box_num.getValue());
        Serial.print("BoltEffect: ");
        Serial.println(myLNBcard.Settings.BoltEffect);

    }

    // Save the custom parameters to FS
    if (shouldSaveConfig)
    {
        Serial.println("Saving config file..");
        nvMem.saveConfig(&myLNBcard.Settings);
    }
}

//----------------- MAIN PROCESS WIFI MANAGER --------------
int oldStatus = 0;

void wifiManagerProcess() {

    wm.process(); // avoid delays() in loop when non-blocking and other long running code

    int newStatus = WiFi.status();
    if (newStatus != oldStatus) {
        if (newStatus == WL_CONNECTED) {
            myLNBcard.LNBStatus = LNB_Connected;
            Serial.println("CONNECTED - Current ip: " + WiFi.localIP().toString());
        } else {
            myLNBcard.LNBStatus = LNB_Connecting;
            Serial.print("[Error] - current status: ");
            Serial.println(newStatus);
        }
        oldStatus = newStatus;
    }
}
