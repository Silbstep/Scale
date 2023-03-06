#ifndef __RS485_h
#define __RS485_h

#include "Global.h"
#include "InOut.h"
#include "Structures.h"
#include "Conversion.h"
#include "FRAM.h"
#include "esp_log.h"
#include <driver/uart.h>

// Global variables

// Function prototypes
// Load RS485 structure from FRAM
void RS485_Load(struct UARTValues *UARTValue);
// Initialise UART
void RS485_Init(struct UARTValues *UARTValue, uint8_t PortNumber);
// RS485 write
void RS485_Write(uint8_t PortNumber, char *str, uint8_t len);

#endif