#ifndef _OTA_APP_H_
#define _OTA_APP_H_

#include "main.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


void ota_setup(void);
void ota_uploadFirmwareTask(void *pvParameters);
void ota_xTaskCreate(void);

#endif