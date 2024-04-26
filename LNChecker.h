#ifndef CHECKER_API_H
#define CHECKER_API_H

#include <Arduino.h>
#ifndef _STORAGE_H_
  #include "src/storage/nvMemory.h"
#endif

// Access point variables
//Lnbits test usr: 7ecf5cf20e9645318854cb9dd76b3e8b 
#define DEFAULT_URL_PREFIX      "ws://"
#define DEFAULT_API_URL         "/api/v1/ws/"
#define DEFAULT_LNBITS_SERVER   "legend.lnbits.com"                      //Url (legend.lnbits.com) or your onion address
#define DEFAULT_WALLET          "8d918866a20948439874f951638b6187" //"YiqCNFJ4tWF9qjpqzqpK7t"

//#define LNHUBurl  "getalby.com/lndhub/"
//#define LNHUBServer   "getalby.com" //"ln.getalby.com"  

//Time update period
#define UPDATE_PERIOD_h   5

enum LNBState {
  LNB_waitingConfig,
  LNB_Connecting,
  LNB_Connected
};

typedef struct{
  LNBState LNBStatus;
  bool forceConfig;
  TSettings Settings;
  bool isStripBlocked; //Flag to block strip update from other process
  bool isLNBitsWallet; // 0-Means LNBits / 1-GetAlby
  bool isWSconnected;  // Indicates if socket is connected
}LNBCard;


void setupChecker(void);
void runChecker(void *name);
void doBoltNotification(void);

#endif //CHECKER_API_H
