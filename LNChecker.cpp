#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <Arduino_JSON.h>
#include "LNChecker.h"
#include "gpio.h"

String payloadStr;
bool paid = false;
bool setupDone = false;

WebSocketsClient webSocket;

extern LNBCard myLNBcard;

/////////////////////////////////////////////////////////////////
/////////////////////// LNBITS stuff ////////////////////////////
/////////////////////////////////////////////////////////////////

/************** WEBSOCKET EVENT *****************/

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            Serial.printf("[WSc] Connected to url: %s\n",  payload);
            webSocket.sendTXT("Connected"); // send message to server when Connected
            break;
        case WStype_TEXT:
            payloadStr = (char*)payload;
            payloadStr.replace(String("'"), String('"'));
            payloadStr.toLowerCase();
            Serial.println("Received data from socket: " + payloadStr);
            paid = true;
        case WStype_ERROR:
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
            break;
    }
}

/*********************** SETUP LNBITS Socket *******************/
void setupLNBitsChecker(void){
  Serial.println("[CHECKER] Starting checker in LNBits Checker Mode");
  Serial.println("Connecting to websocket: ws://" + myLNBcard.Settings.LNURLServer + String(DEFAULT_API_URL) + myLNBcard.Settings.LNURLWallet);
  
  webSocket.beginSSL(myLNBcard.Settings.LNURLServer, 443, String(DEFAULT_API_URL) + myLNBcard.Settings.LNURLWallet);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(1000);

}

/*********** LNBITS CHECKER ****************/
void checkLNURL(void){
      webSocket.loop();
      //client.poll();
      if(paid){
          Serial.println("Paid");
          doBoltNotification();
          paid = false;
      }
}


/////////////////////////////////////////////////////////////////
/////////////////////// LNDHUB stuff ////////////////////////////
/////////////////////////////////////////////////////////////////

String refresh_token = "";
String access_token = "";
String login_auth = "";
String password_auth = "";


void setupLNDHubChecker(void){
  Serial.println("[CHECKER] Starting checker in LNDHub Checker Mode");
  String uri = myLNBcard.Settings.LNDHUBuri;
  
  String credentials = uri.substring(9,uri.indexOf("@"));
  login_auth = credentials.substring(0,credentials.indexOf(":"));
  password_auth = credentials.substring(credentials.indexOf(":")+1); 
  Serial.println("LNDHub credentials: "+ credentials + "\r\nLNDHub login_auth: " + login_auth + "\r\nLNDHub password_auth: " + password_auth);
}

/****************** DO FIRST AUTHORIZATION ****************/
bool doAuth(){
  WiFiClientSecure client;
  client.setInsecure();

  if (!client.connect(LNHUBServer, 443)){
    Serial.println("Client error connect LNHUB");
    return false;   
  }
  Serial.println("\r\n\r\n---------------- CONNECTED TO LNHUB >> GET auth ----------");
  
  String url = "/auth?type=auth";
  String rBody =  String("{\r\n") +
                  "\"login\": \"" + login_auth + "\",\r\n" +
                  "\"password\": \"" + password_auth + "\"\r\n" +
                  "}\r\n";
  String request = String("POST ") + url + " HTTP/1.1\r\n" +
                //HEADER PART
                  "Host: " + LNHUBServer + "\r\n" +
                  "User-Agent: ESP32\r\n" +
                  "Content-Type: application/json\r\n" +
                  "Content-Length: " + String(rBody.length()) + "\r\n" +
                  "Accept: */*\r\n" +
                  "Cache-Control: no-cache\r\n" +
                  "Connection: keep-alive\r\n\r\n" + 
                //BODY PART
                  rBody;
  
  Serial.println(request);
  client.print(request);
  
   while (client.connected()) {
   String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    if (line == "\r") {
      break;
    }
  }
  String line = client.readString();
  Serial.println(line);

  JSONVar myObject = JSON.parse(line);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return false;
  }
  
  //Obtain token vars and replace ["] character 
  refresh_token = JSON.stringify(myObject["refresh_token"]); 
  access_token = JSON.stringify(myObject["access_token"]);
  refresh_token.remove(0,1); //remove first character
  access_token.remove(0,1); //remove first character
  refresh_token.remove(refresh_token.length()-1,1); //remove last character
  access_token.remove(access_token.length()-1,1); //remove last character
  
  Serial.println("Current access_token: " + access_token);
  return true;
}

/*********** CHECK LNDHUB BALANCE ***************/
unsigned long checkBalance(void){
  WiFiClientSecure client;
  client.setInsecure();

  Serial.println("Connecting to LNDHUB at " + String(LNHUBServer) + " ...");

  unsigned long charBalance = 0;
  try {
    if (!client.connect(LNHUBServer, 443)){
      return -1;   
    }
    Serial.println("\r\n\r\n---------------- CONNECTED TO LNHUB >> GET BALANCE ----------");
    
    String url = "/balance";
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                  "Host: " + LNHUBServer + "\r\n" +
                  "Authorization: Bearer " + access_token +" \r\n" +
                  "User-Agent: ESP32\r\n" +
                  "Accept: */*\r\n" +
                  "Cache-Control: no-cache\r\n" +
                  "Connection: keep-alive\r\n\r\n"); 
  
     while (client.connected()) {
     String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
      if (line == "\r") {
        break;
      }
    }
    String line = client.readString();
    Serial.println(line);
    JSONVar myObject = JSON.parse(line);
  
    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return -1;
    }
    // Convierte el valor JSONVar a String primero
    String balanceStr = JSON.stringify(myObject["BTC"]["AvailableBalance"]);
    Serial.println("Current balance: " + String(balanceStr));
    char *endPtr;
    charBalance = strtoul(balanceStr.c_str(), &endPtr, 10);
  
  }catch(...){
    Serial.println("Failed to read balance ");
  }
  return charBalance;
}

/*********** LNDHUB CHECKER ****************/

bool authDone=false;
unsigned long currentBalance = 0;
void checkLNDHub(void){
    
    Serial.println("[checkLNDHub] Checking LNDHub Balance AuthDone[" + String(authDone) + "]");
    if(!authDone){
      authDone = doAuth();
      if(authDone) currentBalance = checkBalance();
    }
    else{
      unsigned long newBalance = checkBalance();
      if(newBalance != currentBalance) {
        doBoltNotification();
        currentBalance = newBalance;
      }
    }
}

/////////////////////////////////////////////////////////////////
///////////////// Checker generic functions /////////////////////
/////////////////////////////////////////////////////////////////


/******************  SETUP CHECKER *****************/

void setupChecker(void){

    myLNBcard.isLNBitsWallet = myLNBcard.Settings.LNURLWallet.length() > 4; //Means we have a non 0 or a valid address
    
    Serial.println("[CHECKER] setup");
    if(myLNBcard.isLNBitsWallet) setupLNBitsChecker();
    else setupLNDHubChecker();

    setupDone = true;
}

void doBoltNotification(void){
  
  myLNBcard.isStripBlocked = true; //Block other processes update strip leds
  switch(myLNBcard.Settings.BoltEffect){
    case 1: blinkBoltLeds(0); break;
    case 2: rainbowBolt(6); break;
    case 3: snakeBolt(6); break;
    default: blinkBoltLeds(0); break;
  }
  myLNBcard.isStripBlocked = false; //Allow other processes update strip leds
}
void runChecker(void *name){
  
  Serial.println("[CHECKER] started");
  int countChecks = 0;
  
  while (1)
  {
    if ((WiFi.status() == WL_CONNECTED)&&(setupDone)){

        if(myLNBcard.isLNBitsWallet) {
          if(countChecks++>600){ Serial.println(">> Checker LNBITs working - waiting payments "); countChecks=0;} //EveryMinute is print
          //LNBITS stuff
          checkLNURL();
          vTaskDelay(100 / portTICK_PERIOD_MS);
        }       
        else{
          //LNDHUB stuff - every 5 seconds
          Serial.println(">> Checker LNDHUB ");
          checkLNDHub();
          doQueryLed();
          vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }else{
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }
}



