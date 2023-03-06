#include "AD5621.h"

#define TAG "AD5621"

// Write to AD7190
void AD5621_Write(uint16_t Data)
{
  uint8_t i;

  gpio_set_level(AOSS, 0); // Chip select low
  for (i = 0; i < 16; i++)
  {
    gpio_set_level(AOCLK, 1); // Clock high
    if ((Data & 0x8000) == 0x8000)
      gpio_set_level(AOMOSI, 1); // Write a one
    else
      gpio_set_level(AOMOSI, 0); // Write a zero
    gpio_set_level(AOCLK, 0);    // Clock low
    Data = Data << 1;
  }
  gpio_set_level(AOSS, 1); // Chip select high
}

// Calibrate AD5621
void AD5621_Calib(bool *A420Calib, struct A420Values *A420Value)
{
  static bool CalibZeroFull = false;
  char ReturnStr[10];
  uint8_t FramData[10];

  if ((RXReg[0] == 'A') & (RXReg[1] == '4') & (RXReg[2] == 'W'))
  {
    strcpy(ReturnStr, "A4WNOK");
    // Zero calibration
    if ((RXReg[3] == 'Z') & (RXReg[4] == 'C'))
    {
      if (RXReg[5] == '#')
      {
        strcpy(ReturnStr, "A4WZC-OK");
        A420Value->CalibValue = A420ZERO;
        *A420Calib = true;
        CalibZeroFull = false;
      }
      else
        strcpy(ReturnStr, "A4WZC-NOK");
    }
    // Full calibration
    if ((RXReg[3] == 'F') & (RXReg[4] == 'C'))
    {
      if (RXReg[5] == '#')
      {
        strcpy(ReturnStr, "A4WFC-OK");
        A420Value->CalibValue = A420FULL;
        *A420Calib = true;
        CalibZeroFull = true;
      }
      else
        strcpy(ReturnStr, "A4WFC-NOK");
    }
    // Change value
    if ((RXReg[3] == 'C') & (RXReg[4] == 'H') & (RXReg[5] == '-'))
    {
      if ((RXReg[7] == '#') & (*A420Calib))
      {
        // Up by one
        if (RXReg[6] == '+')
        {
          if (A420Value->CalibValue < A420FULL)
            A420Value->CalibValue++;
          strcpy(ReturnStr, "A4WCH-OK");
        }
        // Up by ten
        else if (RXReg[6] == 'U')
        {
          if (A420Value->CalibValue < (A420FULL - 10))
            A420Value->CalibValue = A420Value->CalibValue + 10;
          strcpy(ReturnStr, "A4WCH-OK");
        }
        // Down by one
        else if (RXReg[6] == '-')
        {
          if (A420Value->CalibValue > A420ZERO)
            A420Value->CalibValue--;
          strcpy(ReturnStr, "A4WCH-OK");
        }
        // Down by ten
        else if (RXReg[6] == 'D')
        {
          if (A420Value->CalibValue > (A420ZERO + 10))
            A420Value->CalibValue = A420Value->CalibValue - 10;
          strcpy(ReturnStr, "A4WCH-OK");
        }
        else
          strcpy(ReturnStr, "A4WCH-NOK");
        ESP_LOGI(TAG, "Value: %d", A420Value->CalibValue);
      }
      else
        strcpy(ReturnStr, "A4WCH-NOK");
    }
    // Save calibration
    if ((RXReg[3] == 'S') & (RXReg[4] == 'C'))
    {
      if ((RXReg[5] == '#') & (*A420Calib))
      {
        strcpy(ReturnStr, "A4WSC-OK");
        // Save zero calibration
        if (!CalibZeroFull)
        {
          A420Value->CalibZero = A420Value->CalibValue;
          // Save in FRAM
          memcpy(FramData, &A420Value->CalibZero, 2);
          Fram_Update(FramData, ACALIBZERO, ACALIBZEROL, ACALIBZEROU);
          *A420Calib = false;
        }
        else
        {
          A420Value->CalibFull = A420Value->CalibValue;
          // Save in FRAM
          memcpy(FramData, &A420Value->CalibFull, 2);
          Fram_Update(FramData, ACALIBFULL, ACALIBFULLL, ACALIBFULLU);
          A420Value->CalibRatio = ((float)(A420Value->CalibFull - A420Value->CalibZero)) / A420Value->CalibWeight;
          memcpy(FramData, &A420Value->CalibRatio, 4);
          Fram_Update(FramData, ACALIBRATIO, ACALIBRATIOL, ACALIBRATIOU);
          *A420Calib = false;
        }
      }
      else
        strcpy(ReturnStr, "A4WSC-NOK");
    }
    // Save maximum reference weight
    // Calibration weight
    else if ((RXReg[3] == 'M') & (RXReg[4] == 'W') & (RXReg[5] == '-'))
    {
      char str[11];
      uint8_t i = 0;
      uint32_t temp32;

      // Find # and replace with 0 (to turn into a string)
      while ((RXReg[6 + i] != '#') & (i < 11))
      {
        str[i] = RXReg[6 + i];
        i++;
      }
      if ((RXReg[6 + i] == '#') & (i < 11))
      {
        str[i] = 0;
        temp32 = (uint32_t)atoi(str);
        A420Value->CalibWeight = temp32;
        strcpy(ReturnStr, "A4WMW-OK");
        memcpy(FramData, &A420Value->CalibWeight, 4);
        Fram_Update(FramData, ACALIBWEIGHT, ACALIBWEIGHTL, ACALIBWEIGHTU);
      }
      else
        strcpy(ReturnStr, "A4WMW-NOK");
    }
    // Send ethernet response
    for (uint8_t i = 0; i < strlen(ReturnStr); i++)
      EthData[i + (256 * (EthSocket - 1))] = ReturnStr[i];
    LenTX[EthSocket] = strlen(ReturnStr);
    EthProgress[EthSocket] = 6;
  }
}

// Load AD5621 structure
void AD5621_Load(struct A420Values *A420Value)
{
  uint8_t ADData[10];

  // Zero calibration amount
  if (Fram_Loc_Updated(ACALIBZEROU))
  {
    Fram_Read(ACALIBZERO, ADData, ACALIBZEROL);
    memcpy(&A420Value->CalibZero, ADData, 4);
    ESP_LOGI(TAG, "Zero calibration amount from FRAM: %d", A420Value->CalibZero);
  }
  else
    A420Value->CalibZero = 0;
  // Full calibration amount
  if (Fram_Loc_Updated(ACALIBFULLU))
  {
    Fram_Read(ACALIBFULL, ADData, ACALIBFULLL);
    memcpy(&A420Value->CalibFull, ADData, 4);
    ESP_LOGI(TAG, "Full calibration amount from FRAM: %d", A420Value->CalibFull);
  }
  else
    A420Value->CalibFull = 0;
  // Calibration weight in grams
  if (Fram_Loc_Updated(ACALIBWEIGHTU))
  {
    Fram_Read(ACALIBWEIGHT, ADData, ACALIBWEIGHTL);
    memcpy(&A420Value->CalibWeight, ADData, 4);
    ESP_LOGI(TAG, "Calibration weight from FRAM: %d", A420Value->CalibWeight);
  }
  else
    A420Value->CalibWeight = 1000;
  // Calibration ratio
  if (Fram_Loc_Updated(ACALIBRATIOU))
  {
    Fram_Read(ACALIBRATIO, ADData, ACALIBRATIOL);
    memcpy(&A420Value->CalibRatio, ADData, 4);
    ESP_LOGI(TAG, "Calibration ratio from FRAM: %4.2f", A420Value->CalibRatio);
  }
  else
    A420Value->CalibRatio = 0;
  // Calibration value
  A420Value->CalibValue = 0;
}

void AD5621_Read(struct A420Values *A420Value)
{
  char str[20];
  char ReturnStr[40];

  if ((RXReg[0] == 'A') & (RXReg[1] == '4') & (RXReg[2] == 'R'))
  {
    strcpy(ReturnStr, "A4RNOK");
    if ((RXReg[3] == 'Z') & (RXReg[4] == 'C'))
    {
      IntToString(str, A420Value->CalibZero);
      strcpy(ReturnStr, "A4RZC: ");
      strcat(ReturnStr, str);
    }
    else if ((RXReg[3] == 'F') & (RXReg[4] == 'C'))
    {
      IntToString(str, A420Value->CalibFull);
      strcpy(ReturnStr, "A4RFC: ");
      strcat(ReturnStr, str);
    }
    else if ((RXReg[3] == 'M') & (RXReg[4] == 'W'))
    {
      IntToString(str, A420Value->CalibWeight);
      strcpy(ReturnStr, "A4RMW: ");
      strcat(ReturnStr, str);
      strcat(ReturnStr, "g");
    }
    else if ((RXReg[3] == 'R') & (RXReg[4] == 'A'))
    {
      double d;
      d = A420Value->CalibRatio;
      sprintf(str, "%d.%02u", (int)d, (int)((d - (int)d) * 100));
      strcpy(ReturnStr, "A4RRA: ");
      strcat(ReturnStr, str);
    }
    // Send ethernet response
    for (uint8_t i = 0; i < strlen(ReturnStr); i++)
      EthData[i + (256 * (EthSocket - 1))] = ReturnStr[i];
    LenTX[EthSocket] = strlen(ReturnStr);
    EthProgress[EthSocket] = 6;
  }
}