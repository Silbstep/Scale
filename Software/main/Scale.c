#include <stdio.h>
#include <math.h>
#include "Global.h"
#include "InOut.h"
#include "AD5621.h"
#include "AD7190.h"
#include "FRAM.h"
#include "RS485.h"
#include <driver/uart.h>
#include "Ethernet.h"
#include "EthSPI.h"
#include "Structures.h"
#include "Conversion.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "esp_log.h"

// Global variables
bool StartWeighing = false;
bool StopWeighing = false;
uint8_t Indicator = 0;
uint16_t StopCounter = 0;
// Ethernet
bool EthRead = false; // If true then ethernet parameters have been read
bool EthReceived = false;
bool TransmitNow = false;
uint8_t EthSocket = 1;
uint8_t EthTimeOut; // Timout for Ethernet
uint8_t Inputs, InputsHL, InputsLH;
uint8_t NoCommsTimeout = 0;
uint8_t EthBuffer[10];      // Data to be written to and read from ethernet
uint8_t EthData[ETHBUFFER]; // Data to be read and written
uint8_t EthProgress[9] = {0};
uint8_t RXReg[256];
uint16_t LenRX[9];  // Length of received packet
uint16_t SourcePtr; // Points to received data
uint16_t LenTX[9];  // Length of transmitted packet
uint16_t DestPtr;
spi_device_handle_t spi; // SPI for Ethernet

static const char *TAG = "Scale";

// Check Ethernet every 10mS.
void EthernetTask(void *p)
{
  uint8_t Socket1Opened = 0;
  uint8_t Socket2Opened = 0;
  uint8_t RXLen = 0;
  uint8_t EthParam[48];
  uint16_t EthernetReset = 0;
  struct EthernetValues EthernetValue;

  // Ethernet
  for (;;)
  {
    if (!EthRead)
    {
      // Get Ethernet parameters from FRAM
      if (Fram_Loc_Updated(ETHSTRUCTU))
      {
        Fram_Read(ETHSTRUCT, EthParam, ETHSTRUCTL);
        EthernetValue.SelfIp[0] = EthParam[0];
        EthernetValue.SelfIp[1] = EthParam[1];
        EthernetValue.SelfIp[2] = EthParam[2];
        EthernetValue.SelfIp[3] = EthParam[3];
        EthernetValue.SelfSubnet[0] = EthParam[4];
        EthernetValue.SelfSubnet[1] = EthParam[5];
        EthernetValue.SelfSubnet[2] = EthParam[6];
        EthernetValue.SelfSubnet[3] = EthParam[7];
        EthernetValue.SelfGateway[0] = EthParam[8];
        EthernetValue.SelfGateway[1] = EthParam[9];
        EthernetValue.SelfGateway[2] = EthParam[10];
        EthernetValue.SelfGateway[3] = EthParam[11];
        EthernetValue.DestinationIp[0] = EthParam[12];
        EthernetValue.DestinationIp[1] = EthParam[13];
        EthernetValue.DestinationIp[2] = EthParam[14];
        EthernetValue.DestinationIp[3] = EthParam[15];
        EthernetValue.SelfPort[0] = EthParam[16];
        EthernetValue.SelfPort[1] = EthParam[17];
        EthernetValue.DestPort[EthSocket - 1][0] = EthParam[18];
        EthernetValue.DestPort[EthSocket - 1][1] = EthParam[19];
        EthernetValue.Type = EthParam[30];
      }
      else
      // Defaults
      {
        EthernetValue.SelfIp[0] = 192;
        EthernetValue.SelfIp[1] = 168;
        EthernetValue.SelfIp[2] = 1;
        EthernetValue.SelfIp[3] = 10;
        EthernetValue.SelfSubnet[0] = 255;
        EthernetValue.SelfSubnet[1] = 255;
        EthernetValue.SelfSubnet[2] = 255;
        EthernetValue.SelfSubnet[3] = 0;
        EthernetValue.SelfGateway[0] = 192;
        EthernetValue.SelfGateway[1] = 168;
        EthernetValue.SelfGateway[2] = 1;
        EthernetValue.SelfGateway[3] = 1;
        EthernetValue.DestinationIp[0] = 192;
        EthernetValue.DestinationIp[1] = 168;
        EthernetValue.DestinationIp[2] = 1;
        EthernetValue.DestinationIp[3] = 5;
        EthernetValue.SelfPort[0] = 0x0C;
        EthernetValue.SelfPort[1] = 0x7C;
        EthernetValue.DestPort[EthSocket - 1][0] = 0x13;
        EthernetValue.DestPort[EthSocket - 1][1] = 0x88;
        EthernetValue.Type = CLIENT;
      }
      EthRead = true;
    }
    // Check that link is still up
    EthReg(0, 0, PHYCFGR, 1, EthBuffer, READ);
    if (((EthBuffer[0] & 0xF1) != 0xB1) & (EthProgress[EthSocket] != 0xFF) & (EthProgress[EthSocket] != 0))
    {
      EthProgress[EthSocket] = 0xFF;
    }
    // Check Ethernet
    if ((EthProgress[EthSocket] < 4) & (EthernetReset >= ETHERNETRESET))
    {
      EthernetReset = 0;
      EthProgress[EthSocket] = 0xFF;
    }
    if (EthProgress[EthSocket] == 0xFF)
    {
      EthProgress[EthSocket] = EthInit(); // Reset Ethernet IC
    }
    if (EthProgress[EthSocket] == 0)
    {
      EthProgress[EthSocket] = EthConfigure(&EthernetValue.SelfIp[0]); // Configure Ethernet IC
    }
    if (EthProgress[EthSocket] == 1)
    {
      EthProgress[EthSocket] = SocketInit(EthSocket, SOCK_TCP, &EthernetValue.SelfIp[0]); // Intialise and open socket
    }
    if (EthProgress[EthSocket] == 2)
    {
      if (EthernetValue.Type == CLIENT)
        EthProgress[EthSocket] = SocketClient(EthSocket, &EthernetValue.SelfIp[0]); // Make socket a client
      if (EthernetValue.Type == SERVER)
        EthProgress[EthSocket] = SocketServer(EthSocket); // Make socket a server
    }
    if (EthProgress[EthSocket] == 3)
    {
      if (EthSocket == 1)
      {
        if (Socket1Opened > 0)
        {
          Socket1Opened = 0;
          SocketClose(1);
        }
      }
      if (EthSocket == 2)
      {
        if (Socket2Opened > 0)
        {
          Socket2Opened = 0;
          SocketClose(2);
        }
      }
      NoCommsTimeout = 0;
      EthProgress[EthSocket] = SocketEstablish(EthSocket); // Establish socket, try until timout or succesful
    }
    if ((EthProgress[EthSocket] >= 4) & (EthProgress[EthSocket] <= 6))
    {
      NoCommsTimeout = 0;
      EthernetReset = 0;
      // Check for received data
      if (EthProgress[EthSocket] == 4)
      {
        EthProgress[EthSocket] = SocketCheckRX(EthSocket, EthernetValue.Type); // EthProgress[EthSocket] becomes a 5 once data is received, becomes a 0 if for some reason the socket is closed
      }
      // Data received (In global array EthData)
      if (EthProgress[EthSocket] == 5)
      {
        RXLen = SocketRX(EthSocket, &EthernetValue.SelfIp[0]);
        for (uint8_t i = 0; i < RXLen; i++)
          RXReg[i] = EthData[i + (256 * (EthSocket - 1))];
        if ((RXReg[0] == 'S') & (RXReg[1] == 'e') & (RXReg[2] == 'n') & (RXReg[3] == 'd'))
        {
          EthData[0 + (256 * (EthSocket - 1))] = 'R';
          EthData[1 + (256 * (EthSocket - 1))] = 'e';
          EthData[2 + (256 * (EthSocket - 1))] = 'c';
          EthData[3 + (256 * (EthSocket - 1))] = 'e';
          EthData[4 + (256 * (EthSocket - 1))] = 'i';
          EthData[5 + (256 * (EthSocket - 1))] = 'v';
          EthData[6 + (256 * (EthSocket - 1))] = 'e';
          EthData[7 + (256 * (EthSocket - 1))] = 'd';
          EthData[8 + (256 * (EthSocket - 1))] = ':';
          EthData[9 + (256 * (EthSocket - 1))] = ' ';
          EthData[10 + (256 * (EthSocket - 1))] = EthSocket + 0x30;
          LenTX[EthSocket] = 11;
          EthProgress[EthSocket] = 6;
        }
        else
        {
          EthProgress[EthSocket] = 4;
          EthReceived = true;
        }
      }
      // Transmit data
      if ((EthProgress[EthSocket] == 6) | TransmitNow)
      {
        EthProgress[EthSocket] = SocketTX(EthSocket, SOCK_TCP, &EthernetValue.SelfIp[0]);
        TransmitNow = false;
        // EthProgress[EthSocket] = 4;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// Serial task occurs every 10mS
void SerialTask(void *p)
{
  uint8_t data[256];
  int uart_num = PORTNUMBER;

  for (;;)
  {
    int len = uart_read_bytes(uart_num, data, BUF_SIZE, PACKET_READ_TICS);
    if ((len > 0) & (len < 256))
    {
      data[len] = 0;
      // memcpy(RXReg, data, len);
      ESP_LOGI(TAG, "Received: %s, Length: %d", data, len);
      // ESP_LOGI(TAG, "Received");
    }
  }
  vTaskDelete(NULL);
}

// Input and output task occurs every 10mS.
void InputTask(void *p)
{
  uint8_t Debounce = 0;
  uint8_t ReadInputs = 0, LastInputs = 0;
  uint8_t i, temp;

  for (;;)
  {
    if (!gpio_get_level(IN1))
      ReadInputs = ReadInputs | 1;
    else
      ReadInputs = ReadInputs & 0xFE;
    if (!gpio_get_level(IN2))
      ReadInputs = ReadInputs | 2;
    else
      ReadInputs = ReadInputs & 0xFD;
    if (!gpio_get_level(IN3))
      ReadInputs = ReadInputs | 4;
    else
      ReadInputs = ReadInputs & 0xFB;
    if (!gpio_get_level(SWITCH))
      ReadInputs = ReadInputs | 8;
    else
      ReadInputs = ReadInputs & 0xF7;
    if (LastInputs != ReadInputs)
    {
      Debounce++;
      if (Debounce >= DEBOUNCETIME) // Debounced change in input
      {
        // Record which bit has changed and how
        temp = 0x80;
        for (i = 0; i < 8; i++)
        {
          if ((LastInputs & temp) != (ReadInputs & temp)) // Bit has changed
          {
            if ((LastInputs & temp) == 0) // Bit was 0
              InputsLH = InputsLH | temp;
            else // Bit was 1
              InputsHL = InputsHL | temp;
          }
          temp = temp >> 1;
        }
        // Save new value for the digital inputs
        Inputs = ReadInputs;
        LastInputs = ReadInputs;
        Debounce = 0;
        ESP_LOGI(TAG, "Inputs: %2x, LH: %2x, HL: %2x", Inputs, InputsLH, InputsHL);
      }
    }
    else
      Debounce = 0;
    if (Indicator != 0xFF)
      Indicator++;
    if (Indicator >= 5)
    {
      Pin_High(PLEDB);
      Indicator = 0xFF;
    }
    // Stop counter
    if (StopCounter > 0)
    {
      StopCounter--;
      if (StopCounter == 0)
        StopWeighing = true;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// Task occurs every 1S.
void OneSTask(void *p)
{
  for (;;)
  {
    ESP_LOGI(TAG, "Status: %d", EthProgress[EthSocket]);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// Collision test
void CollisionTask(void *p)
{
  uint8_t len;
  uint32_t Counter = 0;
  char str[20];

  for (;;)
  {
    IntToString(str, Counter++);
    len = strlen(str);
    str[len++] = 0x0D;
    str[len++] = 0x0A;
    str[len] = 0x00;
    RS485_Write(PORTNUMBER, str, len);
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// Main routine
void app_main(void)
{
  bool ZeroCalibration = false, FullCalibration = false;
  bool ZeroOn = false;
  bool WeightZero = true;
  bool ShowWeight = false;
  bool SkipReading = true;
  bool ReturnedToZero = true;
  bool A420Calib = false;
  bool TimeStart = false;
  uint8_t WeightCounter = 0;
  uint8_t ShowCounter = 0;
  uint8_t FramData[4];
  char ReturnStr[30];
  // uint32_t temp32low = 0xFFFFFFFF, temp32high = 0, Counter = 0;
  uint16_t Calib420Value = 0;
  uint32_t ReadWeight;
  uint32_t WeightCount = 0;
  int AverageWeight = 0;
  int SendWeight = 0;
  float Weight;
  float AdjustedWeight;
  float LastReading = 0.0;
  float ZeroValue = 0.0;
  double WeightTotal = 0;
  struct AD7190Mode AD7190Modes;
  struct AD7190Mode Test;
  struct AD7190Config AD7190Configs;
  struct ScaleValues ScaleValue;
  struct EthernetValues EthernetValue;
  struct A420Values A420Value;
  struct UARTValues UARTValue;

  // brief Set the log level for serial port printing.
  esp_log_level_set("*", ESP_LOG_INFO);
  esp_log_level_set(TAG, ESP_LOG_DEBUG);

  // Initialise pins
  // Outputs
  Pin_Init(LEDG, GPIO_MODE_OUTPUT, 1);
  Pin_Init(LEDB, GPIO_MODE_OUTPUT, 1);
  Pin_Init(LEDR, GPIO_MODE_OUTPUT, 1);
  Pin_Init(AMOSI, GPIO_MODE_OUTPUT, 1);
  Pin_Init(ACLK, GPIO_MODE_OUTPUT, 1);
  Pin_Init(ASS, GPIO_MODE_OUTPUT, 1);
  Pin_Init(AOMOSI, GPIO_MODE_OUTPUT, 1);
  Pin_Init(AOCLK, GPIO_MODE_OUTPUT, 1);
  Pin_Init(AOSS, GPIO_MODE_OUTPUT, 1);
  Pin_Init(ERST, GPIO_MODE_OUTPUT, 1);
  Pin_Init(ESS, GPIO_MODE_OUTPUT, 1);
  Pin_Init(ECLK, GPIO_MODE_OUTPUT, 1);
  Pin_Init(EMOSI, GPIO_MODE_OUTPUT, 1);
  // Inputs
  Pin_Init(SWITCH, GPIO_MODE_INPUT, 1);
  Pin_PullUp(SWITCH);
  Pin_Init(IN1, GPIO_MODE_INPUT, 1);
  Pin_PullUp(IN1);
  Pin_Init(IN2, GPIO_MODE_INPUT, 1);
  Pin_PullUp(IN2);
  Pin_Init(IN3, GPIO_MODE_INPUT, 1);
  Pin_PullUp(IN3);
  Pin_Init(AMISO, GPIO_MODE_INPUT, 0);
  Pin_Init(EMISO, GPIO_MODE_INPUT, 0);

  Fram_Init();
  // Fram_Clear();
  AD7190_Reset();
  // Load AD7190 config and mode registers
  AD7190_Load(&AD7190Modes, &AD7190Configs);
  if (!AD7190_Mode(&AD7190Modes))
    ESP_LOGI(TAG, "Error programming AD7190 mode register");
  if (!AD7190_Config(&AD7190Configs))
    ESP_LOGI(TAG, "Error programming AD7190 config register");
  // Load AD7190 Scale register
  AD7190_ScaleLoad(&ScaleValue);
  // Load AD5621 structure
  AD5621_Load(&A420Value);
  // Make AD5621 4mA
  AD5621_Write(A420Value.CalibZero);
  // Load RS485
  RS485_Load(&UARTValue);
  // Initialise RS485
  RS485_Init(&UARTValue, PORTNUMBER);
  // Check RS485 data
  uint32_t A1;
  uint8_t A2, A3, A4;
  uart_get_baudrate(PORTNUMBER, &A1);
  uart_get_word_length(PORTNUMBER, &A2);
  uart_get_parity(PORTNUMBER, &A3);
  uart_get_stop_bits(PORTNUMBER, &A4);
  ESP_LOGI(TAG, " Baud: %d, Bits: %d, Parity: %d, Stop: %d", A1, A2, A3, A4);
  // Initialise Ethernet SPI
  EthSPI_Init();
  EthInit();
  uint8_t data[10];
  for (uint8_t j = 0; j < 10; j++)
    data[j] = 0;
  EthReg(0, 0, 0x0039, 1, data, 0);
  EthReg(0, 0, 0x0039, 1, data, 0);
  EthReg(0, 0, 0x0039, 1, data, 0);
  EthReg(0, 0, 0x0039, 1, data, 0);
  ESP_LOGI(TAG, "Ethernet: %2x, %2x, %2x, %2x, %2x", data[0], data[1], data[2], data[3], data[4]);

  // Create 10mS task for Ethernet
  xTaskCreate(EthernetTask, "Ethernet task occurs every 10mS", 8192, NULL, 2, NULL);
  // Create 10mS task for inputs
  xTaskCreate(InputTask, "Input task occurs every 10mS", 4196, NULL, 2, NULL);
  // Create Serial task
  xTaskCreate(SerialTask, "Serial task occurs every PACKET_READ_TICS", 4196, NULL, 2, NULL);
  // Collision task
  // xTaskCreate(CollisionTask, "Task occurs every 50mS", 4196, NULL, 2, NULL);
  // Create 1S task
  // xTaskCreate(OneSTask, "Task occurs every 1S", 4196, NULL, 2, NULL);

  // Main loop
  for (;;)
  {
    // Test inputs
    if (InputsLH & INP1)
    {
      if (ScaleValue.StartTrigger == 2)
        StartWeighing = true;
      if (ScaleValue.StartTrigger == 3)
        StopWeighing = true;
      InputsLH = InputsLH & (~INP1);
    }

    if (InputsHL & INP1)
    {
      if (ScaleValue.StartTrigger == 3)
        StartWeighing = true;
      if (ScaleValue.StartTrigger == 2)
        StopWeighing = true;
      InputsHL = InputsHL & (~INP1);
    }
    if (InputsLH & INP2)
    {
      if (ScaleValue.StartTrigger == 5)
        StopWeighing = true;
      InputsLH = InputsLH & (~INP2);
    }

    if (InputsHL & INP2)
    {
      if (ScaleValue.StartTrigger == 4)
        StopWeighing = true;
      InputsHL = InputsHL & (~INP2);
    }
    if (InputsLH & INP3)
    {
      if (ScaleValue.ZeroType == 2)
        ZeroOn = false;
      if (ScaleValue.ZeroType == 3)
        ZeroOn = true;
      InputsLH = InputsLH & (~INP3);
    }

    if (InputsHL & INP3)
    {
      if (ScaleValue.ZeroType == 2)
        ZeroOn = true;
      if (ScaleValue.ZeroType == 3)
        ZeroOn = false;
      InputsHL = InputsHL & (~INP3);
    }
    if (InputsLH & INP4)
    {
      InputsLH = InputsLH & (~INP4);
    }

    if (InputsHL & INP4)
    {
      InputsHL = InputsHL & (~INP4);
    }

    if (AD7190_Ready())
    {
      uint8_t AIBuffer[3];
      AIBuffer[0] = 0x58; // Write to communication register
      AD7190_Write(1, AIBuffer);
      ReadWeight = AD7190_Read(3, AIBuffer) & 0x00FFFFFF;
      // ESP_LOGI(TAG, "ReadWeight: %d", ReadWeight);
      if (ZeroCalibration)
        AD7190_ScaleZero(&ScaleValue, ReadWeight, &ZeroCalibration);
      if (FullCalibration)
        AD7190_ScaleFull(&ScaleValue, ReadWeight, &FullCalibration);
      Weight = ((float)ReadWeight - (float)ScaleValue.CalibZero) * ScaleValue.CalibRatio;
      // Zeroing
      if (fabs(Weight) < ScaleValue.ZeroTarget)
      {
        // ESP_LOGI(TAG, "Weight: %4.2f, Struct: %d", Weight, ScaleValue.ZeroTarget);
        WeightZero = true;
        if ((ScaleValue.ZeroType == 1) | ZeroOn)
        {
          if (ScaleValue.ZeroTarget > 0)
            ZeroValue = AD7190_Zero(Weight, 10, &Indicator);
        }
        else
          ZeroValue = 0;
        ReturnedToZero = true;
      }
      else
      {
        WeightZero = false;
        if ((ScaleValue.StartTrigger == 1) & ReturnedToZero)
        {
          ReturnedToZero = false;
          StartWeighing = true;
          LastReading = 0;
          WeightCounter = 0;
          Pin_Low(PLEDG);
        }
      }
      // Start weighing
      AdjustedWeight = Weight - ZeroValue;
      SendWeight = round(AdjustedWeight); // SendWeight is used unadjusted in freeflow mode
      if (StartWeighing)
      {
        if (!StopWeighing)
        {
          // Stop when weight settles
          if (ScaleValue.StartTrigger == 1)
          {
            ESP_LOGI(TAG, "Flag: %d", ScaleValue.StartTrigger);
            WeightCounter++;
            if (fabs(AdjustedWeight - LastReading) < ScaleValue.WeightVar)
            {
              if (WeightCounter >= ScaleValue.WeightCount)
              {
                AverageWeight = round(AdjustedWeight);
                StopWeighing = true;
              }
            }
            else
              WeightCounter = 0;
            LastReading = AdjustedWeight;
          }
          else
          {
            if ((SkipReading) & (ScaleValue.Skip == 1))
              SkipReading = false;
            else
            {
              WeightTotal = WeightTotal + AdjustedWeight;
              WeightCount++;
              AverageWeight = round(WeightTotal / WeightCount);
              if ((ScaleValue.StopTrigger == 1) & (TimeStart == false))
              {
                TimeStart = true;
                StopCounter = ScaleValue.StopTime / 10;
              }
              Pin_Low(PLEDG);
            }
          }
        }
        if (StopWeighing)
        {
          StartWeighing = false;
          StopWeighing = false;
          SkipReading = true;
          TimeStart = false;
          ShowCounter = 0;
          ESP_LOGI(TAG, "WeightTotal: %4.2f, WeightCount: %d, AverageWeight: %d", WeightTotal, WeightCount, AverageWeight);
          WeightTotal = 0;
          WeightCount = 0;
          ShowWeight = true;
          Pin_High(PLEDG);
        }
      }
      if (ShowWeight)
      {
        if (ScaleValue.TransRead != 0)
        {
          if (ShowCounter < ScaleValue.TransRead)
          {
            ESP_LOGI(TAG, "Counter: %d, Count: %d", ShowCounter, ScaleValue.TransRead);
            ShowCounter++;
          }
          else
          {
            ShowWeight = false;
            ShowCounter = 0;
          }
        }
      }
      /*if ((!WeightZero) & (ScaleValue.StartTrigger != 0))
      {
        if (ScaleValue.TransRead == 0)
          ShowWeight = true;
        else
        {
          if (ShowCounter < ScaleValue.TransRead)
          {
            ShowWeight = true;
            ShowCounter++;
          }
          else
            ShowWeight = false;
        }
      }*/

      // Send weight through ethernet if allowed to by setup
      if (((ScaleValue.TransZero != 0) & (WeightZero)) | ShowWeight | (ScaleValue.StartTrigger == 0))
      {
        char tempstr[20];
        uint16_t A420Weight = 0;
        // ESP_LOGI(TAG, "TransZero: %d, WeightZero: %d, ShowWeight: %d, SendWeight: %d", ScaleValue.TransZero, WeightZero, ShowWeight, SendWeight);
        if (ShowWeight)
        {
          SendWeight = AverageWeight;
        }
        IntToString(tempstr, SendWeight);
        if (Fram_Loc_Updated(PREFIXU))
          strcpy(ReturnStr, ScaleValue.Prefix);
        else
          strcpy(ReturnStr, "");
        strcat(ReturnStr, tempstr);
        if (Fram_Loc_Updated(SUFFIXU))
          strcat(ReturnStr, ScaleValue.Suffix);
        if (EthProgress[EthSocket] >= 4)
          Send_Weight(ReturnStr);
        TransmitNow = true;
        // Send weight via RS485
        RS485_Write(PORTNUMBER, ReturnStr, strlen(ReturnStr));
        // Prepare and send weight for 4-20mA
        if (!A420Calib)
        {
          if (SendWeight < 0)
            A420Weight = A420Value.CalibZero;
          else
          {
            if (SendWeight > A420Value.CalibWeight)
              A420Weight = ((uint16_t)(A420Value.CalibWeight * A420Value.CalibRatio)) + A420Value.CalibZero;
            else
              A420Weight = ((uint16_t)(SendWeight * A420Value.CalibRatio)) + A420Value.CalibZero;
            // ESP_LOGI("AD", "AD5621: %d, SendWeight: %d, Weight: %d, Ratio: %4.2f", A420Weight, SendWeight, A420Value.CalibWeight, A420Value.CalibRatio);
          }
          AD5621_Write(A420Weight);
        }
      }
      // ESP_LOGI(TAG, "Progress: %d, Length: %d", EthProgress[EthSocket], LenTX[EthSocket]);
      // ESP_LOGI(TAG, "SendWeight: %d, String: %s", SendWeight, ReturnStr);
    }

    if (EthReceived)
    {
      ESP_LOGI(TAG, "Ethernet received");
      // Check if mode needs to be updated
      if (AD7190_ModeLoad(&AD7190Modes))
        ESP_LOGI(TAG, "AD7190 mode register updated");
      if (AD7190_ConfigLoad(&AD7190Configs))
        ESP_LOGI(TAG, "AD7190 config register updated");
      AD7190_ModeRead(&AD7190Modes);
      AD7190_ConfigRead(&AD7190Configs);
      AD7190_ScaleSet(&ScaleValue, &ZeroCalibration, &FullCalibration);
      AD7190_ScaleRead(&ScaleValue);
      AD5621_Calib(&A420Calib, &A420Value);
      AD5621_Read(&A420Value);
      // 4-20mA calibration output
      if (A420Calib)
        AD5621_Write(A420Value.CalibValue);
      EthReceived = false;
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
