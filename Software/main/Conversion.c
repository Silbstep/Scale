#include "Conversion.h"

void IntToString(char *str, int Number)
{
  bool Negative = false;
  uint8_t i = 0, j;
  char tempstr[30];

  if (Number < 0)
  {
    Negative = true;
    Number = 0 - Number;
  }
  while (Number > 0)
  {
    tempstr[i] = Number % 10;
    Number = Number / 10;
    i++;
  }
  if (i > 0)
  {
    if (Negative)
      i++;
    for (j = 0; j < i; j++)
    {
      if (Negative)
      {
        str[j] = '-';
        Negative = false;
      }
      else
        str[j] = tempstr[i - j - 1] + 0x30;
    }
    str[j] = 0;
  }
  else
    strcpy(str, "0");
}