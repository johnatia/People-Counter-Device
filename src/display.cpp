#include "display.h"

void Display_Show(uint8_t NumToDisplay)
{
  //mapping
  uint16_t Num_2Digit;
  uint16_t NumDecoder[10] =
  {
    0b01000000,  //0
    0b01111001,  //1
    0b00100100,  //2
    0b00110000,  //3
    0b00011001,  //4
    0b00010010,  //5
    0b00000010,  //6
    0b01111000,  //7
    0b00000000,  //8
    0b00010000   //9
  };
  if(NumToDisplay < 10)
  {
    Num_2Digit |= ( (NumDecoder[NumToDisplay%10] << 8) | 0x00ff );
  }
  else
  {
   Num_2Digit = ( (NumDecoder[NumToDisplay%10]<<8) | NumDecoder[NumToDisplay/10] );
  }
  STP_Shift(Num_2Digit);
}

// void Display_OFF(void)
// {
//   Serial.println("Display is OFF");
//   STP_Shift(0xffff);
// }

// void Display_Init(void )
// {
//   STP_Init();
//   //Display People Count
// }

