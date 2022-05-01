#ifndef _SERIAL_TO_PARALLEL_H_
#define _SERIAL_TO_PARALLEL_H_

#include <Arduino.h>
#include "display.h"

#define STP_CLK   DISP_STP_CLK
#define STP_LATCH DISP_STP_LATCH
#define STP_DATA  DISP_STP_DATA
#define GET_BIT(VAR,BIT)											((VAR >> BIT) & 1	)

void STP_Init(void);
void STP_Shift(uint16_t SerialData);


#endif