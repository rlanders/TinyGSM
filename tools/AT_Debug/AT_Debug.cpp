/**************************************************************
 *
 * This script tries to auto-detect the baud rate
 * and allows direct AT commands access
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 **************************************************************/

// Select your modem:
#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM868
// #define TINY_GSM_MODEM_UBLOX
// #define TINY_GSM_MODEM_M95
// #define TINY_GSM_MODEM_BG96
// #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_MC60
// #define TINY_GSM_MODEM_MC60E
// #define TINY_GSM_MODEM_ESP8266
// #define TINY_GSM_MODEM_XBEE

// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// or Software Serial on Uno, Nano
//#include <SoftwareSerial.h>
//SoftwareSerial SerialAT(2, 3); // RX, TX

#define TINY_GSM_DEBUG SerialMon

#include <TinyGsmClient.h>

// Module baud rate
uint32_t rate = 0; // Set to 0 for Auto-Detect

void setup() {
  // Set console baud rate
  SerialMon.baud(115200);
  delay(6000);
}

void loop() {

  if (!rate) {
    rate = TinyGsmAutoBaud(SerialAT);
  }

  if (!rate) {
    SerialMon.printf("***********************************************************"));
    SerialMon.printf(" Module does not respond!"));
    SerialMon.printf("   Check your Serial wiring"));
    SerialMon.printf("   Check the module is correctly powered and turned on"));
    SerialMon.printf("***********************************************************"));
    delay(30000L);
    return;
  }

  SerialAT.baud(rate);

  // Access AT commands from Serial Monitor
  SerialMon.printf("***********************************************************"));
  SerialMon.printf(" You can now send AT commands"));
  SerialMon.printf(" Enter \"AT\" (without quotes), and you should see \"OK\""));
  SerialMon.printf(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
  SerialMon.printf("***********************************************************"));

  while(true) {
    if (SerialAT.available()) {
      SerialMon.write(SerialAT.read());
    }
    if (SerialMon.available()) {
      SerialAT.write(SerialMon.read());
    }
    delay(0);
  }
}
