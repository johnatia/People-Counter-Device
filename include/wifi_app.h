
#ifndef _WIFI_APP_H_
#define _WIFI_APP_H_


#include <Arduino.h>
#include <WString.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <HTTPClient.h>

#include "config.h"

#include "main.h"

extern char WiFiStatus[7][20];
extern int lastlogin;
extern volatile uint8_t WiFi_GetPassword;
extern char * ssid;
extern char temp_ssid[30];
extern char password[50];
extern uint8_t WiFiConnectAttempt;
void WiFi_Init(char* SSID , char* Pass);
void WiFi_Scan(void);
void WiFi_TryToConnect(void);

#endif



