#include "device_mode.h"

int SignalPerSpad = 0 , SignalRate = 0 , AmbientPerSpad = 0 , AmbientRate = 0;

void distanceSensorGetPara(void)
{
  char distanceMode_str[2][6] = {"Short" , "Long"};
  uint8_t distanceMode = 0;
 //Get the distance mode, returns 1 for short and 2 for long
 distanceMode = distanceSensor.getDistanceMode();
 if(distanceMode  == 1 || distanceMode == 2)
 {
  SerialBT.print("DistnaceMode: ");
  SerialBT.println(distanceMode_str[ (distanceSensor.getDistanceMode() - 1) ]);
 }
  //Get the timing budget for a measurement
  SerialBT.print("TimingBudgetInMs: ");
  SerialBT.print(distanceSensor.getTimingBudgetInMs());
  SerialBT.println("ms");
  //Get time between measurements in ms
  SerialBT.print("IntermeasurementPeriod: ");
  SerialBT.print(distanceSensor.getIntermeasurementPeriod());
  SerialBT.println("ms");
  SerialBT.print("getROIX: ");
  SerialBT.println(distanceSensor.getROIX());
  SerialBT.print("getROIY: ");
  SerialBT.println(distanceSensor.getROIY());
  SerialBT.print("Offest: ");
  SerialBT.println(distanceSensor.getOffset());

  SerialBT.print("SignalPerSpad: ");
  SerialBT.println(distanceSensor.getSignalPerSpad());  //Returns the average signal rate per SPAD (The sensitive pads that detect light, the VL53L1X has a 16x16 array of these) in kcps/SPAD, or kilo counts per second per SPAD.
  
  SerialBT.print("AmbientPerSpad: ");
  SerialBT.println(distanceSensor.getAmbientPerSpad()); //Returns the ambient noise when not measuring a signal in kcps/SPAD.

  SerialBT.print("SignalRate: ");
  SerialBT.println(distanceSensor.getSignalRate());     //Returns the signal rate in kcps. All SPADs combined.

  SerialBT.print("SpadNb: ");
  SerialBT.println(distanceSensor.getSpadNb());         //Returns the current number of enabled SPADs

  SerialBT.print("AmbientRate: ");
  SerialBT.println(distanceSensor.getAmbientRate());  	// Returns the total ambinet rate in kcps. All SPADs combined.

  SerialBT.print("XTalk: ");
  SerialBT.println(distanceSensor.getXTalk());          //Returns the current crosstalk value in cps.

  SerialBT.print("RangeStatus: ");
  SerialBT.println(distanceSensor.getRangeStatus());  //Returns the range status, which can be any of the following. 0 = no error, 1 = signal fail, 2 = sigma fail, 7 = wrapped target fail

  // Serial.print("Interrupt Polarity = ");
  // Serial.println(distanceSensor.getInterruptPolarity());
  // printf("Distance Threshold: \n");
  // printf("lowThresh = %d, hiThresh = %d, window = %d \n",	 
  // distanceSensor.getDistanceThresholdLow(),
	// distanceSensor.getDistanceThresholdHigh(),
  // distanceSensor.getDistanceThresholdWindow()
  // );

}

void Device_ConfigMode(void)
{
      // Serial.print("main() running on core ");
      // Serial.println(xPortGetCoreID());
  if(BLE_ENABLE_FLAG == 1) //Enable BLE
  {
    digitalWrite(BuiltInLED , HIGH);
    Serial.println("Initialize BLE");
    SerialBT.begin(DEVICE_ID);
    BLE_ENABLE_FLAG |= 1<<1;
  }
  else if(BLE_ENABLE_FLAG == 0) //Disable BLE & Turn ON Active Mode
  {
    Serial.println("BLE is going to say bye bye");
    SerialBT.println("BLE is going to say bye bye");
    vTaskDelay(100);
    SerialBT.end();
    BLE_ENABLE_FLAG |= 1<<1;
    SelectMode = ACTIVE_MODE;
    for (uint16_t i = 0; i < 400; i+=10)
    {
      digitalWrite(BuiltInLED , !digitalRead(BuiltInLED));
      vTaskDelay(400-i);
    }
    digitalWrite(BuiltInLED , LOW);
  }  

  if(WiFi_Scan_Flag == 1)
  {
    //wifi scan
    SerialBT.println("Wifi scan ");
    WiFi_Scan();
    //delay(1000);

    WiFi_Scan_Flag = 0;
  }
  if(WiFi_GetPassword == 1)
  {
  //  SerialBT.println("WiFi_GetPassword mbrook");
    SerialBT.print("ssid = ");
    SerialBT.println(ssid);
    SerialBT.print("password = ");
    SerialBT.println(password);
    WiFi_Init(ssid , password);
    WiFi_GetPassword = 0;
  }
}


void Device_ActiveMode(void)
{
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady())
  {
    vTaskDelay(1);
  }
  Distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  SignalRate = distanceSensor.getSignalRate();
  AmbientRate = distanceSensor.getAmbientRate();
  SignalPerSpad = distanceSensor.getSignalPerSpad();
  AmbientPerSpad = distanceSensor.getAmbientPerSpad();
  // Serial.print("Distance = ");
  // Serial.println(Distance);
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();
  PeopleCount = PeopleCountAlgorithm(Distance, Zone , Threshold);
  #if(DISPLAY_VERSION == YES)
  if(PeopleCount >= OverDensity)     vTaskResume(  AlarmHandle );
  else
  {
    vTaskSuspend( AlarmHandle );
    Alarm_OFF();
  }                              
  #endif
  Zone++;
  Zone = Zone%2;
  distanceSensor.setROI(ROWS_OF_SPADS, 16, center[Zone]);
  //vTaskDelay(1);
}