#ifndef _DEVICE_MODE_
#define _DEVICE_MODE_

#include <Arduino.h>

#include <Wire.h>
#include "BluetoothSerial.h"
#include "wifi_app.h"
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

#include "people_count.h"
#include "http_app.h"
#include "alarm.h"
#include "display.h"

#include "config.h"


#define CONFIG_MODE 5
#define ACTIVE_MODE 10

//set 5 seconds WDT
#define WDT_TIMEOUT             5
// #define WIFI_MODE          0x06
#define SETTINGS_PEOPLE_COUNT   0x04
#define SETTINGS_OVER_DENSITY   0x05
#define SETTINGS_WIFI           0x06
#define SETTINGS_THRESHOLD      0x09
#define SETTINGS_LOCATION       0x08
#define DEVICE_ON               10
#define GET_DEVICE_DATA         11
#define UPLOAD_OTA              12
#define SETTINGS_DEVICE_ID      0x07
#define DISABLE_BLE             0x100



extern volatile uint8_t BLE_ENABLE_FLAG;
extern SFEVL53L1X distanceSensor;
extern uint16_t Distance;
extern int SignalPerSpad ,  SignalRate  , AmbientPerSpad  , AmbientRate ;
extern int *center;
extern int Zone;
extern uint16_t Threshold;
extern volatile uint8_t WiFi_Scan_Flag;
extern uint8_t SelectMode;
#if(DISPLAY_VERSION == YES)
extern TaskHandle_t AlarmHandle;
extern int OverDensity;
#endif

void distanceSensorGetPara(void);
void Device_ConfigMode(void);
void Device_ActiveMode(void);


#endif