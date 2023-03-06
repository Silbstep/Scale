#include "FRAM.h"

esp_err_t Fram_Init(void)
{
  int i2c_master_port = I2C_MASTER_NUMBER;

  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .sda_pullup_en = false,
      .scl_pullup_en = false,
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };
  i2c_param_config(i2c_master_port, &conf);
  return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

// Read FRAM (Address - 16 bits, len amount of data)
void Fram_Read(uint16_t Address, uint8_t *data, size_t len)
{
  uint8_t Opcode[2];

  Opcode[0] = (uint8_t)(Address >> 8);
  Opcode[1] = (uint8_t)(Address);
  i2c_master_write_read_device(I2C_MASTER_NUMBER, FRAM_ADDRESS, Opcode, 2, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

// Write FRAM (Maximum of 48 bytes, Address - 16 bits, len amount of data)
void Fram_Write(uint16_t Address, uint8_t *data, size_t len)
{
  uint8_t i;
  uint8_t write_buffer[50];

  write_buffer[0] = (uint8_t)(Address >> 8);
  write_buffer[1] = (uint8_t)(Address);
  for (i = 0; i < len; i++)
    write_buffer[2 + i] = data[i];
  i2c_master_write_to_device(I2C_MASTER_NUMBER, FRAM_ADDRESS, write_buffer, len + 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void Fram_Clear(void)
{
  uint8_t i;
  uint8_t write_buffer[1026];
  uint16_t Address = 0;

  for (i = 0; i < 7; i++)
  {
    memset(write_buffer, 0, 1026); // sizeof(write_buffer));
    write_buffer[0] = (uint8_t)(Address >> 8);
    write_buffer[1] = (uint8_t)(Address);
    write_buffer[FRAMSIZE - 1] = 0x4d;
    i2c_master_write_to_device(I2C_MASTER_NUMBER, FRAM_ADDRESS, write_buffer, 1024, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    Address = Address + 1024;
  }
}

void Fram_Check(void)
{
  uint8_t read_buffer[1];

  Fram_Read(8191, read_buffer, 1);
  if (read_buffer[0] != 0x4d)
  {
    Fram_Clear();
    ESP_LOGI("FRAM", "FRAM cleared");
  }
  else
    ESP_LOGI("FRAM", "FRAM not cleared");
}

bool Fram_Loc_Updated(uint16_t Location)
{
  uint8_t read_buffer[1];

  Fram_Read(Location, read_buffer, 1);
  if (read_buffer[0] == 0xa7)
    return true;
  else
    return false;
}

void Fram_Update(uint8_t *WriteData, uint16_t Location, uint8_t Length, uint16_t LocationU)
{
  // Write data of length LocationL starting at address Location
  Fram_Write(Location, WriteData, Length);
  // Update register at Location U with 0xa7 to indicate that memory has been updated
  WriteData[0] = 0xa7;
  Fram_Write(LocationU, WriteData, 1);
}

bool Fram_Same(uint8_t *WriteData, uint16_t Location, uint8_t Length)
{
  bool ReturnValue = true;
  uint8_t i;
  uint8_t ReadData[64];

  Fram_Read(Location, ReadData, Length);
  for (i = 0; i < Length; i++)
  {
    if (WriteData[i] != ReadData[i])
    {
      ReturnValue = false;
      break;
    }
    if (WriteData[i] == 0)
      break;
  }
  return ReturnValue;
}
