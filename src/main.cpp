

#include "main.h"



uint16_t Distance;
int Zone = 0;
int *center  = outdoor_indoor[0];

int RxData = 0;
uint16_t Threshold = 1000;
int lastlogin = 0;
uint8_t SelectMode = ACTIVE_MODE;
uint8_t LastMode = CONFIG_MODE;
#define DEBOUNCE_TIME 250
volatile uint32_t DebounceTimer = 0;
#define EXTI_PIN 35
volatile uint8_t BLE_ENABLE_FLAG = 5;
volatile uint8_t WiFi_Scan_Flag = 0;
volatile uint8_t WiFi_GetPassword = 0;
char temp_ssid[30] = "your SSID";
char * ssid =  temp_ssid;
char password[50] = "Your Password" ;
SFEVL53L1X distanceSensor;
int8_t PeopleCount = 0;
const uint8_t PeopleCount_queue_len = 100 , WebFlag_queue_len = 100;
QueueHandle_t PeopleCount_queue , WebFlag_queue;
TaskHandle_t TaskmainHandle , TaskWebHandle , WiFiReconnectHandle, BLEReceiveDataHandle ;
#if(DISPLAY_VERSION == YES)
TaskHandle_t DisplayHandle , AlarmHandle;
int OverDensity = 99;
#endif
void IRAM_ATTR isr() {
     if ( millis() - DEBOUNCE_TIME  >= DebounceTimer ) 
    {
      DebounceTimer = millis();
      BLE_ENABLE_FLAG = (BLE_ENABLE_FLAG & 0x01);
      BLE_ENABLE_FLAG ^= 0x01;
      Serial.print("BLE_ENABLE_FLAG = ");
      Serial.println(BLE_ENABLE_FLAG); 
      SelectMode = CONFIG_MODE;   
    }   
}






void WiFiReconnectCode( void * pvParameters )
{
  while(1)
  {
    if( (WiFi.status() != WL_CONNECTED) && (SelectMode != CONFIG_MODE) )
    {
      SerialBT.println("Reconnecting To WiFi");
      WiFi.disconnect();
      vTaskDelay(500);
      WiFi.reconnect();
    }
      vTaskDelay(9000);
  }
}
void TaskmainCode( void * pvParameters )
{
  while(1)
  {
    esp_task_wdt_reset();
    switch(SelectMode)
    {
      case CONFIG_MODE:
        if(LastMode != CONFIG_MODE)
        { SerialBT.println("Config Mode is working now!");  }
        Device_ConfigMode();
        LastMode = CONFIG_MODE;
      break;
      case ACTIVE_MODE:
        if(LastMode != ACTIVE_MODE)
        {SerialBT.println("Active Mode is working now!");}
        Device_ActiveMode();
        LastMode = ACTIVE_MODE;
      break;
    }
  vTaskDelay(1);
}

}
#if(DISPLAY_VERSION == YES)
void DisplayCode( void * pvParameters )
{
  while(1)
  {
    Display_Show(PeopleCount);
    vTaskSuspend( DisplayHandle );
    vTaskDelay(100);
  }
}
void AlarmCode( void * pvParameters )
{
  while(1)
  {
    Alarm_ON();
  }

}
#endif

void setup(void)
{
  Alarm_Init();
  Alarm_OFF();
  #if(DISPLAY_VERSION == YES)
  Display_Init();
  #endif
  pinMode(BuiltInLED , OUTPUT);
  pinMode(EXTI_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(EXTI_PIN), isr, FALLING);
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts

  Serial.begin(115200);
  Wire.begin();
  EEPROM.begin(EEPROM_SIZE);      // initialize EEPROM with predefined size
  if(EEPROM_APP_READ_8BIT(EEPROM_DEVICE_IS_1ST_TIME_TO_BOOT) == 'N')
  {
    Serial.println("Read EEPROM Data...");
    Threshold = EEPROM_APP_READ_16BIT(EEPROM_THRESHOLD_ADDRESS_LSB );
    PeopleCount = EEPROM_APP_READ_8BIT(EEPROM_PEOPLECOUNT_ADDRESS);
    center = outdoor_indoor[ EEPROM_APP_READ_8BIT(EEPROM_LOCATION_ADDRESS) -1 ];
#if (DISPLAY_VERSION == YES)
    OverDensity = EEPROM_APP_READ_8BIT(EEPROM_OVER_DENSITY_ADDRESS);
#endif
    eeprom_app_read_wifi();
    eeprom_app_read_device_ID();
  }
  else 
  {
    Serial.println("People Counter: Hello Wolrd !\nDevice is booting for the 1st time ...");
    if( eeprom_app_1st_time_to_boot() != ERROR ) 
    {Serial.println ("saving data to eeprom after 1st time ");}
    else{ Serial.println("error while saving data in eeprom after 1st boot");  }
  }
  #if(DISPLAY_VERSION == YES)
  Display_Show(PeopleCount);
  #endif
  SerialBT.begin(DEVICE_ID);
  SerialBT.register_callback(BLE_CallBack);
  digitalWrite(LED_BUILTIN , HIGH);
  WiFi.mode(WIFI_STA);
  if(distanceSensor.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor failed to begin. Try again...");
    ESP.restart();  
  }
  distanceSensor.setDistanceModeLong();
  distanceSensor.setTimingBudgetInMs(33);  //Set Timing Budget
  //Set InterMeasurement
  distanceSensor.setROI(ROWS_OF_SPADS, 16 , center[Zone]); //Set Region of Interest
  //distanceSensorGetPara();
 // WiFi_Init(ssid , password);


  WebFlag_queue             = xQueueCreate(WebFlag_queue_len      , sizeof(int) );
  PeopleCount_queue         = xQueueCreate(PeopleCount_queue_len  , sizeof(int) );
  BleReceiveBinarySemaphore = xSemaphoreCreateBinary();
  if(BleReceiveBinarySemaphore == NULL)
  {
    Serial.println("Error while creating BleReceiveBinarySemaphore");
//  ESP.restart();
  }

xTaskCreatePinnedToCore(
                    TaskWebCode,   /* Task function. */
                    "Task Web",     /* name of task. */
                    5000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    3,           /* priority of the task */
                    &TaskWebHandle,     /* Task handle to keep track of created task */
                    1);
xTaskCreatePinnedToCore(
                    TaskmainCode,   /* Task function. */
                    "Task main",     /* name of task. */
                    8000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    5,           /* priority of the task */
                    &TaskmainHandle,      /* Task handle to keep track of created task */
                    1);

esp_task_wdt_add(TaskmainHandle); //add current thread to WDT watch

xTaskCreatePinnedToCore(
                    WiFiReconnectCode,   /* Task function. */
                    "Task WiFi Reconnect",     /* name of task. */
                    2000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    2,           /* priority of the task */
                    &WiFiReconnectHandle,     /* Task handle to keep track of created task */
                    1);

xTaskCreatePinnedToCore(
                    BLEReceiveDataCode,   /* Task function. */
                    "ble receive data from isr",     /* name of task. */
                    5000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                     10 ,           /* priority of the task */
                    &BLEReceiveDataHandle,     /* Task handle to keep track of created task */
                    1);

#if(DISPLAY_VERSION == YES)
xTaskCreatePinnedToCore(
                    DisplayCode,   /* Task function. */
                    "Task display the peoplce count",     /* name of task. */
                    2000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    4,           /* priority of the task */
                    &DisplayHandle,     /* Task handle to keep track of created task */
                    1);
xTaskCreatePinnedToCore(
                    AlarmCode,   /* Task function. */
                    "Task Alarm",     /* name of task. */
                    2000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    2,           /* priority of the task */
                    &AlarmHandle,     /* Task handle to keep track of created task */
                    1);
#endif
 //Delete setup
  vTaskDelete(NULL);
  
}

void loop(void) 
{
  //delet loop function
  vTaskDelete(NULL);
}
