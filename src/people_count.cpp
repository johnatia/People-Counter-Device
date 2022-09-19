#include "people_count.h"

int flag = 0;
uint16_t RightDistance = 0 , LeftDistance = 0;
extern int SignalPerSpad , SignalRate ,  AmbientPerSpad  , AmbientRate ;
extern uint8_t Tx_Measurements;

uint16_t PeopleCountAlgorithm(uint16_t distance, uint8_t zone, uint16_t DistanceThreshold)
{
  static int PathTrack[4] = {0,0,0,0};
  static int PathTrackFillingSize = 1; // init this to 1 as we start from state where nobody is any of the zones
  static int LeftPreviousStatus = NOBODY;
  static int RightPreviousStatus = NOBODY;
  //static int PeopleCount = 0;
  static uint16_t Distances[2][DISTANCES_ARRAY_SIZE];
  static uint8_t DistancesTableSize[2] = {0,0};
  uint16_t MinDistance;
  uint8_t i;
  int CurrentZoneStatus = NOBODY;
  int AllZonesCurrentStatus = 0;
  int AnEventHasOccured = 0;
  static uint8_t UploadDataToWebFlag = 0;

  char zone_str[2][6] =
  {
    "Front",
    "Back"
  };
  // read 10 distances for every zone 
  // Serial.print("Distance(mm): ");
  // Serial.println(distance);
  // if(DistancesTableSize[zone] < DISTANCES_ARRAY_SIZE)
  // {
  //   Distances[zone][DistancesTableSize[zone]] = distance;
  //   DistancesTableSize[zone]++;
  // }
  // else{
  //    // SerialBT.println("d5le f el else");
  //     //SerialBT.print("Distances[zone");
  //   //SerialBT.print(zone);
  //   //SerialBT.print("]= {");
  //   // for(uint8_t x = 0; x < DISTANCES_ARRAY_SIZE; x++)
  //   // {
  //   //   SerialBT.print(Distances[zone][x]);
  //   //   SerialBT.print(", ");
  //   // }
  //   // Serial.println("}");

  //   DistancesTableSize[zone] = 0;
  //  // pickup the minimum distance
  //   MinDistance = Distances[zone][0];

  //   for(uint8_t i = 0; i < DISTANCES_ARRAY_SIZE; i++)
  //   {
  //     if(MinDistance > Distances[zone][i])
  //     {
  //       MinDistance = Distances[zone][i];
  //     }
  //   }
  if(Tx_Measurements == 1)
  {
      switch(zone)
      {
        case 0:
          RightDistance =  distance;
        break;
        case 1:
          LeftDistance =  distance;
          flag = 1;
        break;
      }
      if(flag == 1)
      {      
          SerialBT.print("/*");        // Frame start sequence  [/*]
          SerialBT.print(RightDistance); 
          SerialBT.print(",");         // Separator character   [,]
          SerialBT.print(LeftDistance);   
          SerialBT.print(",");
          SerialBT.print(DistanceThreshold); 
          SerialBT.print(",");
          SerialBT.print(PeopleCount);
          SerialBT.print(",");
          SerialBT.print(SignalRate);
          SerialBT.print(",");
          SerialBT.print(AmbientRate);
          SerialBT.print(",");
          // SerialBT.print(SignalPerSpad);
          // SerialBT.print(",");
          // SerialBT.print(AmbientPerSpad);
          SerialBT.print("*/");        // Frame finish sequence [*/]
          SerialBT.println("");


          flag = 0;
      }
  }
    // SerialBT.print("MinDistance[zone][");
    // SerialBT.print(zone);
    // SerialBT.print("] = ");
    // SerialBT.println(MinDistance);
  
    if( (distance < DistanceThreshold) && (distance > 500 ) )
    { 
      CurrentZoneStatus = SOMEONE;
        // Serial.print("SOMEONE \t distance: ");
        // Serial.println(distance);
        SerialBT.print("SOMEONE at ");
        SerialBT.print(zone_str[zone]);
        SerialBT.print(" zone: ");
        SerialBT.println(distance);
    }
      // left zone
  if (zone == LEFT) {
    if (CurrentZoneStatus != LeftPreviousStatus) {
      // event in left zone has occured
      AnEventHasOccured = 1;
      
      if (CurrentZoneStatus == SOMEONE) {
        AllZonesCurrentStatus += 1;
      }
      // need to check right zone as well ...
      if (RightPreviousStatus == SOMEONE) {
        // event in left zone has occured
        AllZonesCurrentStatus += 2;
      }
      // remember for next time
      LeftPreviousStatus = CurrentZoneStatus;
    }
  }
    // right zone
  else {
    
    if (CurrentZoneStatus != RightPreviousStatus) {
      
      // event in left zone has occured
      AnEventHasOccured = 1;
      if (CurrentZoneStatus == SOMEONE) {
        AllZonesCurrentStatus += 2;
      }
      // need to left right zone as well ...
      if (LeftPreviousStatus == SOMEONE) {
        // event in left zone has occured
        AllZonesCurrentStatus += 1;
      }
      // remember for next time
      RightPreviousStatus = CurrentZoneStatus;
    }
  }

  


    // if an event has occured
  if (AnEventHasOccured) {
    if (PathTrackFillingSize < 4) {
      PathTrackFillingSize ++;
    }
    
    // if nobody anywhere lets check if an exit or entry has happened
    if ((LeftPreviousStatus == NOBODY) && (RightPreviousStatus == NOBODY)) {
      
      // check exit or entry only if PathTrackFillingSize is 4 (for example 0 1 3 2) and last event is 0 (nobobdy anywhere)
      if (PathTrackFillingSize == 4) {
        //print PathTrack
        SerialBT.print("PathTrack: ");
        for(uint8_t count = 0; count < 4; count++)
        {
            SerialBT.print(PathTrack[count]);
        }
        SerialBT.println("");

        // check exit or entry. no need to check PathTrack[0] == 0 , it is always the case
        
        if ((PathTrack[1] == 1)  && (PathTrack[2] == 3) && (PathTrack[3] == 2)) {
          // This an exit
          PeopleCount--;
          if(PeopleCount < 0 )
          {
            PeopleCount = 0 ;
          }
          // reset the table filling size in case an entry or exit just found
          DistancesTableSize[0] = 0;
          DistancesTableSize[1] = 0;
          // for(int n = 0; n < DISTANCES_ARRAY_SIZE; n++)
          // {
          //   Distances[0][n] = 4000;
          //   Distances[1][n] = 4000;
          // }
		    // Serial.print("Walk Out, People Count:");
		    // Serial.println(PeopleCount);
          #if (DISPLAY_VERSION == YES)
          vTaskResume(DisplayHandle);   //Display_Show(PeopleCount);
          #endif
        SerialBT.print("Walk Out, People Count: ");
		    SerialBT.println(PeopleCount);

        EEPROM.write(EEPROM_PEOPLECOUNT_ADDRESS , (uint8_t)PeopleCount);
        EEPROM.commit();
        if(WiFi.status()== WL_CONNECTED) 
        {
          //Upload Data to Web
          // Task2code(NULL);
          if(xQueueSend(PeopleCount_queue , (void *)&PeopleCount , 10) != pdTRUE )
          {
            SerialBT.println("People Count Queue is full, Reset the Queue !");
            xQueueReset(PeopleCount_queue);
          }
          //trigger the flag to upload data to web 
          UploadDataToWebFlag = 1;
          //send the flag to queue of flags to activate the operation untill all data is sent completely
          if(xQueueSend(WebFlag_queue , (void *)&UploadDataToWebFlag , 10) != pdTRUE )
          {
            SerialBT.println("Web Flag Queue is full, Reset the Queue !");
            xQueueReset(WebFlag_queue);
          }

        }
        } else if ((PathTrack[1] == 2)  && (PathTrack[2] == 3) && (PathTrack[3] == 1)) {
          // This an entry
          PeopleCount++;
          // reset the table filling size in case an entry or exit just found
          DistancesTableSize[0] = 0;
          DistancesTableSize[1] = 0;
          // for(int n = 0; n < DISTANCES_ARRAY_SIZE; n++)
          // {
          //   Distances[0][n] = 4000;
          //   Distances[1][n] = 4000;
          // }
          // Serial.print("Walk In, People Count:");
		      // Serial.println(PeopleCount);
          SerialBT.print("Walk In, People Count: ");
		      SerialBT.println(PeopleCount);
          EEPROM.write(EEPROM_PEOPLECOUNT_ADDRESS , (uint8_t)PeopleCount);
          EEPROM.commit();
          #if (DISPLAY_VERSION == YES)
          vTaskResume(DisplayHandle);   //Display_Show(PeopleCount);
          #endif
          
          if(WiFi.status()== WL_CONNECTED)
          {
            //Upload Data to Web
          if(xQueueSend(PeopleCount_queue , (void *)&PeopleCount , 10) != pdTRUE )
          {
            SerialBT.println("People Count Queue is full, Reset the Queue !");
            xQueueReset(PeopleCount_queue);
          }
          //trigger the flag to upload data to web 
          UploadDataToWebFlag = 1;
          //send the flag to queue of flags to activate the operation untill all data is sent completely
          if(xQueueSend(WebFlag_queue , (void *)&UploadDataToWebFlag , 10) != pdTRUE )
          {
            SerialBT.println("Web Flag Queue is full, Reset the Queue !");
            xQueueReset(WebFlag_queue);
          }
          //Task2code(NULL);
          }
        } else {
          // reset the table filling size also in case of unexpected path
          DistancesTableSize[0] = 0;
          DistancesTableSize[1] = 0;
          // for(int n = 0; n < DISTANCES_ARRAY_SIZE; n++)
          // {
          //   Distances[0][n] = 4000;
          //   Distances[1][n] = 4000;
          // }
          // Serial.println("Wrong path\n");
          SerialBT.println("Wrong path\n");
        }
      }
      
      PathTrackFillingSize = 1;
      PathTrack[0] = 0;
      PathTrack[1] = 0;
      PathTrack[2] = 0;
      PathTrack[3] = 0;

    }
    else {
      // update PathTrack
      // example of PathTrack update
      // 0
      // 0 1
      // 0 1 3
      // 0 1 3 1
      // 0 1 3 3
      // 0 1 3 2 ==> if next is 0 : check if exit
      PathTrack[PathTrackFillingSize-1] = AllZonesCurrentStatus;
    }
    
  }
//}

  

//}
return PeopleCount;

}

 


