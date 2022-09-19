#ifndef _EEPROM_APP_H_
#define _EEPROM_APP_H_

#include <Arduino.h>
#include "config.h"
#include "bluetooth_app.h"

#define EEPROM_SIZE                         120

#define EEPROM_THRESHOLD_ADDRESS_LSB        0
#define EEPROM_THRESHOLD_ADDRESS_MSB        1
#define EEPROM_LOCATION_ADDRESS             2
#define EEPROM_PEOPLECOUNT_ADDRESS          3
#define EEPROM_SSID_ADDRESS_START           4   
#define EEPROM_SSID_ADDRESS_END             34   
#define EEPROM_PASS_ADDRESS_START           35
#define EEPROM_PASS_ADDRESS_END             85
#define EEPROM_SELECTMODE_ADDRESS           86
#define EEPROM_DEVICE_ID_ADDRESS_START      87
#define EEPROM_DEVICE_ID_ADDRESS_END        117
#define EEPROM_DEVICE_IS_1ST_TIME_TO_BOOT   118
#if(DISPLAY_VERSION == YES)
#define EEPROM_OVER_DENSITY_ADDRESS         119
#endif



#define EEPROM_APP_WRITE_8BIT(EEPROM_APP_ADDRESS , EEPROM_APP_VALUE)                        EEPROM.write(EEPROM_APP_ADDRESS, EEPROM_APP_VALUE );\
                                                                                            EEPROM.commit()
#define EEPROM_APP_WRITE_16BIT(EEPROM_APP_ADDRESS , EEPROM_APP_VALUE)                       EEPROM.write(   EEPROM_APP_ADDRESS          ,  (EEPROM_APP_VALUE & 0x00ff)          );          \                         
                                                                                            EEPROM.write(  (EEPROM_APP_ADDRESS + 1)     , ((EEPROM_APP_VALUE & 0xff00) >> 8)    );          \
                                                                                            EEPROM.commit()

#define EEPROM_APP_READ_8BIT(   EEPROM_APP_ADDRESS    )                         EEPROM.read(EEPROM_APP_ADDRESS)
#define EEPROM_APP_READ_16BIT(  EEPROM_APP_ADDRESS    )                         eeprom_app_read_16bit(EEPROM_APP_ADDRESS)         

                



//#define eeprom_app_read_threshold(threshold)                                            threshold = (EEPROM.read(EEPROM_THRESHOLD_ADDRESS_LSB) & 0x00ff)
extern char DEVICE_ID[30];
typedef enum{
SUCCESS = 0,
ERROR = -1
}status_t;

int16_t eeprom_app_read_16bit(uint16_t eeprom_start_address);
status_t eeprom_app_1st_time_to_boot(void);
void eeprom_app_read_device_ID(void);
void eeprom_app_read_wifi(void);
status_t eeprom_app_write_str(uint8_t eeprom_start_address ,uint8_t eeprom_end_address , char * str);
void eeprom_app_read_str(uint8_t eeprom_start_address ,uint8_t eeprom_end_address , char * str);


#endif
