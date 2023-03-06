#ifndef __AD5621_h
#define __AD5621_h

#include "Global.h"
#include "InOut.h"
#include "Structures.h"
#include "Conversion.h"
#include "stdint.h"
#include "FRAM.h"
#include "esp_log.h"

// Global variables
extern bool A420Calib;
extern uint8_t EthSocket;
extern uint8_t EthProgress[9];
extern uint8_t RXReg[256];
extern uint8_t EthData[ETHBUFFER];
extern uint16_t LenTX[9];
extern uint16_t Calib420Value;

// Function prototypes
// Load AD5621 structure from FRAM
void AD5621_Load(struct A420Values *A420Value);
//  Write to AD5621
void AD5621_Write(uint16_t Data);
// Calibrate AD5621
void AD5621_Calib(bool *A420Calib, struct A420Values *A420Value);
// Read AD5621 calibration values
void AD5621_Read(struct A420Values *A420Value);

#endif