#include "ota_app.h"

xTaskHandle ota_uploadFirmwareHandle;

void ota_setup(void)
{
  ArduinoOTA.setHostname("people_counter_upload_firmware");
    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      SerialBT.println("Start updating " + type);
    })
    .onEnd([]() {
      SerialBT.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      SerialBT.printf("Progress: %u%%\n", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      SerialBT.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) SerialBT.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) SerialBT.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) SerialBT.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) SerialBT.println("Receive Failed");
      else if (error == OTA_END_ERROR) SerialBT.println("End Failed");
    });

  ArduinoOTA.begin();

  SerialBT.println("Ready");
  SerialBT.print("IP address: ");
  SerialBT.println(WiFi.localIP());

}


void ota_uploadFirmwareTask(void *pvParameters)
{
    ota_setup();
    SerialBT.println("disable panic reseting by wdt");
    esp_task_wdt_init(10 , false);
    while(1)
    {
        ArduinoOTA.handle();
        vTaskDelay(1000);  
    }
}
void ota_xTaskCreate(void)
{
    xTaskCreatePinnedToCore(
                    ota_uploadFirmwareTask,   /* Task function. */
                    "Task Update Firmware OTA",     /* name of task. */
                    2000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    11,           /* priority of the task */
                    &ota_uploadFirmwareHandle,      /* Task handle to keep track of created task */
                    1);

}


