#ifndef __AD7190_h
#define __AD7190_h

#include "Global.h"
#include "InOut.h"
#include "Structures.h"
#include "Conversion.h"
#include "FRAM.h"
#include "esp_log.h"
#include <stdlib.h>
#include <math.h>

// Global variables
extern bool StartWeighing;
extern bool StopWeighing;
extern uint8_t EthSocket;
extern uint8_t EthProgress[9];
extern uint8_t RXReg[256];
extern uint8_t EthData[ETHBUFFER];
extern uint16_t LenTX[9];

// Function prototypes
//  Reset AD7190
void AD7190_Reset();
// Returns true when data ready
bool AD7190_Ready(void);
// Read ID
uint8_t AD7190_ReadID(void);
// Read from AD7190
uint32_t AD7190_Read(uint8_t Count, uint8_t *Buf);
// Write to AD7190
void AD7190_Write(uint8_t Count, uint8_t *Buf);
// Write to mode register
bool AD7190_Mode(struct AD7190Mode *AD7190Modes);
// Write to config register
bool AD7190_Config(struct AD7190Config *AD7190Configs);
// Load AD7190 config and mode registers from FRAM if updated else use defaults
void AD7190_Load(struct AD7190Mode *AD7190Modes, struct AD7190Config *AD7190Configs);
// Update AD7190 mode register
bool AD7190_ModeLoad(struct AD7190Mode *AD7190Modes);
// Update AD7190 config register
bool AD7190_ConfigLoad(struct AD7190Config *AD7190Configs);
// Read AD7190 mode register
void AD7190_ModeRead(struct AD7190Mode *AD7190Modes);
// Read AD7190 config register
void AD7190_ConfigRead(struct AD7190Config *AD7190Configs);
// Setup Scale
bool AD7190_ScaleSet(struct ScaleValues *ScaleValue, bool *ZeroCalibration, bool *FullCalibration);
// Read scale setup
void AD7190_ScaleRead(struct ScaleValues *ScaleValue);
// Load values from FRAM into ScaleValue structure
void AD7190_ScaleLoad(struct ScaleValues *ScaleValue);
// Zero calibrate
void AD7190_ScaleZero(struct ScaleValues *ScaleValue, uint32_t ReadWeight, bool *ZeroCalibration);
// Full calibrate
void AD7190_ScaleFull(struct ScaleValues *ScaleValue, uint32_t ReadWeight, bool *FullCalibration);
// Returns a zero value by averaging acceptable readings
float AD7190_Zero(float Weight, uint8_t Count, uint8_t *Indicator);
// Send Weight
void Send_Weight(char *str);
#endif