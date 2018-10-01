//*************************************************************************************************
//*************** Bibliotecas utilizadas **********************************************************
//*************************************************************************************************
#ifdef USE_TFT // Do not change this line
#include <UTFT.h>
#include <URTouch.h>
#endif // Do not change this line
#include <Wire.h>
#include <EEPROM.h>
#include <writeAnything.h>
#include <DS1307_HENNING.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <avr/pgmspace.h>

#ifdef USE_PCF8575 // Do not change this line!
#include <PCF8575.h>
#endif // Do not change this line!

#if defined(RFM12B_LED_CONTROL) || defined(RFM12B_RELAY_CONTROL) // Do not change this line!
#include <RFM12B.h>
#endif // Do not change this line!

#ifdef ETHERNET_SHIELD // Do not change this line!
#ifndef USE_ESP8266 // Do not change this line!
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#else // Do not change this line!
#include <ELClient.h>
#include <ELClientMqtt.h>
#include <NeoSWSerial.h>
#endif // Do not change this line!
#include <ArduinoJson.h>
#endif // Do not change this line!

#ifdef WATCHDOG // Do not change this line!
#include <avr/wdt.h>
#endif // Do not change this line!
