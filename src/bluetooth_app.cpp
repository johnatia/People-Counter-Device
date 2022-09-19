#include "bluetooth_app.h"

BluetoothSerial SerialBT;
SemaphoreHandle_t BleReceiveBinarySemaphore = NULL;
uint8_t Tx_Measurements = 0;
char location_str[2][8] = {
  "outdoor",
  "indoor "
};

int outdoor_indoor[2][2] = {   //sensor is placed outdoor
                              {RIGHT_ZONE_CENTER, LEFT_ZONE_CENTER} , 
                              //sensor is placed indoor
                              {LEFT_ZONE_CENTER, RIGHT_ZONE_CENTER}   
                            };

int BLE_ReadData(void)
{
  int result = 0 , counter = 0;
  char data[30];
  while(SerialBT.available() > 0)
  {
    data[counter++] = SerialBT.read();
  }
  Serial.println(data); // for debugging
  Serial.println(data[0]);
  if(data[0] == '/')  
  { 
    //Serial.println("done");
    result = atoi(data+1);  
  }
  else                
  {
     result = -1;          
  }
  //Serial.println(result);
  return result;
}

void BLE_CallBack(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) 
{
    BaseType_t task_woken = pdFALSE;
//if data is received
    switch (event)
    {
        case ESP_SPP_DATA_IND_EVT:
            //give the semaphore to trigger the task to be ready
            xSemaphoreGiveFromISR(BleReceiveBinarySemaphore , &task_woken);
            //Exit from ISR 
            if(task_woken)
            {
                portYIELD_FROM_ISR();
            }
        break;

        case ESP_SPP_CONG_EVT:
            SerialBT.println("ESP_SPP_CONG_EVT happens");
        break;
    }
}

void BLEReceiveDataCode( void * pvParameters )
{
    static uint8_t ota_status = 0;
    char ota_status_str[2][10] = {"disabled" , "enabled" };
    while(1)
    {
        xSemaphoreTake(BleReceiveBinarySemaphore, portMAX_DELAY);
        if(SerialBT.available()>0)
        {
            if(lastlogin == SETTINGS_LOCATION)
            {
                RxData = BLE_ReadData();
                // Serial.print("Location = ");
                // Serial.println(RxData);
                if(RxData == INDOOR) //indoor
                {
                    center  = outdoor_indoor[1];
                    EEPROM_APP_WRITE_8BIT(EEPROM_LOCATION_ADDRESS, INDOOR );
                    SerialBT.println("Device is indoor ...");
                    SerialBT.println("Configurations are saved to EEPROM! ");
                    lastlogin = 0;
                }
                else if(RxData == OUTDOOR) //outdoor
                {
                    center  = outdoor_indoor[0];
                    EEPROM_APP_WRITE_8BIT(EEPROM_LOCATION_ADDRESS, OUTDOOR );
                    SerialBT.println("Device is outdoor ...");
                    SerialBT.println("Configurations are saved to EEPROM! ");
                    lastlogin = 0;
                }
                else{SerialBT.println("Error!, Please try again ...");}
            }

            else if(lastlogin == SETTINGS_THRESHOLD)
            {
                RxData = BLE_ReadData();
                // Serial.print("Threshold Data = ");
                // Serial.println(RxData);
                if(RxData > 500 && RxData < 4000)      
                { 
                    Threshold = RxData ; 
                    EEPROM_APP_WRITE_16BIT(EEPROM_THRESHOLD_ADDRESS_LSB , Threshold);
                    SerialBT.print("the new value for the Threshold = ");
                    SerialBT.print(Threshold);
                    SerialBT.println("Configurations are saved to EEPROM!");

                    lastlogin = 0;
                }
                else SerialBT.print("Threshold value must be in Range of 500 : 4000 ! ");
            }
            else if(lastlogin == SETTINGS_PEOPLE_COUNT)
            {
                RxData = BLE_ReadData();
                PeopleCount = RxData ;
                EEPROM_APP_WRITE_8BIT(EEPROM_PEOPLECOUNT_ADDRESS, PeopleCount  );
                SerialBT.print("Edited value of People Count = ");
                SerialBT.print(PeopleCount);
                SerialBT.println("Configurations are saved to EEPROM!");
            

                #if(DISPLAY_VERSION == YES)
                vTaskResume(DisplayHandle);
                #endif
                lastlogin = 0;
            }
            else if(lastlogin == SETTINGS_DEVICE_ID)
            {
                int count_ID = 0;
                memset(DEVICE_ID, 0 , sizeof(DEVICE_ID)/sizeof(DEVICE_ID[0]) );
                while(SerialBT.available() > 0)
                {
                    DEVICE_ID[count_ID++] = SerialBT.read();
                    vTaskDelay(1);
                }
                DEVICE_ID[count_ID-2] = '\0';
                status_t status = eeprom_app_write_str(EEPROM_DEVICE_ID_ADDRESS_START , EEPROM_DEVICE_ID_ADDRESS_END , DEVICE_ID);
                if(status == SUCCESS )
                {
                    SerialBT.println("DEVICE_ID is written to EEPROM susccefully! ");
                    eeprom_app_read_device_ID();
                    SerialBT.begin(DEVICE_ID);
                }
                else{SerialBT.println("Error in saving DEVICE_ID to EEPROM! ");}
                lastlogin = 0;
            }
            else if(lastlogin == SETTINGS_WIFI)
            {
                WiFi_TryToConnect();
            }
#if(DISPLAY_VERSION == YES)
            else if(lastlogin == SETTINGS_OVER_DENSITY)
            {
                RxData = BLE_ReadData();
                OverDensity = RxData;
                EEPROM_APP_WRITE_8BIT(EEPROM_OVER_DENSITY_ADDRESS , OverDensity);
                SerialBT.print("Modified value of over density = ");
                SerialBT.println(OverDensity);
                if(PeopleCount >= OverDensity) { vTaskResume(  AlarmHandle );                 }  
                else                          { vTaskSuspend( AlarmHandle );    Alarm_OFF(); }            
                lastlogin = 0;
            }
#endif
            else 
            {
                RxData = BLE_ReadData();
                //Serial.print("Received Data = ");
                //Serial.println(RxData);
                switch (RxData)
                {
                    case SETTINGS_DEVICE_ID:
                        SelectMode = CONFIG_MODE;
                        SerialBT.println("Please insert Device ID");  
                        lastlogin = SETTINGS_DEVICE_ID;
                        break;
                    case SETTINGS_PEOPLE_COUNT:
                        SelectMode = CONFIG_MODE;
                        SerialBT.println("Please insert People Count Value ");  
                        lastlogin = SETTINGS_PEOPLE_COUNT;
                        break;
                    case SETTINGS_THRESHOLD:
                        SelectMode = CONFIG_MODE;
                        SerialBT.println("Please insert Threshold Value");  
                        lastlogin = SETTINGS_THRESHOLD;
                        break;

                    case SETTINGS_LOCATION:
                        SelectMode = CONFIG_MODE;
                        SerialBT.print("Location = ");
                        SerialBT.println(   location_str[ (EEPROM_APP_READ_8BIT(EEPROM_LOCATION_ADDRESS) - 1) ]  );
                        SerialBT.println("Please Determine if the device will be indoor or outdoor: ");
                        SerialBT.println("1- Outdoor");
                        SerialBT.println("2- Indoor");  

                        lastlogin = SETTINGS_LOCATION;
                        break;

                    case SETTINGS_WIFI:
                        SelectMode = CONFIG_MODE;
                        WiFi_Scan_Flag = 1;
                        lastlogin = SETTINGS_WIFI;
                        break;
#if(DISPLAY_VERSION == YES)
                    case SETTINGS_OVER_DENSITY:
                        SelectMode = CONFIG_MODE;
                        SerialBT.println("Please insert the value of over density");
                        lastlogin = SETTINGS_OVER_DENSITY;
                        break;                        
#endif
                    case DISABLE_BLE:
                        SelectMode = CONFIG_MODE;
                        BLE_ENABLE_FLAG = 0;
                        break;

                    case GET_DEVICE_DATA:
                        SelectMode = CONFIG_MODE;
                        SerialBT.print("Device Version =  ");
                        SerialBT.println(DEVICE_VERSION);
                        eeprom_app_read_device_ID();
                        SerialBT.print("People Count = ");
                        SerialBT.println(PeopleCount);
#if (DISPLAY_VERSION == YES)
                        SerialBT.print("Max No. allowed in the room = ");
                        SerialBT.println(OverDensity);
#endif
                        SerialBT.print("Distance = ");
                        SerialBT.println(Distance);  
                        SerialBT.print("Threshold = ");
                        SerialBT.println(Threshold);  
                        SerialBT.print("Location = ");
                        SerialBT.println(   location_str[ (EEPROM_APP_READ_8BIT(EEPROM_LOCATION_ADDRESS) - 1) ]  );              
                        eeprom_app_read_wifi();
                        distanceSensorGetPara();
                        SerialBT.print("ota status: ");
                        SerialBT.println(ota_status_str[ota_status]);
                        lastlogin = 0;
                        break;

                    case DEVICE_ON:
                        SelectMode = ACTIVE_MODE;
                        Tx_Measurements ^= 0x01;
                        if(Tx_Measurements)   
                        SerialBT.println("Start Transmitting Measurements ");
                        else SerialBT.println("Stop Transmitting Measurements ..");
                        lastlogin = 0;
                        break;
                    case UPLOAD_OTA:
                        if(ota_status != 1)
                        {
                            ota_status = 1;
                            SerialBT.println("activate feature: updating firmware over the air...");
                            ota_xTaskCreate();
                        }
                        else SerialBT.println("OTA feature is already active");
                        break;
                }
            }
        }
   //     vTaskDelay(10);
    }
}
