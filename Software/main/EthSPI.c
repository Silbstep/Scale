#include "EthSPI.h"

// Initialise SPI for Ethernet (W5500)
void EthSPI_Init(void)
{
  esp_err_t ret;

  spi_bus_config_t buscfg = {
      .miso_io_num = PIN_NUM_MISO,
      .mosi_io_num = PIN_NUM_MOSI,
      .sclk_io_num = PIN_NUM_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 2048};
  spi_device_interface_config_t devcfg = {
      .clock_speed_hz = 10 * 100, // Clock out at 10 MHz
      .mode = 3,                  // SPI mode 0
      .spics_io_num = -1,         // CS pin
      .queue_size = 7,            // We want to be able to queue 7 transactions at a time
  };
  // Initialize the SPI bus
  ret = spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
  assert(ret == ESP_OK);
  // Attach the Ethernet to the SPI bus
  ret = spi_bus_add_device(LCD_HOST, &devcfg, &spi);
  assert(ret == ESP_OK);
}

// Send data to the W5500. Uses spi_device_transmit, which waits until the transfer is complete.
void EthSPI_Write(uint8_t *data, int len)
{
  esp_err_t ret;
  spi_transaction_t t;
  if (len == 0)
    return;                           // no need to send anything
  memset(&t, 0, sizeof(t));           // Zero out the transaction
  t.length = len * 8;                 // Len is in bytes, transaction length is in bits.
  t.tx_buffer = data;                 // Data
  ret = spi_device_transmit(spi, &t); // Transmit!
  assert(ret == ESP_OK);              // Should have had no issues.
}

// Read data from the W5500. Uses spi_device_transmit, which waits until the transfer is complete.
uint32_t EthSPI_Read(uint8_t *data, int len)
{
  esp_err_t ret;
  spi_transaction_t t;
  memset(&t, 0, sizeof(t));
  t.length = len * 8;
  // t.flags = SPI_TRANS_USE_RXDATA; //Use if data must be stored in t.rx_data else data is stored in area pointe to by t.rx_buffer (in this case in data)
  t.user = (void *)1;
  t.rx_buffer = data;
  ret = spi_device_transmit(spi, &t);
  assert(ret == ESP_OK);
  return *(uint32_t *)t.rx_data;
}