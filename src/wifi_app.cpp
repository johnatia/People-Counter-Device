
#include "wifi_app.h"


String ssid_str;
static int8_t WaitForPassword = 0 , WaitForSSID = 0, WiFi_No ;
int WifiNetworkNo = 0 ;
uint8_t WiFiConnectAttempt = 0;
char WiFiStatus[7][20] = {
    // "WL_NO_SHIELD "    ,
    "WL_IDLE_STATUS "     ,
    "WL_NO_SSID_AVAIL "   ,
    "WL_SCAN_COMPLETED "  ,
    "WL_CONNECTED "       ,
    "WL_CONNECT_FAILED "  ,
    "WL_CONNECTION_LOST " ,
    "WL_DISCONNECTED "   
    };






void WiFi_Init(char* SSID , char* Pass) {
  uint8_t timeout = 0 ;
  status_t status;
  WiFi.disconnect();
  vTaskDelay(1000);
  WiFi.begin(SSID, Pass);
  WiFi.waitForConnectResult();
  SerialBT.print("Connecting to ");
  SerialBT.print(SSID);
  SerialBT.print(" ..");
  while (WiFi.status() != WL_CONNECTED) {
    SerialBT.print('.');
    timeout++;
    if(timeout == 30) 
    {
      SerialBT.println("TimeOut! ");
      break;
    }
    vTaskDelay(1000);
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    SerialBT.println(WiFi.localIP());
    SerialBT.print("Wi-Fi Channel: ");
    SerialBT.println(WiFi.channel());
    status = eeprom_app_write_str(EEPROM_SSID_ADDRESS_START ,EEPROM_SSID_ADDRESS_END  , SSID );
    if(status != ERROR)   SerialBT.println("SSID is written to EEPROM susccefully! ");
    else                 SerialBT.println("Error in saving SSID to EEPROM! ");
    
    status = eeprom_app_write_str(EEPROM_PASS_ADDRESS_START ,EEPROM_PASS_ADDRESS_END  , Pass );    
    if(status != ERROR)   SerialBT.println("Password is written to EEPROM susccefully! ");
    else                 SerialBT.println("Error in saving Password to EEPROM! ");
  }
}

void WiFi_Scan(void)
{
  SerialBT.println("Wifi scan start");
  // WiFi.scanNetworks will return the number of networks found
  WifiNetworkNo = WiFi.scanNetworks();
  SerialBT.println("scan done");
  if (WifiNetworkNo == 0) {
      SerialBT.println("no networks found");
  } else {
    Serial.print(WifiNetworkNo);
    Serial.println(" networks found");
    SerialBT.print(WifiNetworkNo);
    SerialBT.println(" networks found");
    SerialBT.println("");
    SerialBT.print(0);
    SerialBT.print(": ");
    SerialBT.println("Manually Add a Network... ");
    for (int i = 0; i < WifiNetworkNo; ++i) {
      // Print SSID and RSSI for each network found
      SerialBT.print(i + 1);
      SerialBT.print(": ");
      SerialBT.print(WiFi.SSID(i));
      SerialBT.print(" (");
      SerialBT.print(WiFi.RSSI(i));
      SerialBT.print(")");
      SerialBT.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      vTaskDelay(10);
    }

  }
  SerialBT.println("");
  SerialBT.println("Please Select Network to Connect");
}

void WiFi_TryToConnect(void)
{
  int counter = 0;
  if(WaitForPassword == 1)
  {
    WiFi.disconnect();
    memset(password , 0 , sizeof(password)/sizeof(password[0]) );
    while(SerialBT.available() > 0)
    {
      password[counter++] = SerialBT.read();
      vTaskDelay(1);
    }
    // for(counter = 0; password[counter] != '\0'; counter++)
    // {
    //   SerialBT.println(password[counter]);
    // }
    password[counter-2] = '\0';
    if(WaitForSSID == 1)
    {
      WaitForSSID = 0;
    }
    else
    {
      ssid_str = WiFi.SSID(WiFi_No-1);
      ssid = &ssid_str[0]; 
    }

    // SerialBT.print("ssid = ");
    // SerialBT.println(ssid);
    // SerialBT.print("password = ");
    // SerialBT.println(password);
    WaitForPassword = 0;
    WiFi_GetPassword = 1;
    lastlogin = 0;
  }
  else if(WaitForSSID == 1)
  {
   Serial.println("start to read ssid");
   
    memset(temp_ssid, 0 , sizeof(temp_ssid)/ sizeof(temp_ssid[0]) );
    while(SerialBT.available() > 0)
    {
      Serial.println("reading ssid");
      temp_ssid[counter++] = SerialBT.read();
      vTaskDelay(1);
    }
    temp_ssid[counter-2] = '\0';
    ssid = temp_ssid;
      SerialBT.print("Enter Password of ");
      SerialBT.println(ssid);
      WaitForPassword = 1;
  }
  else
  {
    WiFi_No = BLE_ReadData();
    if (WiFi_No == 0)
    {
      SerialBT.println("please Enter SSID ");
      WiFiConnectAttempt = 0;
      WaitForSSID = 1;
    }

    else  if(( WiFi_No > WifiNetworkNo ) ||  (WiFi_No < 0) )
    {
      WiFiConnectAttempt++;
      if(WiFiConnectAttempt > 5)
      {
        WiFiConnectAttempt = 0;
        lastlogin = 0;
        SerialBT.println("Multiple Failed attempts, Try again later");
      }
      else      SerialBT.println("Please Enter Correct Value!");
    }
    else
    {
      SerialBT.print("Wifi no = ");
      SerialBT.println(WiFi_No);
      SerialBT.print("Enter Password of ");
      SerialBT.println(WiFi.SSID(WiFi_No-1));
      SerialBT.println("please Enter the Password ");
      WiFiConnectAttempt = 0;
      WaitForPassword = 1;
    }

  }
}


