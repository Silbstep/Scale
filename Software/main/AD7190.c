#include "AD7190.h"

#define TAG "AD7190"

// Reset AD7190
void AD7190_Reset()
{
  uint8_t i, AIBuffer[5] = {0};
  for (i = 0; i < 5; i++)
    AIBuffer[i] = 0xFF;      // Load buffer with AD7190 reset value
  AD7190_Write(5, AIBuffer); // Write buffer value to AD7190
}

// Data ready
bool AD7190_Ready(void)
{
  bool ReturnValue = false;
  uint8_t AIBuffer[1];
  AIBuffer[0] = 0x00;        // Write to communication register
  AD7190_Write(1, AIBuffer); // Write buffer value to AD7190
  AD7190_Read(1, AIBuffer);  // Read ID
  if ((AIBuffer[0] & 0x80) == 0)
    ReturnValue = true;
  return (ReturnValue);
}

// Read ID
uint8_t AD7190_ReadID(void)
{
  uint8_t AIBuffer[1];
  AIBuffer[0] = 0x40;        // Write to communication register
  AD7190_Write(1, AIBuffer); // Write buffer value to AD7190
  AD7190_Read(1, AIBuffer);  // Read ID
  return (AIBuffer[0]);
}

// Read from AD7190
uint32_t AD7190_Read(uint8_t Count, uint8_t *Buf)
{
  uint8_t i, j, temp, ReadValue = 0;
  uint32_t temp32;

  Pin_Low(PASS); // Chip select low
  for (i = Count; i > 0; i--)
  {
    for (j = 0; j < 8; j++)
    {
      Pin_Low(PACLK); // Clock Low
      ReadValue = ReadValue << 1;
      temp = gpio_get_level(AMISO);
      Pin_High(PACLK); // Clock High
      if (temp)
        ReadValue = ReadValue | 0x01; // Read a one (default is zero)
    }
    *(Buf + i - 1) = ReadValue;
  }
  Pin_High(PASS); // Chip select high
  temp32 = (Buf[3] << 24) + (Buf[2] << 16) + (Buf[1] << 8) + Buf[0];
  return (temp32);
}

// Write to AD7190
void AD7190_Write(uint8_t Count, uint8_t *Buf)
{
  uint8_t i, j, ValueToWrite;

  Pin_Low(PASS); // Chip select low
  for (i = Count; i > 0; i--)
  {
    ValueToWrite = *(Buf + i - 1); // Bitbang
    for (j = 0; j < 8; j++)
    {
      Pin_Low(PACLK); // Clock Low
      if ((ValueToWrite & 0x80) == 0x80)
        Pin_High(PAMOSI); // Write a one
      else
        Pin_Low(PAMOSI); // Write a zero
      Pin_High(PACLK);   // Clock High
      ValueToWrite = ValueToWrite << 1;
    }
  }
  Pin_High(PASS); // Chip select high
}

bool AD7190_Mode(struct AD7190Mode *AD7190Modes)
{
  bool ReturnValue = true;
  uint8_t i;
  uint8_t Mode[3]; // Mode[0] is MR0 to MR7, Mode[1] is MR8 to MR15 and Mode[2] is MR16 to MR23
  uint8_t AIBuffer[4];
  uint16_t Rate;

  Rate = (uint16_t)(4804.6875 / AD7190Modes->Rate);
  Mode[0] = (uint8_t)Rate;
  Mode[1] = (uint8_t)(Rate >> 8);
  if (AD7190Modes->Rej == REJ60)
    Mode[1] = Mode[1] | REJ60;
  if (AD7190Modes->Single == SINGLEEN)
    Mode[1] = Mode[1] | SINGLEEN;
  if (AD7190Modes->Parity == PARITYEN)
    Mode[1] = Mode[1] | PARITYEN;
  if (AD7190Modes->Sinc == SINC3)
    Mode[1] = Mode[1] | SINC3;
  Mode[2] = 0;
  Mode[2] = Mode[2] + AD7190Modes->Clock;
  if (AD7190Modes->StatusTrans == STATUSTRANSON)
    Mode[2] = Mode[2] | STATUSTRANSON;
  Mode[2] = Mode[2] + AD7190Modes->Mode;
  ESP_LOGI(TAG, "Mode0: %2x, Mode1: %2x, Mode2: %2x", Mode[0], Mode[1], Mode[2]);
  AIBuffer[0] = 0x08; // Initiate write to mode register
  AD7190_Write(1, AIBuffer);
  AD7190_Write(3, Mode); // Write mode values
  AIBuffer[0] = 0x48;    // Initiate read from mode register
  AD7190_Write(1, AIBuffer);
  AD7190_Read(3, AIBuffer);
  ESP_LOGI(TAG, "Read0: %2x, Read1: %2x, Read2: %2x", AIBuffer[0], AIBuffer[1], AIBuffer[2]);
  for (i = 0; i < 3; i++)
    if (Mode[i] != AIBuffer[i])
      ReturnValue = false;
  return (ReturnValue);
}

bool AD7190_Config(struct AD7190Config *AD7190Configs)
{
  bool ReturnValue = true;
  uint8_t i;
  uint8_t Config[3]; // Config[0] is CON0 to CON7, Config[1] is CON8 to CON15 and Config[2] is CON16 to CON23
  uint8_t AIBuffer[4];
  uint32_t temp32;

  Config[0] = 0;
  Config[0] = Config[0] + AD7190Configs->Gain;
  if (AD7190Configs->Polarity == UNIPOLAR)
    Config[0] = Config[0] | UNIPOLAR;
  if (AD7190Configs->Buffer == BUFFERON)
    Config[0] = Config[0] | BUFFERON;
  if (AD7190Configs->RefDet == REFDETON)
    Config[0] = Config[0] | REFDETON;
  if (AD7190Configs->Burn == BURNON)
    Config[0] = Config[0] | BURNON;
  Config[1] = AD7190Configs->Channel;
  Config[2] = 0;
  if (AD7190Configs->Refsel == REFSEL2)
    Config[2] = Config[2] | REFSEL2;
  if (AD7190Configs->Chop == CHOPON)
    Config[2] = Config[2] | CHOPON;
  ESP_LOGI(TAG, "Config0: %2x, Config1: %2x, Config2: %2x", Config[0], Config[1], Config[2]);
  AIBuffer[0] = 0x10; // Initiate write to config register
  AD7190_Write(1, AIBuffer);
  AD7190_Write(3, Config); // Write config values
  AIBuffer[0] = 0x50;      // Initiate read from config register
  AD7190_Write(1, AIBuffer);
  temp32 = AD7190_Read(3, AIBuffer);
  ESP_LOGI(TAG, "Read0: %2x, Read1: %2x, Read2: %2x", AIBuffer[0], AIBuffer[1], AIBuffer[2]);
  ESP_LOGI(TAG, "Temp32: %8x", temp32);
  for (i = 0; i < 3; i++)
    if (Config[i] != AIBuffer[i])
      ReturnValue = false;
  return (ReturnValue);
}

void AD7190_Load(struct AD7190Mode *AD7190Modes, struct AD7190Config *AD7190Configs)
{
  uint8_t ADData[4];

  // Mode
  if (Fram_Loc_Updated(ADMODEU))
  {
    Fram_Read(ADMODE, ADData, 1);
    AD7190Modes->Mode = ADData[0];
    ESP_LOGI(TAG, "Mode from FRAM: %2X", AD7190Modes->Mode);
  }
  else
  {
    AD7190Modes->Mode = CONTINOUS;
    ESP_LOGI(TAG, "Mode default: %2X", AD7190Modes->Mode);
  }
  // Status transmit
  if (Fram_Loc_Updated(ADSTRANSU))
  {
    Fram_Read(ADSTRANS, ADData, 1);
    AD7190Modes->StatusTrans = ADData[0];
    ESP_LOGI(TAG, "Status transmit from FRAM: %2X", AD7190Modes->StatusTrans);
  }
  else
  {
    AD7190Modes->StatusTrans = STATUSTRANSOFF;
    ESP_LOGI(TAG, "Status transmit default: %2X", AD7190Modes->StatusTrans);
  }
  // Clock type
  if (Fram_Loc_Updated(ADCLOCKU))
  {
    Fram_Read(ADCLOCK, ADData, 1);
    AD7190Modes->Clock = ADData[0];
    ESP_LOGI(TAG, "Clock from FRAM: %2X", AD7190Modes->Clock);
  }
  else
  {
    AD7190Modes->Clock = INTCLOCKTRI;
    ESP_LOGI(TAG, "Clock default: %2X", AD7190Modes->Clock);
  }
  // Sinc
  if (Fram_Loc_Updated(ADSINCU))
  {
    Fram_Read(ADSINC, ADData, 1);
    AD7190Modes->Sinc = ADData[0];
    ESP_LOGI(TAG, "Sinc from FRAM: %2X", AD7190Modes->Sinc);
  }
  else
  {
    AD7190Modes->Sinc = SINC4;
    ESP_LOGI(TAG, "Sinc default: %2X", AD7190Modes->Sinc);
  }
  // Parity
  if (Fram_Loc_Updated(ADPARITYU))
  {
    Fram_Read(ADPARITY, ADData, 1);
    AD7190Modes->Parity = ADData[0];
    ESP_LOGI(TAG, "Parity from FRAM: %2X", AD7190Modes->Parity);
  }
  else
  {
    AD7190Modes->Parity = PARITYDIS;
    ESP_LOGI(TAG, "Parity default: %2X", AD7190Modes->Parity);
  }
  // Single reading
  if (Fram_Loc_Updated(ADSINGLEU))
  {
    Fram_Read(ADSINGLE, ADData, 1);
    AD7190Modes->Single = ADData[0];
    ESP_LOGI(TAG, "Single reading from FRAM: %2X", AD7190Modes->Single);
  }
  else
  {
    AD7190Modes->Single = SINGLEDIS;
    ESP_LOGI(TAG, "Single reading default: %2X", AD7190Modes->Single);
  }
  // Activate REJ50 or REJ60
  if (Fram_Loc_Updated(ADREJU))
  {
    Fram_Read(ADREJ, ADData, 1);
    AD7190Modes->Rej = ADData[0];
    ESP_LOGI(TAG, "REJ from FRAM: %2X", AD7190Modes->Rej);
  }
  else
  {
    AD7190Modes->Rej = REJ50;
    ESP_LOGI(TAG, "REJ default: %2X", AD7190Modes->Rej);
  }
  // Rate of conversion
  if (Fram_Loc_Updated(ADRATEU))
  {
    Fram_Read(ADRATE, ADData, ADRATEL);
    uint32_t temp32;
    float temp32f;
    temp32 = (ADData[3] << 24) + (ADData[2] << 16) + (ADData[1] << 8) + ADData[0];
    memcpy(&temp32f, &temp32, 4);
    AD7190Modes->Rate = temp32f;
    ESP_LOGI(TAG, "Rate from FRAM: %2.2f", AD7190Modes->Rate);
  }
  else
  {
    AD7190Modes->Rate = (float)4.7;
    ESP_LOGI(TAG, "Rate default: %2.6f", AD7190Modes->Rate);
  }
  // Gain
  if (Fram_Loc_Updated(ADGAINU))
  {
    Fram_Read(ADGAIN, ADData, ADGAINL);
    AD7190Configs->Gain = ADData[0];
    ESP_LOGI(TAG, "Gain from FRAM: %2X", AD7190Configs->Gain);
  }
  else
  {
    AD7190Configs->Gain = GAIN128;
    ESP_LOGI(TAG, "Gain default: %2X", AD7190Configs->Gain);
  }
  // Polarity
  if (Fram_Loc_Updated(ADPOLARITYU))
  {
    Fram_Read(ADPOLARITY, ADData, 1);
    AD7190Configs->Polarity = ADData[0];
    ESP_LOGI(TAG, "Polarity from FRAM: %2X", AD7190Configs->Polarity);
  }
  else
  {
    AD7190Configs->Polarity = BIPOLAR;
    ESP_LOGI(TAG, "Polarity default: %2X", AD7190Configs->Polarity);
  }
  // Buffer
  if (Fram_Loc_Updated(ADBUFFERU))
  {
    Fram_Read(ADBUFFER, ADData, 1);
    AD7190Configs->Buffer = ADData[0];
    ESP_LOGI(TAG, "Buffer from FRAM: %2X", AD7190Configs->Buffer);
  }
  else
  {
    AD7190Configs->Buffer = BUFFEROFF;
    ESP_LOGI(TAG, "Buffer default: %2X", AD7190Configs->Buffer);
  }
  // Reference detect
  if (Fram_Loc_Updated(ADREFDETU))
  {
    Fram_Read(ADREFDET, ADData, 1);
    AD7190Configs->RefDet = ADData[0];
    ESP_LOGI(TAG, "Reference detect from FRAM: %2X", AD7190Configs->RefDet);
  }
  else
  {
    AD7190Configs->RefDet = REFDETON;
    ESP_LOGI(TAG, "Reference detect default: %2X", AD7190Configs->RefDet);
  }
  // Burn
  if (Fram_Loc_Updated(ADBURNU))
  {
    Fram_Read(ADBURN, ADData, 1);
    AD7190Configs->Burn = ADData[0];
    ESP_LOGI(TAG, "Burn from FRAM: %2X", AD7190Configs->Burn);
  }
  else
  {
    AD7190Configs->Burn = BURNOFF;
    ESP_LOGI(TAG, "Burn default: %2X", AD7190Configs->Burn);
  }
  // Channel
  if (Fram_Loc_Updated(ADCHANNELU))
  {
    Fram_Read(ADCHANNEL, ADData, 1);
    AD7190Configs->Channel = ADData[0];
    ESP_LOGI(TAG, "Channel from FRAM: %2X", AD7190Configs->Channel);
  }
  else
  {
    AD7190Configs->Channel = CHANNEL0;
    ESP_LOGI(TAG, "Channel default: %2X", AD7190Configs->Channel);
  }
  // Select reference
  if (Fram_Loc_Updated(ADREFSELU))
  {
    Fram_Read(ADREFSEL, ADData, 1);
    AD7190Configs->Refsel = ADData[0];
    ESP_LOGI(TAG, "Reference from FRAM: %2X", AD7190Configs->Refsel);
  }
  else
  {
    AD7190Configs->Refsel = REFSEL2;
    ESP_LOGI(TAG, "Reference default: %2X", AD7190Configs->Refsel);
  }
  // Chop
  if (Fram_Loc_Updated(ADCHOPU))
  {
    Fram_Read(ADCHOP, ADData, 1);
    AD7190Configs->Chop = ADData[0];
    ESP_LOGI(TAG, "Chop from FRAM: %2X", AD7190Configs->Chop);
  }
  else
  {
    AD7190Configs->Chop = CHOPOFF;
    ESP_LOGI(TAG, "Chop default: %2X", AD7190Configs->Chop);
  }
}

bool AD7190_ModeLoad(struct AD7190Mode *AD7190Modes)
{
  bool InstructionOK = false;
  char ReturnStr[10];
  uint8_t FramData[10];

  // Update FRAM
  if ((RXReg[0] == 'A') & (RXReg[1] == 'M') & (RXReg[2] == 'W'))
  {
    strcpy(ReturnStr, "AMWNOK");
    // Mode
    if ((RXReg[3] == 'M') & (RXReg[4] == 'O') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "AMWMO-OK");
        switch (RXReg[6] - 0x30)
        {
        case 0:
          AD7190Modes->Mode = CONTINOUS;
          break;
        case 1:
          AD7190Modes->Mode = SINGLE;
          break;
        case 2:
          AD7190Modes->Mode = IDLE;
          break;
        case 3:
          AD7190Modes->Mode = POWERDOWN;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "AMWMO-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Modes->Mode;
          Fram_Update(FramData, ADMODE, 1, ADMODEU);
        }
      }
      else
        strcpy(ReturnStr, "AMWMO-NOK");
    }
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'T') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "AMWST-OK");
        switch (RXReg[6] - 0x30)
        {
        case 0:
          AD7190Modes->StatusTrans = STATUSTRANSOFF;
          break;
        case 1:
          AD7190Modes->StatusTrans = STATUSTRANSON;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "AMWST-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Modes->StatusTrans;
          Fram_Update(FramData, ADSTRANS, 1, ADSTRANSU);
        }
      }
      else
        strcpy(ReturnStr, "AMWST-NOK");
    }
    else if ((RXReg[3] == 'C') & (RXReg[4] == 'L') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "AMWCL-OK");
        switch (RXReg[6] - 0x30)
        {
        case 0:
          AD7190Modes->Clock = EXTCRYSTAL;
          break;
        case 1:
          AD7190Modes->Clock = EXTCLOCK;
          break;
        case 2:
          AD7190Modes->Clock = INTCLOCKTRI;
          break;
        case 3:
          AD7190Modes->Clock = INTCLOCK;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "AMWCL-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Modes->Clock;
          Fram_Update(FramData, ADCLOCK, 1, ADCLOCKU);
        }
      }
      else
        strcpy(ReturnStr, "AMWCL-NOK");
    }
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'I') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "AMWSI-OK");
        switch (RXReg[6] - 0x30)
        {
        case 3:
          AD7190Modes->Sinc = SINC3;
          break;
        case 4:
          AD7190Modes->Sinc = SINC4;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "AMWSI-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Modes->Sinc;
          Fram_Update(FramData, ADSINC, 1, ADSINCU);
        }
      }
      else
        strcpy(ReturnStr, "AMWSI-NOK");
    }
    else if ((RXReg[3] == 'P') & (RXReg[4] == 'A') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "AMWPA-OK");
        switch (RXReg[6] - 0x30)
        {
        case 0:
          AD7190Modes->Parity = PARITYDIS;
          break;
        case 1:
          AD7190Modes->Parity = PARITYEN;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "AMWPA-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Modes->Parity;
          Fram_Update(FramData, ADPARITY, 1, ADPARITYU);
        }
      }
      else
        strcpy(ReturnStr, "AMWPA-NOK");
    }
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'N') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "AMWSN-OK");
        switch (RXReg[6] - 0x30)
        {
        case 0:
          AD7190Modes->Single = SINGLEDIS;
          break;
        case 1:
          AD7190Modes->Single = SINGLEEN;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "AMWSN-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Modes->Single;
          Fram_Update(FramData, ADSINGLE, 1, ADSINGLEU);
        }
      }
      else
        strcpy(ReturnStr, "AMWSN-NOK");
    }
    else if ((RXReg[3] == 'R') & (RXReg[4] == 'E') & (RXReg[5] == '-'))
    {
      if (RXReg[8] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "AMWRE-OK");
        switch (((RXReg[6] - 0x30) * 10) + (RXReg[7] - 0x30))
        {
        case 50:
          AD7190Modes->Rej = REJ50;
          break;
        case 60:
          AD7190Modes->Rej = REJ60;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "AMWRE-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Modes->Rej;
          Fram_Update(FramData, ADREJ, 1, ADREJU);
        }
      }
      else
        strcpy(ReturnStr, "AMWRE-NOK");
    }
    else if ((RXReg[3] == 'R') & (RXReg[4] == 'A') & (RXReg[5] == '-'))
    {
      char str[11];
      uint8_t i = 0;
      float tempf;

      InstructionOK = false;
      // Find # and replace with 0 (to turn into a string)
      while ((RXReg[6 + i] != '#') & (i < 8))
      {
        str[i] = RXReg[6 + i];
        i++;
      }
      if ((RXReg[6 + i] == '#') & (i < 8))
      {
        str[i] = 0;
        tempf = (float)atof(str);
        if ((tempf >= 4.69) & (tempf <= 4800.0))
        {
          InstructionOK = true;
          strcpy(ReturnStr, "AMWRA-OK");
          AD7190Modes->Rate = tempf;
          memcpy(FramData, &tempf, 4);
          Fram_Update(FramData, ADRATE, ADRATEL, ADRATEU);
        }
        else
          strcpy(ReturnStr, "AMWRA-NOK");
      }
      else
        strcpy(ReturnStr, "AMWRA-NOK");
    }
    // Send response
    for (uint8_t i = 0; i < strlen(ReturnStr); i++)
      EthData[i + (256 * (EthSocket - 1))] = ReturnStr[i];
    LenTX[EthSocket] = strlen(ReturnStr);
    EthProgress[EthSocket] = 6;
  }
  // Update AD7190
  if (InstructionOK)
  {
    if (!AD7190_Mode(AD7190Modes))
      ESP_LOGI(TAG, "Error programming AD7190 mode register");
  }
  return InstructionOK;
}

bool AD7190_ConfigLoad(struct AD7190Config *AD7190Configs)
{
  bool InstructionOK = false;
  char ReturnStr[10];
  uint8_t FramData[10];

  // Update FRAM
  if ((RXReg[0] == 'A') & (RXReg[1] == 'C') & (RXReg[2] == 'W'))
  {
    strcpy(ReturnStr, "ACWNOK");
    // Mode
    if ((RXReg[3] == 'C') & (RXReg[4] == 'P') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "ACWCP-OK");
        switch (RXReg[6] - 0x30)
        {
        case 0:
          AD7190Configs->Chop = CHOPOFF;
          break;
        case 1:
          AD7190Configs->Chop = CHOPON;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "ACWCP-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Configs->Chop;
          Fram_Update(FramData, ADCHOP, 1, ADCHOPU);
        }
      }
      else
        strcpy(ReturnStr, "ACWCP-NOK");
    }
    else if ((RXReg[3] == 'R') & (RXReg[4] == 'S') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "ACWRS-OK");
        switch (RXReg[6] - 0x30)
        {
        case 1:
          AD7190Configs->Refsel = REFSEL1;
          break;
        case 2:
          AD7190Configs->Refsel = REFSEL2;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "ACWRS-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Configs->Refsel;
          Fram_Update(FramData, ADREFSEL, 1, ADREFSELU);
        }
      }
      else
        strcpy(ReturnStr, "ACWRS-NOK");
    }
    else if ((RXReg[3] == 'C') & (RXReg[4] == 'H') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "ACWCH-OK");
        switch (RXReg[6] - 0x30)
        {
        case 0:
          AD7190Configs->Channel = CHANNEL0;
          break;
        case 1:
          AD7190Configs->Channel = CHANNEL1;
          break;
        case 2:
          AD7190Configs->Channel = CHANNEL2;
          break;
        case 3:
          AD7190Configs->Channel = CHANNEL3;
          break;
        case 4:
          AD7190Configs->Channel = CHANNEL4;
          break;
        case 5:
          AD7190Configs->Channel = CHANNEL5;
          break;
        case 6:
          AD7190Configs->Channel = CHANNEL6;
          break;
        case 7:
          AD7190Configs->Channel = CHANNEL7;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "ACWCH-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Configs->Channel;
          Fram_Update(FramData, ADCHANNEL, 1, ADCHANNELU);
        }
      }
      else
        strcpy(ReturnStr, "ACWCH-NOK");
    }
    else if ((RXReg[3] == 'B') & (RXReg[4] == 'U') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "ACWBU-OK");
        switch (RXReg[6] - 0x30)
        {
        case 0:
          AD7190Configs->Burn = BURNOFF;
          break;
        case 1:
          AD7190Configs->Burn = BURNON;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "ACWBU-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Configs->Burn;
          Fram_Update(FramData, ADBURN, 1, ADBURNU);
        }
      }
      else
        strcpy(ReturnStr, "ACWBU-NOK");
    }
    else if ((RXReg[3] == 'R') & (RXReg[4] == 'D') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "ACWRD-OK");
        switch (RXReg[6] - 0x30)
        {
        case 0:
          AD7190Configs->RefDet = REFDETOFF;
          break;
        case 1:
          AD7190Configs->RefDet = REFDETON;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "ACWRD-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Configs->RefDet;
          Fram_Update(FramData, ADREFDET, 1, ADREFDETU);
        }
      }
      else
        strcpy(ReturnStr, "ACWRD-NOK");
    }
    else if ((RXReg[3] == 'B') & (RXReg[4] == 'F') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "ACWBF-OK");
        switch (RXReg[6] - 0x30)
        {
        case 0:
          AD7190Configs->Buffer = BUFFEROFF;
          break;
        case 1:
          AD7190Configs->Buffer = BUFFERON;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "ACWBF-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Configs->Buffer;
          Fram_Update(FramData, ADBUFFER, 1, ADBUFFERU);
        }
      }
      else
        strcpy(ReturnStr, "ACWBF-NOK");
    }
    else if ((RXReg[3] == 'P') & (RXReg[4] == 'O') & (RXReg[5] == '-'))
    {
      if (RXReg[7] == '#')
      {
        InstructionOK = true;
        strcpy(ReturnStr, "ACWPO-OK");
        switch (RXReg[6] - 0x30)
        {
        case 0:
          AD7190Configs->Polarity = UNIPOLAR;
          break;
        case 1:
          AD7190Configs->Polarity = BIPOLAR;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "ACWPO-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Configs->Polarity;
          Fram_Update(FramData, ADPOLARITY, 1, ADPOLARITYU);
        }
      }
      else
        strcpy(ReturnStr, "ACWPO-NOK");
    }
    else if ((RXReg[3] == 'G') & (RXReg[4] == 'A') & (RXReg[5] == '-'))
    {
      uint8_t temp = 0;
      if (RXReg[7] == '#')
      {
        temp = RXReg[6] - 0x30;
      }
      else if (RXReg[8] == '#')
      {
        temp = ((RXReg[6] - 0x30) * 10) + (RXReg[7] - 0x30);
      }
      else if (RXReg[9] == '#')
      {
        temp = ((RXReg[6] - 0x30) * 100) + ((RXReg[7] - 0x30) * 10) + (RXReg[8] - 0x30);
      }
      if (temp > 0)
      {
        InstructionOK = true;
        strcpy(ReturnStr, "ACWGA-OK");
        switch (temp)
        {
        case 1:
          AD7190Configs->Gain = GAIN1;
          break;
        case 8:
          AD7190Configs->Gain = GAIN8;
          break;
        case 16:
          AD7190Configs->Gain = GAIN16;
          break;
        case 32:
          AD7190Configs->Gain = GAIN32;
          break;
        case 64:
          AD7190Configs->Gain = GAIN64;
          break;
        case 128:
          AD7190Configs->Gain = GAIN128;
          break;
        default:
          InstructionOK = false;
          strcpy(ReturnStr, "ACWGA-NOK");
          break;
        }
        if (InstructionOK)
        {
          FramData[0] = AD7190Configs->Gain;
          Fram_Update(FramData, ADGAIN, 1, ADGAINU);
        }
      }
      else
        strcpy(ReturnStr, "ACWGA-NOK");
    }
    // Send response
    for (uint8_t i = 0; i < strlen(ReturnStr); i++)
      EthData[i + (256 * (EthSocket - 1))] = ReturnStr[i];
    LenTX[EthSocket] = strlen(ReturnStr);
    EthProgress[EthSocket] = 6;
  }
  // Update AD7190
  if (InstructionOK)
  {
    if (!AD7190_Config(AD7190Configs))
      ESP_LOGI(TAG, "Error programming AD7190 config register");
  }
  return InstructionOK;
}

void AD7190_ModeRead(struct AD7190Mode *AD7190Modes)
{
  char ReturnStr[40];

  // Read mode registers
  if ((RXReg[0] == 'A') & (RXReg[1] == 'M') & (RXReg[2] == 'R'))
  {
    // Mode
    strcpy(ReturnStr, "AMRMO: Error");
    if ((RXReg[3] == 'M') & (RXReg[4] == 'O'))
    {
      switch (AD7190Modes->Mode)
      {
      case CONTINOUS:
        strcpy(ReturnStr, "AMRMO: Continous");
        break;
      case SINGLE:
        strcpy(ReturnStr, "AMRMO: Single");
        break;
      case IDLE:
        strcpy(ReturnStr, "AMRMO: Idle");
        break;
      case POWERDOWN:
        strcpy(ReturnStr, "AMRMO: Power Down");
        break;
      default:
        break;
      }
    }
    // Status transmit
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'T'))
    {
      switch (AD7190Modes->StatusTrans)
      {
      case STATUSTRANSOFF:
        strcpy(ReturnStr, "AMRST: Status transmit off");
        break;
      case STATUSTRANSON:
        strcpy(ReturnStr, "AMRST: Status transmit on");
        break;
      default:
        break;
      }
    }
    // Clock
    else if ((RXReg[3] == 'C') & (RXReg[4] == 'L'))
    {
      switch (AD7190Modes->Clock)
      {
      case EXTCRYSTAL:
        strcpy(ReturnStr, "AMRCL: External crystal");
        break;
      case EXTCLOCK:
        strcpy(ReturnStr, "AMRCL: External clock");
        break;
      case INTCLOCKTRI:
        strcpy(ReturnStr, "AMRCL: Internal clock (tristate)");
        break;
      case INTCLOCK:
        strcpy(ReturnStr, "AMRCL: Internal clock");
        break;
      default:
        break;
      }
    }
    // Sinc
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'I'))
    {
      switch (AD7190Modes->Sinc)
      {
      case SINC3:
        strcpy(ReturnStr, "AMRSI: Sinc3");
        break;
      case SINC4:
        strcpy(ReturnStr, "AMRSI: Sinc4");
        break;
      default:
        break;
      }
    }
    // Parity
    else if ((RXReg[3] == 'P') & (RXReg[4] == 'A'))
    {
      switch (AD7190Modes->Parity)
      {
      case PARITYDIS:
        strcpy(ReturnStr, "AMRPA: Parity disabled");
        break;
      case PARITYEN:
        strcpy(ReturnStr, "AMRPA: Parity enabled");
        break;
      default:
        break;
      }
    }
    // Single
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'N'))
    {
      switch (AD7190Modes->Single)
      {
      case SINGLEDIS:
        strcpy(ReturnStr, "AMRSN: Single disabled");
        break;
      case SINGLEEN:
        strcpy(ReturnStr, "AMRSN: Single enabled");
        break;
      default:
        break;
      }
    }
    // REJ
    else if ((RXReg[3] == 'R') & (RXReg[4] == 'E'))
    {
      switch (AD7190Modes->Rej)
      {
      case REJ50:
        strcpy(ReturnStr, "AMRRE: Rej50");
        break;
      case REJ60:
        strcpy(ReturnStr, "AMRRE: Rej60");
        break;
      default:
        break;
      }
    }
    else if ((RXReg[3] == 'R') & (RXReg[4] == 'A'))
    {
      char tempstr[20];
      double d;

      d = AD7190Modes->Rate;
      sprintf(tempstr, "%d.%02u", (int)d, (int)((d - (int)d) * 100));
      strcpy(ReturnStr, "AMRRA: ");
      strcat(ReturnStr, tempstr);
    }
    // Send response
    for (uint8_t i = 0; i < strlen(ReturnStr); i++)
      EthData[i + (256 * (EthSocket - 1))] = ReturnStr[i];
    LenTX[EthSocket] = strlen(ReturnStr);
    EthProgress[EthSocket] = 6;
  }
}

void AD7190_ConfigRead(struct AD7190Config *AD7190Configs)
{
  char ReturnStr[40];

  // Read config registers
  if ((RXReg[0] == 'A') & (RXReg[1] == 'C') & (RXReg[2] == 'R'))
  {
    // Chop
    if ((RXReg[3] == 'C') & (RXReg[4] == 'P'))
    {
      switch (AD7190Configs->Chop)
      {
      case CHOPOFF:
        strcpy(ReturnStr, "ACRCP: Chop off");
        break;
      case CHOPON:
        strcpy(ReturnStr, "ACRCP: Chop on");
        break;
      default:
        break;
      }
    }
    // Reference
    else if ((RXReg[3] == 'R') & (RXReg[4] == 'S'))
    {
      switch (AD7190Configs->Refsel)
      {
      case REFSEL1:
        strcpy(ReturnStr, "ACRRS: Reference 1");
        break;
      case REFSEL2:
        strcpy(ReturnStr, "ACRRS: Reference 2");
        break;
      default:
        break;
      }
    }
    // Channel
    else if ((RXReg[3] == 'C') & (RXReg[4] == 'H'))
    {
      switch (AD7190Configs->Channel)
      {
      case CHANNEL0:
        strcpy(ReturnStr, "ACRCH: Channel0");
        break;
      case CHANNEL1:
        strcpy(ReturnStr, "ACRCH: Channel1");
        break;
      case CHANNEL2:
        strcpy(ReturnStr, "ACRCH: Channel2");
        break;
      case CHANNEL3:
        strcpy(ReturnStr, "ACRCH: Channel3");
        break;
      case CHANNEL4:
        strcpy(ReturnStr, "ACRCH: Channel4");
        break;
      case CHANNEL5:
        strcpy(ReturnStr, "ACRCH: Channel5");
        break;
      case CHANNEL6:
        strcpy(ReturnStr, "ACRCH: Channel6");
        break;
      case CHANNEL7:
        strcpy(ReturnStr, "ACRCH: Channel7");
        break;
      default:
        break;
      }
    }
    // Burn
    else if ((RXReg[3] == 'B') & (RXReg[4] == 'U'))
    {
      switch (AD7190Configs->Burn)
      {
      case BURNOFF:
        strcpy(ReturnStr, "ACRBU: Burn off");
        break;
      case BURNON:
        strcpy(ReturnStr, "ACRBU: Burn on");
        break;
      default:
        break;
      }
    }
    // Reference detect
    else if ((RXReg[3] == 'R') & (RXReg[4] == 'D'))
    {
      switch (AD7190Configs->RefDet)
      {
      case REFDETOFF:
        strcpy(ReturnStr, "ACRRD: Reference detect off");
        break;
      case REFDETON:
        strcpy(ReturnStr, "ACRRD: Reference detect on");
        break;
      default:
        break;
      }
    }
    // Buffer
    else if ((RXReg[3] == 'B') & (RXReg[4] == 'F'))
    {
      switch (AD7190Configs->Buffer)
      {
      case BUFFEROFF:
        strcpy(ReturnStr, "ACRBF: Buffer off");
        break;
      case BUFFERON:
        strcpy(ReturnStr, "ACRBF: Buffer on");
        break;
      default:
        break;
      }
    }
    // Polarity
    else if ((RXReg[3] == 'P') & (RXReg[4] == 'O'))
    {
      switch (AD7190Configs->Polarity)
      {
      case UNIPOLAR:
        strcpy(ReturnStr, "ACRPO: Unipolar");
        break;
      case BIPOLAR:
        strcpy(ReturnStr, "ACRPO: Bipolar");
        break;
      default:
        break;
      }
    }
    else if ((RXReg[3] == 'G') & (RXReg[4] == 'A'))
    {
      switch (AD7190Configs->Gain)
      {
      case GAIN1:
        strcpy(ReturnStr, "ACRGA: Gain1");
        break;
      case GAIN8:
        strcpy(ReturnStr, "ACRGA: Gain8");
        break;
      case GAIN16:
        strcpy(ReturnStr, "ACRGA: Gain16");
        break;
      case GAIN32:
        strcpy(ReturnStr, "ACRGA: Gain32");
        break;
      case GAIN64:
        strcpy(ReturnStr, "ACRGA: Gain64");
        break;
      case GAIN128:
        strcpy(ReturnStr, "ACRGA: Gain128");
        break;
      default:
        break;
      }
    }
    // Send response
    for (uint8_t i = 0; i < strlen(ReturnStr); i++)
      EthData[i + (256 * (EthSocket - 1))] = ReturnStr[i];
    LenTX[EthSocket] = strlen(ReturnStr);
    EthProgress[EthSocket] = 6;
  }
}

bool AD7190_ScaleSet(struct ScaleValues *ScaleValue, bool *ZeroCalibration, bool *FullCalibration)
{
  bool InstructionOK = false;
  char ReturnStr[10];
  uint8_t FramData[10];

  if ((RXReg[0] == 'A') & (RXReg[1] == 'S') & (RXReg[2] == 'W'))
  {
    strcpy(ReturnStr, "ASWNOK");
    // Zero calibration
    if ((RXReg[3] == 'Z') & (RXReg[4] == 'C'))
    {
      if ((RXReg[5] == '#') & (ScaleValue->CalibCount > 0))
      {
        strcpy(ReturnStr, "ASWZC-OK");
        *ZeroCalibration = true;
      }
      else
        strcpy(ReturnStr, "ASWZC-NOK");
    }
    // Full calibration
    else if ((RXReg[3] == 'F') & (RXReg[4] == 'C'))
    {
      if ((RXReg[5] == '#') & (*ZeroCalibration == false) & (ScaleValue->CalibCount > 0) & (ScaleValue->CalibWeight > 0))
      {
        strcpy(ReturnStr, "ASWFC-OK");
        *FullCalibration = true;
      }
      else
        strcpy(ReturnStr, "ASWFC-NOK");
    }
    // Software start weighing
    if ((RXReg[3] == 'S') & (RXReg[4] == 'S') & (RXReg[5] == '-'))
    {
      if ((RXReg[7] == '#') & (ScaleValue->StartTrigger == 4))
      {
        if (RXReg[6] == '1')
          strcpy(ReturnStr, "ASWSS-OK");
        else
          strcpy(ReturnStr, "");
        StartWeighing = true;
      }
      else
        strcpy(ReturnStr, "ASWSS-NOK");
    }
    // Software stop weighing
    if ((RXReg[3] == 'S') & (RXReg[4] == 'P') & (RXReg[5] == '-'))
    {
      if ((RXReg[7] == '#') & (ScaleValue->StopTrigger == 6))
      {
        if (RXReg[6] == '1')
          strcpy(ReturnStr, "ASWSP-OK");
        else
          strcpy(ReturnStr, "");
        StopWeighing = true;
      }
      else
        strcpy(ReturnStr, "ASWSP-NOK");
    }
    // Calibration count
    else if ((RXReg[3] == 'C') & (RXReg[4] == 'C') & (RXReg[5] == '-'))
    {
      char str[11];
      uint8_t i = 0, temp;

      // Find # and replace with 0 (to turn into a string)
      while ((RXReg[6 + i] != '#') & (i < 4))
      {
        str[i] = RXReg[6 + i];
        i++;
      }
      if ((RXReg[6 + i] == '#') & (i < 4))
      {
        str[i] = 0;
        temp = (uint8_t)atoi(str);
        if (temp == 0)
          strcpy(ReturnStr, "ASWCC-NOK");
        else
        {
          InstructionOK = true;
          ScaleValue->CalibCount = temp;
          strcpy(ReturnStr, "ASWCC-OK");
          FramData[0] = ScaleValue->CalibCount;
          Fram_Update(FramData, CALIBCOUNT, CALIBCOUNTL, CALIBCOUNTU);
        }
      }
      else
        strcpy(ReturnStr, "ASWCC-NOK");
    }
    // Zero target
    else if ((RXReg[3] == 'Z') & (RXReg[4] == 'T') & (RXReg[5] == '-'))
    {
      char str[11];
      uint8_t i = 0, temp;

      // Find # and replace with 0 (to turn into a string)
      while ((RXReg[6 + i] != '#') & (i < 4))
      {
        str[i] = RXReg[6 + i];
        i++;
      }
      if ((RXReg[6 + i] == '#') & (i < 4))
      {
        str[i] = 0;
        temp = (uint8_t)atoi(str);
        if (temp == 0)
          strcpy(ReturnStr, "ASWZT-NOK");
        else
        {
          InstructionOK = true;
          ScaleValue->ZeroTarget = temp;
          strcpy(ReturnStr, "ASWZT-OK");
          FramData[0] = ScaleValue->ZeroTarget;
          Fram_Update(FramData, ZEROTARGET, ZEROTARGETL, ZEROTARGETU);
        }
      }
      else
        strcpy(ReturnStr, "ASWZT-NOK");
    }
    // Weight variance
    else if ((RXReg[3] == 'W') & (RXReg[4] == 'V') & (RXReg[5] == '-'))
    {
      char str[11];
      uint8_t i = 0, temp;

      // Find # and replace with 0 (to turn into a string)
      while ((RXReg[6 + i] != '#') & (i < 4))
      {
        str[i] = RXReg[6 + i];
        i++;
      }
      if ((RXReg[6 + i] == '#') & (i < 4))
      {
        str[i] = 0;
        temp = (uint8_t)atoi(str);
        if (temp == 0)
          strcpy(ReturnStr, "ASWWV-NOK");
        else
        {
          InstructionOK = true;
          ScaleValue->WeightVar = temp;
          strcpy(ReturnStr, "ASWWV-OK");
          FramData[0] = ScaleValue->WeightVar;
          Fram_Update(FramData, WEIGHTVAR, WEIGHTVARL, WEIGHTVARU);
        }
      }
      else
        strcpy(ReturnStr, "ASWWV-NOK");
    }
    // Weight count
    else if ((RXReg[3] == 'W') & (RXReg[4] == 'C') & (RXReg[5] == '-'))
    {
      char str[11];
      uint8_t i = 0, temp;

      // Find # and replace with 0 (to turn into a string)
      while ((RXReg[6 + i] != '#') & (i < 3))
      {
        str[i] = RXReg[6 + i];
        i++;
      }
      if ((RXReg[6 + i] == '#') & (i < 3))
      {
        str[i] = 0;
        temp = (uint8_t)atoi(str);
        if ((temp < 2) | (temp > 99))
          strcpy(ReturnStr, "ASWWC-NOK");
        else
        {
          InstructionOK = true;
          ScaleValue->WeightCount = temp;
          strcpy(ReturnStr, "ASWWC-OK");
          FramData[0] = ScaleValue->WeightCount;
          Fram_Update(FramData, WEIGHTCOUNT, WEIGHTCOUNTL, WEIGHTCOUNTU);
        }
      }
      else
        strcpy(ReturnStr, "ASWWV-NOK");
    }
    // Zero type
    else if ((RXReg[3] == 'Z') & (RXReg[4] == 'Y') & (RXReg[5] == '-'))
    {
      if ((RXReg[7] == '#') & ((RXReg[6] - 0x30) < 4))
      {
        InstructionOK = true;
        ScaleValue->ZeroType = RXReg[6] - 0x30;
        strcpy(ReturnStr, "ASWZY-OK");
        FramData[0] = ScaleValue->ZeroType;
        Fram_Update(FramData, ZEROTYPE, ZEROTYPEL, ZEROTYPEU);
      }
      else
        strcpy(ReturnStr, "ASWZY-NOK");
    }
    // Calibration weight
    else if ((RXReg[3] == 'C') & (RXReg[4] == 'W') & (RXReg[5] == '-'))
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
        InstructionOK = true;
        str[i] = 0;
        temp32 = (uint32_t)atoi(str);
        ScaleValue->CalibWeight = temp32;
        strcpy(ReturnStr, "ASWCW-OK");
        memcpy(FramData, &ScaleValue->CalibWeight, 4);
        Fram_Update(FramData, CALIBWEIGHT, CALIBWEIGHTL, CALIBWEIGHTU);
      }
      else
        strcpy(ReturnStr, "ASWCW-NOK");
    }
    // Start trigger
    else if ((RXReg[3] == 'T') & (RXReg[4] == 'S') & (RXReg[5] == '-'))
    {
      if ((RXReg[7] == '#') & ((RXReg[6] - 0x30) < 5))
      {
        InstructionOK = true;
        ScaleValue->StartTrigger = RXReg[6] - 0x30;
        strcpy(ReturnStr, "ASWTS-OK");
        FramData[0] = ScaleValue->StartTrigger;
        Fram_Update(FramData, STARTTRIGGER, STARTTRIGGERL, STARTTRIGGERU);
      }
      else
        strcpy(ReturnStr, "ASWTS-NOK");
    }
    // Stop trigger
    else if ((RXReg[3] == 'T') & (RXReg[4] == 'P') & (RXReg[5] == '-'))
    {
      if ((RXReg[7] == '#') & ((RXReg[6] - 0x30) < 7))
      {
        InstructionOK = true;
        ScaleValue->StopTrigger = RXReg[6] - 0x30;
        strcpy(ReturnStr, "ASWTP-OK");
        FramData[0] = ScaleValue->StopTrigger;
        Fram_Update(FramData, STOPTRIGGER, STOPTRIGGERL, STOPTRIGGERU);
      }
      else
        strcpy(ReturnStr, "ASWTP-NOK");
    }
    // Transmission on zero
    else if ((RXReg[3] == 'T') & (RXReg[4] == 'Z') & (RXReg[5] == '-'))
    {
      if ((RXReg[7] == '#') & ((RXReg[6] - 0x30) < 2))
      {
        InstructionOK = true;
        ScaleValue->TransZero = RXReg[6] - 0x30;
        strcpy(ReturnStr, "ASWTZ-OK");
        FramData[0] = ScaleValue->TransZero;
        Fram_Update(FramData, TRANSZERO, TRANSZEROL, TRANSZEROU);
      }
      else
        strcpy(ReturnStr, "ASWTZ-NOK");
    }
    // Transmission after reading
    else if ((RXReg[3] == 'T') & (RXReg[4] == 'R') & (RXReg[5] == '-'))
    {
      char str[11];
      uint8_t i = 0, temp;

      // Find # and replace with 0 (to turn into a string)
      while ((RXReg[6 + i] != '#') & (i < 4))
      {
        str[i] = RXReg[6 + i];
        i++;
      }
      if ((RXReg[6 + i] == '#') & (i < 4))
      {
        str[i] = 0;
        temp = (uint8_t)atoi(str);
        if ((temp == 0) & (str[0] != '0'))
          strcpy(ReturnStr, "ASWTR-NOK");
        else
        {
          InstructionOK = true;
          ScaleValue->TransRead = temp;
          strcpy(ReturnStr, "ASWTR-OK");
          FramData[0] = ScaleValue->TransRead;
          Fram_Update(FramData, TRANSREAD, TRANSREADL, TRANSREADU);
        }
      }
      else
        strcpy(ReturnStr, "ASWTR-NOK");
    }
    // Stop time
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'T') & (RXReg[5] == '-'))
    {
      char str[11];
      uint8_t i = 0;
      uint16_t temp16;

      // Find # and replace with 0 (to turn into a string)
      while ((RXReg[6 + i] != '#') & (i < 6))
      {
        str[i] = RXReg[6 + i];
        i++;
      }
      if ((RXReg[6 + i] == '#') & (i < 6))
      {
        InstructionOK = true;
        str[i] = 0;
        temp16 = (uint16_t)atoi(str);
        if (temp16 > 60000)
          strcpy(ReturnStr, "ASWST-NOK");
        else
        {
          ScaleValue->StopTime = temp16;
          strcpy(ReturnStr, "ASWST-OK");
          memcpy(FramData, &ScaleValue->StopTime, STOPTIMEL);
          Fram_Update(FramData, STOPTIME, STOPTIMEL, STOPTIMEU);
        }
      }
      else
        strcpy(ReturnStr, "ASWST-NOK");
    }
    // Skip
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'K') & (RXReg[5] == '-'))
    {
      if ((RXReg[7] == '#') & ((RXReg[6] - 0x30) < 2))
      {
        InstructionOK = true;
        ScaleValue->Skip = RXReg[6] - 0x30;
        strcpy(ReturnStr, "ASWSK-OK");
        FramData[0] = ScaleValue->Skip;
        Fram_Update(FramData, SKIP, SKIPL, SKIPU);
      }
      else
        strcpy(ReturnStr, "ASWSK-NOK");
    }
    // Prefix
    else if ((RXReg[3] == 'P') & (RXReg[4] == 'R') & (RXReg[5] == '-'))
    {
      char str[11];
      uint8_t i = 0, temp;
      // Find # and replace with 0 (to turn into a string)
      while ((RXReg[6 + i] != '#') & (i < 9))
      {
        if (RXReg[6 + i] > 126) // Only allow ascii characters from 0 to 126
          i = 10;
        else
        {
          str[i] = RXReg[6 + i];
          i++;
        }
      }
      if ((RXReg[6 + i] == '#') & (i < 9))
      {
        str[i] = 0;
        strcpy(ScaleValue->Prefix, str);
        strcpy(ReturnStr, "ASWPR-OK");
        memcpy(FramData, str, 9);
        Fram_Update(FramData, PREFIX, PREFIXL, PREFIXU);
      }
      else
        strcpy(ReturnStr, "ASWPR-NOK");
    }
    // Suffix
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'U') & (RXReg[5] == '-'))
    {
      char str[11];
      uint8_t i = 0, temp;
      // Find # and replace with 0 (to turn into a string)
      while ((RXReg[6 + i] != '#') & (i < 9))
      {
        if (RXReg[6 + i] > 126) // Only allow ascii characters from 0 to 126
          i = 10;
        else
        {
          str[i] = RXReg[6 + i];
          i++;
        }
      }
      if ((RXReg[6 + i] == '#') & (i < 9))
      {
        str[i] = 0;
        strcpy(ScaleValue->Suffix, str);
        strcpy(ReturnStr, "ASWSU-OK");
        memcpy(FramData, str, 9);
        Fram_Update(FramData, SUFFIX, SUFFIXL, SUFFIXU);
      }
      else
        strcpy(ReturnStr, "ASWSU-NOK");
    }
    // Send ethernet response
    for (uint8_t i = 0; i < strlen(ReturnStr); i++)
      EthData[i + (256 * (EthSocket - 1))] = ReturnStr[i];
    LenTX[EthSocket] = strlen(ReturnStr);
    EthProgress[EthSocket] = 6;
  }
  return (InstructionOK | *ZeroCalibration | *FullCalibration);
}

void AD7190_ScaleLoad(struct ScaleValues *ScaleValue)
{
  uint8_t ADData[10];

  // Calibration count
  if (Fram_Loc_Updated(CALIBCOUNTU))
  {
    Fram_Read(CALIBCOUNT, ADData, CALIBCOUNTL);
    ScaleValue->CalibCount = ADData[0];
    ESP_LOGI(TAG, "Calibration count from FRAM: %2X", ScaleValue->CalibCount);
  }
  else
    ScaleValue->CalibCount = 10;
  // Zero target
  if (Fram_Loc_Updated(ZEROTARGETU))
  {
    Fram_Read(ZEROTARGET, ADData, ZEROTARGETL);
    ScaleValue->ZeroTarget = ADData[0];
    ESP_LOGI(TAG, "Zero target from FRAM: %2X", ScaleValue->ZeroTarget);
  }
  else
    ScaleValue->ZeroTarget = 50;
  // Weight variance
  if (Fram_Loc_Updated(WEIGHTVARU))
  {
    Fram_Read(WEIGHTVAR, ADData, WEIGHTVARL);
    ScaleValue->WeightVar = ADData[0];
    ESP_LOGI(TAG, "Weight variance from FRAM: %2X", ScaleValue->WeightVar);
  }
  else
    ScaleValue->WeightVar = 2;
  // Weight count
  if (Fram_Loc_Updated(WEIGHTCOUNTU))
  {
    Fram_Read(WEIGHTCOUNT, ADData, WEIGHTCOUNTL);
    ScaleValue->WeightCount = ADData[0];
    ESP_LOGI(TAG, "Weight variance from FRAM: %2X", ScaleValue->WeightCount);
  }
  else
    ScaleValue->WeightCount = 2;
  // Zero Type
  if (Fram_Loc_Updated(ZEROTYPEU))
  {
    Fram_Read(ZEROTYPE, ADData, ZEROTYPEL);
    ScaleValue->ZeroType = ADData[0];
    ESP_LOGI(TAG, "Zero type from FRAM: %d", ScaleValue->ZeroType);
  }
  else
    ScaleValue->ZeroType = 1;
  // Zero calibration amount
  if (Fram_Loc_Updated(CALIBZEROU))
  {
    Fram_Read(CALIBZERO, ADData, CALIBZEROL);
    memcpy(&ScaleValue->CalibZero, ADData, 4);
    ESP_LOGI(TAG, "Zero calibration amount from FRAM: %d", ScaleValue->CalibZero);
  }
  else
    ScaleValue->CalibZero = 0;
  // Full calibration amount
  if (Fram_Loc_Updated(CALIBFULLU))
  {
    Fram_Read(CALIBFULL, ADData, CALIBFULLL);
    memcpy(&ScaleValue->CalibFull, ADData, 4);
    ESP_LOGI(TAG, "Full calibration amount from FRAM: %d", ScaleValue->CalibFull);
  }
  else
    ScaleValue->CalibFull = 0;
  // Calibration weight in grams
  if (Fram_Loc_Updated(CALIBWEIGHTU))
  {
    Fram_Read(CALIBWEIGHT, ADData, CALIBWEIGHTL);
    memcpy(&ScaleValue->CalibWeight, ADData, 4);
    ESP_LOGI(TAG, "Calibration weight from FRAM: %d", ScaleValue->CalibWeight);
  }
  else
    ScaleValue->CalibWeight = 800;
  // Calibration ratio
  if (Fram_Loc_Updated(CALIBRATIOU))
  {
    Fram_Read(CALIBRATIO, ADData, CALIBRATIOL);
    memcpy(&ScaleValue->CalibRatio, ADData, 4);
    ESP_LOGI(TAG, "Calibration ratio from FRAM: %4.2f", ScaleValue->CalibRatio);
  }
  else
    ScaleValue->CalibRatio = 0;
  // Start trigger
  if (Fram_Loc_Updated(STARTTRIGGERU))
  {
    Fram_Read(STARTTRIGGER, ADData, STARTTRIGGERL);
    memcpy(&ScaleValue->StartTrigger, ADData, 1);
    ESP_LOGI(TAG, "Start trigger from FRAM: %2x", ScaleValue->StartTrigger);
  }
  else
    ScaleValue->StartTrigger = 0;
  // Stop trigger
  if (Fram_Loc_Updated(STOPTRIGGERU))
  {
    Fram_Read(STOPTRIGGER, ADData, STOPTRIGGERL);
    memcpy(&ScaleValue->StopTrigger, ADData, 1);
    ESP_LOGI(TAG, "Stop trigger from FRAM: %2x", ScaleValue->StopTrigger);
  }
  else
    ScaleValue->StopTrigger = 0;
  // Transmission on zero
  if (Fram_Loc_Updated(TRANSZEROU))
  {
    Fram_Read(TRANSZERO, ADData, TRANSZEROL);
    memcpy(&ScaleValue->TransZero, ADData, 1);
    ESP_LOGI(TAG, "Transmission on zero from FRAM: %2x", ScaleValue->TransZero);
  }
  else
    ScaleValue->TransZero = 1;
  // Transmission after reading
  if (Fram_Loc_Updated(TRANSREADU))
  {
    Fram_Read(TRANSREAD, ADData, TRANSREADL);
    memcpy(&ScaleValue->TransRead, ADData, 1);
    ESP_LOGI(TAG, "Transmission after reading from FRAM: %2x", ScaleValue->TransRead);
  }
  else
    ScaleValue->TransRead = 0;
  // Stop time
  if (Fram_Loc_Updated(STOPTIMEU))
  {
    Fram_Read(STOPTIME, ADData, STOPTIMEL);
    memcpy(&ScaleValue->StopTime, ADData, 1);
    ESP_LOGI(TAG, "Transmission after reading from FRAM: %2x", ScaleValue->StopTime);
  }
  else
    ScaleValue->StopTime = 0;
  // Skip
  if (Fram_Loc_Updated(SKIPU))
  {
    Fram_Read(SKIP, ADData, SKIPL);
    memcpy(&ScaleValue->Skip, ADData, 1);
    ESP_LOGI(TAG, "Skip from FRAM: %2x", ScaleValue->Skip);
  }
  else
    ScaleValue->Skip = 0;
  // Prefix
  if (Fram_Loc_Updated(PREFIXU))
  {
    Fram_Read(PREFIX, ADData, PREFIXL);
    memcpy(&ScaleValue->Prefix, ADData, 9);
    ESP_LOGI(TAG, "Prefix from FRAM: %s", ScaleValue->Prefix);
  }
  else
    strcpy(ScaleValue->Prefix, "");
  // Suffix
  if (Fram_Loc_Updated(SUFFIXU))
  {
    Fram_Read(SUFFIX, ADData, SUFFIXL);
    memcpy(&ScaleValue->Suffix, ADData, 9);
    ESP_LOGI(TAG, "Suffix from FRAM: %s", ScaleValue->Suffix);
  }
  else
    strcpy(ScaleValue->Suffix, "");
}

void AD7190_ScaleRead(struct ScaleValues *ScaleValue)
{
  char ReturnStr[40];

  if ((RXReg[0] == 'A') & (RXReg[1] == 'S') & (RXReg[2] == 'R'))
  {
    strcpy(ReturnStr, "ASRNOK");
    // Calibration count
    if ((RXReg[3] == 'C') & (RXReg[4] == 'C'))
    {
      char str[11];
      IntToString(str, ScaleValue->CalibCount);
      strcpy(ReturnStr, "ASRCC: ");
      strcat(ReturnStr, str);
      strcat(ReturnStr, " calibration counts");
    }
    // Zero target
    else if ((RXReg[3] == 'Z') & (RXReg[4] == 'T'))
    {
      char str[11];
      IntToString(str, ScaleValue->ZeroTarget);
      strcpy(ReturnStr, "ASRZT: ");
      strcat(ReturnStr, str);
      strcat(ReturnStr, "g zero target");
    }
    // Weight variance
    else if ((RXReg[3] == 'W') & (RXReg[4] == 'V'))
    {
      char str[11];
      IntToString(str, ScaleValue->WeightVar);
      strcpy(ReturnStr, "ASRWV: ");
      strcat(ReturnStr, str);
      strcat(ReturnStr, "g weight variance");
    }
    // Weight count
    else if ((RXReg[3] == 'W') & (RXReg[4] == 'C'))
    {
      char str[11];
      IntToString(str, ScaleValue->WeightCount);
      strcpy(ReturnStr, "ASRWC: ");
      strcat(ReturnStr, str);
      strcat(ReturnStr, " weight count");
    }
    // Zero type
    else if ((RXReg[3] == 'Z') & (RXReg[4] == 'Y'))
    {
      switch (ScaleValue->ZeroType)
      {
      case 0:
        strcpy(ReturnStr, "ASRZY: No Zero");
        break;
      case 1:
        strcpy(ReturnStr, "ASRZY: AutoZero");
        break;
      case 2:
        strcpy(ReturnStr, "ASRZY: Zero when Input3 goes low");
        break;
      case 3:
        strcpy(ReturnStr, "ASRZY: Zero when Input3 goes high");
        break;
      default:
        strcpy(ReturnStr, "ASRZY: ASRZY-NOK");
        break;
      }
    }
    // Calibration weight
    else if ((RXReg[3] == 'C') & (RXReg[4] == 'W'))
    {
      char str[11];
      IntToString(str, ScaleValue->CalibWeight);
      strcpy(ReturnStr, "ASRCW: ");
      strcat(ReturnStr, str);
      strcat(ReturnStr, "g calibration weight");
    }
    // Start trigger
    else if ((RXReg[3] == 'T') & (RXReg[4] == 'S'))
    {
      switch (ScaleValue->StartTrigger)
      {
      case 0:
        strcpy(ReturnStr, "ASRTS: Freeflow");
        break;
      case 1:
        strcpy(ReturnStr, "ASRTS: Weight On");
        break;
      case 2:
        strcpy(ReturnStr, "ASRTS: Input1 goes high");
        break;
      case 3:
        strcpy(ReturnStr, "ASRTS: Input1 goes low");
        break;
      case 4:
        strcpy(ReturnStr, "ASRTS: Software");
        break;
      default:
        strcpy(ReturnStr, "ASRTS-NOK");
        break;
      }
    }
    // Stop trigger
    else if ((RXReg[3] == 'T') & (RXReg[4] == 'P'))
    {
      if (ScaleValue->StartTrigger == 0)
        strcpy(ReturnStr, "ASRTP: Freeflow");
      else
      {
        switch (ScaleValue->StopTrigger)
        {
        case 0:
          strcpy(ReturnStr, "ASRTP: Weight Stable");
          break;
        case 1:
          strcpy(ReturnStr, "ASRTP: Time");
          break;
        case 2:
          strcpy(ReturnStr, "ASRTP: Input1 goes low");
          break;
        case 3:
          strcpy(ReturnStr, "ASRTP: Input1 goes high");
          break;
        case 4:
          strcpy(ReturnStr, "ASRTP: Input2 goes low");
          break;
        case 5:
          strcpy(ReturnStr, "ASRTP: Input2 goes high");
          break;
        case 6:
          strcpy(ReturnStr, "ASRTP: Software");
          break;
        default:
          strcpy(ReturnStr, "ASRTP-NOK");
          break;
        }
      }
    }
    // Transmission on zero
    else if ((RXReg[3] == 'T') & (RXReg[4] == 'Z'))
    {
      switch (ScaleValue->TransZero)
      {
      case 0:
        strcpy(ReturnStr, "ASRTZ: Transmit nothing when zero");
        break;
      case 1:
        strcpy(ReturnStr, "ASRTZ: Freeflow when zero");
        break;
      default:
        strcpy(ReturnStr, "ASRTZ-NOK");
        break;
      }
    }
    // Transmission after reading
    else if ((RXReg[3] == 'T') & (RXReg[4] == 'R'))
    {
      char str[11];
      if (ScaleValue->TransRead == 0)
        strcpy(ReturnStr, "ASRTR: Continous when weight on");
      else
      {
        IntToString(str, ScaleValue->TransRead);
        strcpy(ReturnStr, "ASRTW: ");
        strcat(ReturnStr, str);
        strcat(ReturnStr, " times when weight on");
      }
    }
    // Stop time
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'T'))
    {
      char str[11];
      IntToString(str, ScaleValue->StopTime);
      strcpy(ReturnStr, "ASRST: ");
      strcat(ReturnStr, str);
      strcat(ReturnStr, "mS after start trigger");
    }
    // Skip
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'K'))
    {
      switch (ScaleValue->Skip)
      {
      case 0:
        strcpy(ReturnStr, "ASRSK: Don't skip first reading");
        break;
      case 1:
        strcpy(ReturnStr, "ASRSK: Skip first reading");
        break;
      default:
        strcpy(ReturnStr, "ASRSK-NOK");
        break;
      }
    }
    // Prefix
    else if ((RXReg[3] == 'P') & (RXReg[4] == 'R'))
    {
      char str[11];
      strcpy(str, ScaleValue->Prefix);
      strcpy(ReturnStr, "ASRPR: ");
      strcat(ReturnStr, str);
    }
    // Suffix
    else if ((RXReg[3] == 'S') & (RXReg[4] == 'U'))
    {
      char str[11];
      strcpy(str, ScaleValue->Suffix);
      strcpy(ReturnStr, "ASRSU: ");
      strcat(ReturnStr, str);
    }
    // Send response
    for (uint8_t i = 0; i < strlen(ReturnStr); i++)
      EthData[i + (256 * (EthSocket - 1))] = ReturnStr[i];
    LenTX[EthSocket] = strlen(ReturnStr);
    EthProgress[EthSocket] = 6;
  }
}

void AD7190_ScaleZero(struct ScaleValues *ScaleValue, uint32_t ReadWeight, bool *ZeroCalibration)
{
  char ReturnStr[30];
  char tempstr[30];
  uint8_t FramData[4];
  static uint8_t CalibCounter = 0;
  static uint64_t CalibTotal = 0;

  CalibTotal = CalibTotal + ReadWeight;
  CalibCounter++;
  ESP_LOGI(TAG, "ReadWeight: %d, CalibCounter: %d, CalibCount: %d", ReadWeight, CalibCounter, ScaleValue->CalibCount);
  if (CalibCounter >= ScaleValue->CalibCount)
  {

    ScaleValue->CalibZero = (uint32_t)(CalibTotal / CalibCounter);
    CalibCounter = 0;
    CalibTotal = 0;
    *ZeroCalibration = false;
    ESP_LOGI(TAG, "Zero calibration value: %d", ScaleValue->CalibZero);
    // Store in FRAM
    memcpy(FramData, &ScaleValue->CalibZero, 4);
    Fram_Update(FramData, CALIBZERO, CALIBZEROL, CALIBZEROU);
    // Send ethernet response
    strcpy(ReturnStr, "ZC: ");
    IntToString(tempstr, ScaleValue->CalibZero);
    strcat(ReturnStr, tempstr);
    for (uint8_t i = 0; i < strlen(ReturnStr); i++)
      EthData[i + (256 * (EthSocket - 1))] = ReturnStr[i];
    LenTX[EthSocket] = strlen(ReturnStr);
    EthProgress[EthSocket] = 6;
  }
}

void AD7190_ScaleFull(struct ScaleValues *ScaleValue, uint32_t ReadWeight, bool *FullCalibration)
{
  char ReturnStr[30];
  char tempstr[30];
  uint8_t FramData[4];
  static uint8_t CalibCounter = 0;
  static uint64_t CalibTotal = 0;

  CalibTotal = CalibTotal + ReadWeight;
  CalibCounter++;
  ESP_LOGI(TAG, "ReadWeight: %d, CalibCounter: %d, CalibCount: %d", ReadWeight, CalibCounter, ScaleValue->CalibCount);
  if (CalibCounter >= ScaleValue->CalibCount)
  {

    ScaleValue->CalibFull = (uint32_t)(CalibTotal / CalibCounter);
    CalibCounter = 0;
    CalibTotal = 0;
    *FullCalibration = false;
    ESP_LOGI(TAG, "Full calibration value: %d", ScaleValue->CalibFull);
    ScaleValue->CalibRatio = (float)ScaleValue->CalibWeight / (float)(ScaleValue->CalibFull - ScaleValue->CalibZero);
    // Store in FRAM
    memcpy(FramData, &ScaleValue->CalibFull, 4);
    Fram_Update(FramData, CALIBFULL, CALIBFULLL, CALIBFULLU);
    memcpy(FramData, &ScaleValue->CalibRatio, 4);
    Fram_Update(FramData, CALIBRATIO, CALIBRATIOL, CALIBRATIOU);
    // Send ethernet response
    strcpy(ReturnStr, "FC: ");
    IntToString(tempstr, ScaleValue->CalibFull);
    strcat(ReturnStr, tempstr);
    for (uint8_t i = 0; i < strlen(ReturnStr); i++)
      EthData[i + (256 * (EthSocket - 1))] = ReturnStr[i];
    LenTX[EthSocket] = strlen(ReturnStr);
    EthProgress[EthSocket] = 6;
  }
}

float AD7190_Zero(float Weight, uint8_t Count, uint8_t *Indicator)
{
  static uint8_t Counter = 0;
  static float Total = 0.0;
  static float ReturnValue = 0.0;
  static float CurrentValue = 0.0;
  static float LastValue = 0.0;

  Total = Total + Weight;
  Counter++;
  if (Counter >= Count)
  {
    LastValue = CurrentValue;
    CurrentValue = Total / Counter;
    Counter = 0;
    Total = 0.0;
    if (fabs(CurrentValue - LastValue) < 0.5)
    {
      ReturnValue = CurrentValue;
      Pin_Low(PLEDB);
      *Indicator = 0;
    }
  }
  return (ReturnValue);
}

void Send_Weight(char *str)
{
  for (uint8_t i = 0; i < strlen(str); i++)
    EthData[i + (256 * (EthSocket - 1))] = str[i];
  LenTX[EthSocket] = strlen(str);
  EthProgress[EthSocket] = 6;
}
