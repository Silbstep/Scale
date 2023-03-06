#include "RS485.h"

#define TAG "RS485"

void RS485_Load(struct UARTValues *UARTValue)
{
  uint8_t RSData[10];

  // Databits
  if (Fram_Loc_Updated(DATABITSU))
  {
    Fram_Read(DATABITS, RSData, DATABITSL);
    memcpy(&UARTValue->DataBits, RSData, DATABITSL);
    ESP_LOGI(TAG, "RS485 Databits from FRAM: %d", UARTValue->DataBits);
  }
  else
    UARTValue->DataBits = UART_DATA_8_BITS;
  // Parity
  if (Fram_Loc_Updated(PARITYU))
  {
    Fram_Read(PARITY, RSData, PARITYL);
    memcpy(&UARTValue->Parity, RSData, PARITYL);
    ESP_LOGI(TAG, "Parity from FRAM: %d", UARTValue->Parity);
  }
  else
    UARTValue->Parity = UART_PARITY_DISABLE;
  // Stopbits
  if (Fram_Loc_Updated(STOPBITSU))
  {
    Fram_Read(STOPBITS, RSData, STOPBITSL);
    memcpy(&UARTValue->StopBits, RSData, STOPBITSL);
    ESP_LOGI(TAG, "RS485 Stopbits from FRAM: %d", UARTValue->StopBits);
  }
  else
    UARTValue->StopBits = UART_STOP_BITS_1;
  // BaudRate
  if (Fram_Loc_Updated(BAUDRATEU))
  {
    Fram_Read(BAUDRATE, RSData, BAUDRATEL);
    memcpy(&UARTValue->BaudRate, RSData, BAUDRATEL);
    ESP_LOGI(TAG, "RS485 Baudrate from FRAM: %d", UARTValue->BaudRate);
  }
  else
    UARTValue->BaudRate = 115200;
  // Flow control
  UARTValue->FlowControl = UART_HW_FLOWCTRL_DISABLE;
  // Mode
  UARTValue->Mode = UART_MODE_RS485_HALF_DUPLEX;
}

void RS485_Init(struct UARTValues *UARTValue, uint8_t PortNumber)
{
  int uart_num = PortNumber;
  uart_config_t uart_config = {
      .baud_rate = UARTValue->BaudRate,
      .data_bits = UARTValue->DataBits,
      .parity = UARTValue->Parity,
      .stop_bits = UARTValue->StopBits,
      .flow_ctrl = UARTValue->FlowControl,
      .rx_flow_ctrl_thresh = 122,
      .source_clk = UART_SCLK_APB,
  };

  // Install UART driver (we don't need an event queue here)
  // In this example we don't even use a buffer for sending data.
  ESP_ERROR_CHECK(uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0));
  // Configure UART parameters
  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
  // Set pins
  uart_set_pin(uart_num, RS485TX, RS485RX, RS485RTS, UART_PIN_NO_CHANGE);
  // Set RS485 half duplex mode
  ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX));
  // ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_APP_CTRL));
  // UART1->rs485_conf->rx_busy_tx_en = 0; // don't send while receiving => collision avoidance
  // UART1.rs485_conf.tx_rx_en = 1;        // loopback (1), so collision detection works
}

void RS485_Write(uint8_t PortNumber, char *str, uint8_t len)
{
  bool RS485Collision;
  if (uart_write_bytes(PortNumber, str, len) != len)
  {
    ESP_LOGI(TAG, "Send data critical failure.");
  }
  uart_get_collision_flag(PortNumber, &RS485Collision);
  if (RS485Collision)
    ESP_LOGI(TAG, "Collision occured.");
}
