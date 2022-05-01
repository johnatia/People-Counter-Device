#ifndef _BLUETOOTH_APP_H_
#define _BLUETOOTH_APP_H_

#include <Arduino.h>
#include "BluetoothSerial.h"

#include <Wire.h>
#include "wifi_app.h"
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X
#include "people_count.h"
#include "http_app.h"
#include "alarm.h"
#include "display.h"
#include "config.h"
#include "device_mode.h"

#include  "wifi_app.h"
#include  "eeprom_app.h"


#define CONFIG_MODE 5

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


extern BluetoothSerial SerialBT;
extern volatile uint8_t BLE_ENABLE_FLAG;
extern SFEVL53L1X distanceSensor;
extern uint16_t Distance;
extern int SignalPerSpad;
extern int *center;
extern int Zone;
extern uint16_t Threshold;
extern volatile uint8_t WiFi_Scan_Flag;
extern uint8_t SelectMode;

extern int RxData;
extern int lastlogin;
extern uint8_t SelectMode;
extern int outdoor_indoor[2][2];
extern char location_str[2][8];
extern int OverDensity;
extern SemaphoreHandle_t BleReceiveBinarySemaphore;

extern EventGroupHandle_t  xEventGroup;
extern TaskHandle_t DisplayHandle;
extern uint8_t Tx_Measurements ;

int  BLE_ReadData(void);
void BLE_CallBack(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
void BLEReceiveDataCode( void * pvParameters );

#endif