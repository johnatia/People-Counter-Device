#include "eeprom_app.h"


char DEVICE_ID[30] = "People Counter";

int16_t eeprom_app_read_16bit(uint16_t eeprom_start_address)
{
    int16_t eeprom_value;
    eeprom_value = (EEPROM.read(eeprom_start_address) & 0x00ff);                                                     
    eeprom_value |= ( ((uint16_t)EEPROM.read(eeprom_start_address+1)<<8) & 0xff00 ) ;      
    return    eeprom_value;                                  
}

status_t eeprom_app_1st_time_to_boot(void)
{ status_t status ;
  EEPROM_APP_WRITE_16BIT(EEPROM_THRESHOLD_ADDRESS_LSB , Threshold);
	EEPROM_APP_WRITE_8BIT(EEPROM_LOCATION_ADDRESS, OUTDOOR );
	EEPROM_APP_WRITE_8BIT(EEPROM_PEOPLECOUNT_ADDRESS, PeopleCount  );
#if (DISPLAY_VERSION == YES)
	EEPROM_APP_WRITE_8BIT(EEPROM_OVER_DENSITY_ADDRESS , OverDensity);
#endif
  status = eeprom_app_write_str(EEPROM_DEVICE_ID_ADDRESS_START , EEPROM_DEVICE_ID_ADDRESS_END , DEVICE_ID);
  status = eeprom_app_write_str(EEPROM_SSID_ADDRESS_START , EEPROM_SSID_ADDRESS_END , ssid);
  status = eeprom_app_write_str(EEPROM_PASS_ADDRESS_START , EEPROM_PASS_ADDRESS_END , password);
  EEPROM_APP_WRITE_8BIT(EEPROM_DEVICE_IS_1ST_TIME_TO_BOOT , 'N');
  return status;
}

status_t eeprom_app_write_str(uint8_t eeprom_start_address ,uint8_t eeprom_end_address , char * str)
{
  int count;  status_t status;
  for(count = 0; str[count] != '\0'; count++)
  {
    EEPROM.write( (count + eeprom_start_address ), str[count]);
  }
  EEPROM.write( (count + eeprom_start_address ), '\0');
  if(count < (eeprom_end_address - eeprom_start_address) ) { EEPROM.commit(); status = SUCCESS; } 
  else                                                     { status = ERROR;                 }
  return status;
}

void eeprom_app_read_str(uint8_t eeprom_start_address ,uint8_t eeprom_end_address , char * str)
{
    for(int count = 0; count < (eeprom_end_address - eeprom_start_address); count++)
    {
      str[count] = EEPROM.read(count  +   eeprom_start_address);
    }
}

void eeprom_app_read_device_ID(void)
{
  eeprom_app_read_str(EEPROM_DEVICE_ID_ADDRESS_START , EEPROM_DEVICE_ID_ADDRESS_END , DEVICE_ID);
  SerialBT.print("DEVICE ID in EEPROM = ");
  SerialBT.println(DEVICE_ID);
}

void  eeprom_app_read_wifi(void)
{
  SerialBT.print("WiFi Status = ");
  SerialBT.println(WiFiStatus[WiFi.status()]);
  memset(temp_ssid, 0 , sizeof(temp_ssid)/ sizeof(temp_ssid[0]) );
  memset(password , 0 , sizeof(password)/sizeof(password[0]) );
  Serial.print("ssid before eeprom = ");
  Serial.println(temp_ssid);  
  Serial.print("password before eeprom = ");
  Serial.println(password);
  eeprom_app_read_str(EEPROM_SSID_ADDRESS_START , EEPROM_SSID_ADDRESS_END , ssid);
  eeprom_app_read_str(EEPROM_PASS_ADDRESS_START , EEPROM_PASS_ADDRESS_END , password);   
  SerialBT.print("SSID in EEPROM = ");
  SerialBT.println(ssid);
  SerialBT.print("Password in EEPROM = ");
  SerialBT.println(password);   
}
