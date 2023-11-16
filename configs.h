/************************************************/

// Wifi
#define WIFI_SSID     "YourWifi"     //"YourWifi"
#define WIFI_PASSWORD "Pass"        //"Pass"

// Access point variables
#define password     "ToTheMoon"                              //Access point password
#define lnbitsServer "legend.lnbits.com"                      //Url (legend.lnbits.com) or your onion address
#define LNURLP       "LNURL1DP68GURN8GHJ7MR9VAJKUEPWD3HXY6T5WVHXXMMD9AKXUATJD3CZ7CTSDYHHVVF0D3H82UNV9UENSWPH5Q207Y"
                                                              //Copy and paste your LNURLpaylink                    
#define invoiceKey   "yourKey"       //Invoice/read key:
#define amount       500                                      //Sats amount expected to receive

//Lamp Spark pin
#define switchPin    4                                        //ESP32 pin to set high when payment is done
//Pin conectado al NeoPixel
#define LED_PIN           10      // Pin - leds neopixel
#define LED_COUNT         25       // Neopixel number of leds
#define COLOR_RED         0xFF0000
#define COLOR_GREEN       0x00FF00
#define COLOR_BLUE        0x0000FF
#define COLOR_YELLOW      0xFFFF00
#define COLOR_NONE        0x000000

// setting PWM properties
#define freq         5000
#define ledChannel   0
#define resolution   8
#define initDuty     50
#define MAX          180
#define MIN          0

//Touch properties
#define touchPin    32
#define threshold   50
/******************************************************/




