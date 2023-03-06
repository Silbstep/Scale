#ifndef __EthSPI_h
#define __EthSPI_h

#include "Global.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include <string.h>

// Global variables
extern spi_device_handle_t spi;

// Function prototypes
// Initialise SPI for Ethernet (W5500)
void EthSPI_Init(void);
// Send data to the LCD. Uses spi_device_transmit, which waits until the transfer is complete.
void EthSPI_Write(uint8_t *data, int len);
// Send data to the LCD. Uses spi_device_transmit, which waits until the transfer is complete.
uint32_t EthSPI_Read(uint8_t *data, int len);
#endif