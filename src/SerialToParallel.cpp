#include "SerialToParallel.h"

void STP_Init(void)
{
  pinMode(STP_CLK  , OUTPUT);
  pinMode(STP_LATCH  , OUTPUT);
  pinMode(STP_DATA  , OUTPUT);
}

void STP_Shift(uint16_t SerialData)
{
	int8_t s8Counter=0;
	for(s8Counter=15; s8Counter>=0; s8Counter--)
	{
		//Send bit by bit starting by MSB
		digitalWrite(STP_DATA, GET_BIT(SerialData , s8Counter));
		//Send Pulse
		digitalWrite(STP_CLK, HIGH);
		vTaskDelay(1);
		digitalWrite(STP_CLK, LOW);
		vTaskDelay(1);
	}
	//Send pulse to Storage CLK
	digitalWrite(STP_LATCH, HIGH);
	vTaskDelay(1);
	digitalWrite(STP_LATCH, LOW);
	vTaskDelay(1);

}