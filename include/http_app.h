#ifndef _HTTP_APP_H_
#define _HTTP_APP_H_

#include <Arduino.h>
#include "wifi_app.h"
#include "BluetoothSerial.h"
#include "config.h"
#include <WString.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <HTTPClient.h>
#include "main.h"


// write interval (in ms)
#define httpWriteInterval  1000

extern char DEVICE_ID[30];

extern TaskHandle_t TaskWeb;
extern const uint8_t PeopleCount_queue_len , WebFlag_queue_len;
extern QueueHandle_t PeopleCount_queue , WebFlag_queue;

void UploadDataToWeb(String url , String IoT_ID , uint8_t counter , String message);
void TaskWebCode( void * pvParameters );


#endif