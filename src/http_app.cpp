#include "http_app.h"


void UploadDataToWeb(String url , String IoT_ID , uint8_t counter , String message)
{
 // WiFiClient client; // Use WiFiClient class to create TCP connections
 HTTPClient http;
/**
 * @brief json body
 * {"IoT_ID":1,"counter":15,"message":"testpost"}
 */
String payload = "{\"IoT_ID\":\"";
payload += IoT_ID;
payload += "\",\"counter\":";
payload += counter;
payload += ",\"message\":\"";
payload += message;
payload += "\"}";

 // Serial.print(url); Serial.print("\t");  Serial.println(payload);
  http.begin(url); //Specify the URL
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(payload);  //check for returing value
  SerialBT.println("Requesting URL: ");
  SerialBT.println(url);
  SerialBT.println("payload: ");
  SerialBT.println(payload);
  SerialBT.print("httpcode = ");
  SerialBT.println(httpCode);
    if (httpCode > 0) 
    { 
      String payloadResponse = http.getString();
      SerialBT.print("httpCode:");      SerialBT.println(httpCode);
      SerialBT.println("Payload Response:");
      SerialBT.println(payloadResponse);
    } 
    else 
    {
      SerialBT.println("Error on HTTP request");
    }
    http.end(); //End 
    SerialBT.println("********** End **********");
  //client.stop();  // stop client
  vTaskDelay(httpWriteInterval);    // delay
}



void TaskWebCode( void * pvParameters ){
int PeopleCountToWeb , isDataReadyToUpload;
  while(1)
  {
      if(xQueueReceive(WebFlag_queue , (void *)&isDataReadyToUpload , 0 ) == pdTRUE)
      {
        //printf("isDataReadyToUpload = %d" , isDataReadyToUpload ); //for debugging
        if(isDataReadyToUpload == 1)
        {
          SerialBT.print("Web Task is running on core ");
          SerialBT.println(xPortGetCoreID());
          if(xQueueReceive(PeopleCount_queue , (void *)&PeopleCountToWeb , 0 ) == pdTRUE)
          {
            UploadDataToWeb(URL_LINK , DEVICE_ID, PeopleCountToWeb , "logging for debugging");
          }
        }
      }

    vTaskDelay(1000);
  }

}