#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <Arduino.h>

// config files

// default settings
#define DEFAULT_SSID			"LNBCardAP"
#define DEFAULT_WIFIPW			"BoltPower"
#define DEFAULT_API_URL         "/api/v1/ws/"
#define DEFAULT_SERVER			"legend.lnbits.com"
#define DEFAULT_WALLET			"8d918866a20948439874f951638b6187" //"ws://legend.lnbits.com/api/v1/ws/YiqCNFJ4tWF9qjpqzqpK7t"
#define DEFAULT_URILNDHUB		"lndhub://XXXX:YYY@https://ln.getalby.com/"
#define DEFAULT_BOLTCOLOR		1
#define DEFAULT_BOLTEFFECT		1

// JSON config files
#define JSON_CONFIG_FILE		"/config.json"

// JSON config file SPIFFS 
#define JSON_KEY_SSID			"SSID"
#define JSON_KEY_PASW			"WifiPW"
#define JSON_KEY_LNURLServer	"LNURLServer"
#define JSON_KEY_LNURLWallet	"LNURLWallet"
#define JSON_KEY_LNDHUBUri		"LNDHUBuri"
#define JSON_KEY_BOLTCOLOR		"BoltColor"
#define JSON_KEY_BOLTEFFECT		"BoltEffect"

// settings
struct TSettings
{
	String WifiSSID{ DEFAULT_SSID };
	String WifiPW{ DEFAULT_WIFIPW };
	String LNURLServer { DEFAULT_SERVER };
	String LNURLWallet{ DEFAULT_WALLET };
	String LNDHUBuri{ DEFAULT_URILNDHUB };
	int BoltColor{ DEFAULT_BOLTCOLOR };
	int BoltEffect{ DEFAULT_BOLTEFFECT };
};


#endif // _STORAGE_H_