#ifndef __FRAM_h
#define __FRAM_h

#include "Global.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <string.h>

// Function prototypes
// Initialise I2C for FRAM
esp_err_t Fram_Init(void);
// Read FRAM (Address - 16 bits, len amount of data)
void Fram_Read(uint16_t Address, uint8_t *data, size_t len);
// Write FRAM (Maximum of 16 bytes, Address - 16 bits, len amount of data)
void Fram_Write(uint16_t Address, uint8_t *data, size_t len);
// Clear FRAM contents and make address 8191 0x4d to indicate that FRAM has been cleared.
void Fram_Clear(void);
// Check FRAM for 0x4D in address 8191, clear fram if 0x4d not there (used to clear FRAM the first time it is used).
void Fram_Check(void);
// Check if FRAM memory location has been updated. Contains 0xa7 if it has been updated. Returns true if it has been updated
bool Fram_Loc_Updated(uint16_t Location);
// Update FRAM with data from web server
void Fram_Update(uint8_t *WriteData, uint16_t Location, uint8_t Length, uint16_t LocationU);
// Check if data to be written is the same as data in FRAM
bool Fram_Same(uint8_t *WriteData, uint16_t Location, uint8_t Length);
#endif