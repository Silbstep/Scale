#include "Ethernet.h"

// Initialise Ethernet
uint8_t EthInit(void)
{
  // Hard Reset
  Pin_Low(PERST);
  vTaskDelay(pdMS_TO_TICKS(5));
  Pin_High(PERST);
  vTaskDelay(pdMS_TO_TICKS(5));
  return (0);
}

// Configure Ethernet IC once a link is up
uint8_t EthConfigure(uint8_t *EthernetStructure)
{
  uint8_t status;
  uint8_t Mac[6] = {0};

  EthBuffer[0] = 0x00; // Determine if link is up
  EthReg(0, 0, PHYCFGR, 1, EthBuffer, READ);
  if ((EthBuffer[0] & 0xF1) == 0xB1)
    status = 1;
  else
    status = 0;
  // Link is up so configure Ethernet IC
  if (status == 1)
  {
    // Load Ethernet configuration
    EthBuffer[0] = 0x07; // Ethernet time out (200mS)
    EthBuffer[1] = 0xD0;
    // EthBuffer[0] = 0x00;                                                        //Ethernet time out (200mS)
    // EthBuffer[1] = 0x0A;
    EthReg(0, 0, RTR, 2, EthBuffer, WRITE);
    EthReg(0, 0, RTR, 2, EthBuffer, READ);
    EthBuffer[0] = 0x0A; // 10 retries
    EthReg(0, 0, RCR, 1, EthBuffer, WRITE);
    EthReg(0, 0, RCR, 1, EthBuffer, READ);
    esp_read_mac(Mac, ESP_MAC_ETH); // Get MAC address from ESP32-S3
    ESP_LOGI("ETHERNET", "Ethernet Mac Address: %2x, %2x, %2x, %2x, %2x, %2x", Mac[0], Mac[1], Mac[2], Mac[3], Mac[4], Mac[5]);
    ESP_LOGI("ETHERNET", "Self IP Address: %2x, %2x, %2x, %2x", EthernetStructure[0], EthernetStructure[1], EthernetStructure[2], EthernetStructure[3]);
    EthReg(0, 0, GATEWAY, 4, EthernetStructure + 8, WRITE); // Write Gateway
    EthReg(0, 0, SUBNET, 4, EthernetStructure + 4, WRITE);  // Write Subnet
    EthReg(0, 0, MAC, 6, Mac, WRITE);                       // Write Mac
    EthReg(0, 0, IP, 4, EthernetStructure, WRITE);          // Write Ip
  }
  return (status);
}

// Initialise and open socket
uint8_t SocketInit(uint8_t Socket, uint8_t Protocol, uint8_t *EthernetStructure)
{
  uint8_t status;
  EthTimeOut = 0;
  do
  {
    if (Protocol == SOCK_TCP)
      EthBuffer[0] = 0x01; // Write to SN_MR of socket to make the socket TCP
    if (Protocol == SOCK_UDP)
      EthBuffer[0] = 0x02; // Write to SN_MR of socket to make the socket UDP
    if (Protocol == SOCK_IPRAW)
      EthBuffer[0] = 0x03; // Write to SN_MR of socket to make the socket IPRAW
    if (Protocol == SOCK_MACRAW)
      EthBuffer[0] = 0x04; // Write to SN_MR of socket to make the socket MACRAW
    EthReg(Socket, 0, Sn_MR, 1, EthBuffer, 1);
    EthReg(Socket, 0, Sn_PORT, 2, EthernetStructure + 16, 1); // Write own port number for socket
    EthBuffer[0] = OPEN;                                      // Open Port
    EthReg(Socket, 0, Sn_CR, 1, EthBuffer, 1);
    EthBuffer[0] = 0x00; // Check the socket initialization process is completed
    EthReg(Socket, 0, Sn_SR, 1, EthBuffer, 0);
    if (EthBuffer[0] != Protocol) // Changed Protocol for SOCK_INIT
    {
      EthTimeOut = EthTimeOut + 1; // If not completed try again (for 10 times)
      EthBuffer[0] = CLOSE;
      EthReg(Socket, 0, Sn_CR, 1, EthBuffer, 1);
    }
  } while ((EthBuffer[0] != Protocol) & (EthTimeOut < 10)); // Changed Protocol for SOCK_INIT
  if (EthTimeOut < 10)
    status = 2;
  else
    status = 0xFF; // Reboot chip if socket cannot open
  return (status);
}

// Make socket a server
uint8_t SocketServer(uint8_t Socket)
{
  uint8_t status;

  EthBuffer[0] = LISTEN;
  EthReg(Socket, 0, Sn_CR, 1, EthBuffer, 1);
  EthBuffer[0] = 0x00;
  EthTimeOut = 0;
  while ((EthBuffer[0] != SOCK_LISTEN) & (EthTimeOut < 10)) // Try 10 times for status to change to SOCK_LISTEN
  {
    EthReg(Socket, 0, Sn_SR, 1, EthBuffer, 0); // Read Sn_SR and put result into EthBuffer[0]
    EthTimeOut = EthTimeOut + 1;
  }
  if (EthTimeOut < 10)
    status = 3;
  else
    status = 0xFF; // Reboot chip if socket cannot be made a server
  return (status);
}

// Make socket a client
uint8_t SocketClient(uint8_t Socket, uint8_t *EthernetStructure)
{
  uint8_t status;

  EthReg(Socket, 0, Sn_DIPR, 4, EthernetStructure + 12, 1); // Destination IP
  // EthReg(Socket, 0, Sn_DPORT, 2, EthernetStructure + 18 + (2 * (Socket - 1)), 1); //Destination Port
  EthReg(Socket, 0, Sn_DPORT, 2, EthernetStructure + 18, 1); // Destination Port
  EthBuffer[0] = CONNECT;
  EthReg(Socket, 0, Sn_CR, 1, EthBuffer, 1);
  status = 3;
  EthTimeOut = 0;
  vTaskDelay(pdMS_TO_TICKS(5));
  return (status);
}

// Establish socket, wait until a SYN (server) or ACK SYN (client) packet is received
uint8_t SocketEstablish(uint8_t Socket)
{
  uint8_t status;
  status = 3;

  EthReg(Socket, 0, Sn_SR, 1, EthBuffer, 0); // Read Sn_SR and put result into EthBuffer[0]
  if ((EthBuffer[0] == SOCK_SYNSENT) | (EthBuffer[0] == SOCK_CLOSED) | (EthBuffer[0] == SOCK_CLOSE_WAIT) | (EthBuffer[0] == 0xBF))
    EthTimeOut = EthTimeOut + 1;
  if (EthTimeOut >= 10) // Socket has been closed because no SYN packet has been received before timout
  {
    SocketClose(Socket);
    status = 1; // Try again to open socket
  }
  if (EthBuffer[0] == SOCK_ESTABLISHED) // Socket established
  {
    EthBuffer[0] = 0x01;
    EthReg(Socket, 0, Sn_KPALVTR, 1, EthBuffer, 1);
    EthBuffer[0] = 0x1F;
    EthReg(Socket, 0, Sn_IMR, 1, EthBuffer, WRITE);
    status = 4;
  }
  return (status);
}

// Check if data has been received
uint8_t SocketCheckRX(uint8_t Socket, uint8_t Type)
{
  uint8_t status, temp;

  temp = 3;
  if (Type == CLIENT)
    temp = 0;
  EthReg(Socket, 0, Sn_SR, 1, EthBuffer, 0);
  if ((EthBuffer[0] == SOCK_CLOSED) | (EthBuffer[0] == SOCK_CLOSE_WAIT))
  {
    SocketClose(Socket); // Re-initialize
    status = 3;
  }
  else
  {
    status = 4;
    EthReg(Socket, 0, Sn_RX_RSR, 2, EthBuffer, 0);
    LenRX[Socket] = (EthBuffer[0] << 8) + EthBuffer[1];
    if ((EthBuffer[0] > temp) | (EthBuffer[1] > 0)) // If buffer is not zero then data has been received
      status = 5;
  }
  return (status);
}

// Receive data and put into EthData
uint16_t SocketRX(uint8_t Socket, uint8_t *EthernetStructure)
{
  // Continue checking for data until previous length and current length equal (data has stopped coming)
  // Get length of data
  EthReg(Socket, 0, Sn_RX_RSR, 2, EthBuffer, 0);
  do
  {
    LenRX[Socket] = (EthBuffer[0] << 8) + EthBuffer[1]; // Previous length of data received
    EthReg(Socket, 0, Sn_RX_RSR, 2, EthBuffer, 0);      // New length of data
  } while (LenRX[Socket] != ((EthBuffer[0] << 8) + EthBuffer[1]));
  EthReg(Socket, 0, Sn_RX_RD, 2, EthBuffer, 0); // Get address of received data
  SourcePtr = (EthBuffer[0] << 8) + EthBuffer[1];
  EthReg(Socket, 2, SourcePtr, LenRX[Socket], EthData + (256 * (Socket - 1)), 0); // Read data and put into data buffer
  SourcePtr = SourcePtr + LenRX[Socket];                                          // Update new address of data
  EthBuffer[0] = (uint8_t)(SourcePtr >> 8);
  EthBuffer[1] = (uint8_t)SourcePtr;
  EthReg(Socket, 0, Sn_RX_RD, 2, EthBuffer, 1);
  EthBuffer[0] = RECV; // Complete receive process
  EthReg(Socket, 0, Sn_CR, 1, EthBuffer, 1);
  // EthReg(Socket, 0, Sn_DPORT, 2, EthernetStructure + 18 + (2 * (Socket - 1)), 0);
  EthReg(Socket, 0, Sn_DPORT, 2, EthernetStructure + 18, 0);
  return (LenRX[Socket]);
}

// Transmit data from EthData
uint8_t SocketTX(uint8_t Socket, uint8_t Protocol, uint8_t *EthernetStructure)
{
  uint8_t status = 0;

  EthReg(Socket, 0, Sn_TX_FSR, 2, EthBuffer, 0);            // Available space for data
  if (LenTX[Socket] < ((EthBuffer[0] << 8) + EthBuffer[1])) // Only transmit if space is available
  {
    if ((Protocol == SOCK_UDP) | (Protocol == SOCK_IPRAW))
    {
      EthReg(Socket, 0, Sn_DIPR, 4, EthernetStructure + 12, 1); // Destination IP
      // EthReg(Socket, 0, Sn_DPORT, 2, EthernetStructure + 18 + (2 * (Socket - 1)), 1);      //Destination Port
      EthReg(Socket, 0, Sn_DPORT, 2, EthernetStructure + 18, 1);
    }
    EthReg(Socket, 0, Sn_TX_WR, 2, EthBuffer, 0); // Get address in transmit data buffer
    DestPtr = (EthBuffer[0] << 8) + EthBuffer[1];
    EthReg(Socket, 1, DestPtr, LenTX[Socket], EthData + (256 * (Socket - 1)), 1); // Write data
    DestPtr = DestPtr + LenTX[Socket];                                            // Update new address of data
    EthBuffer[0] = (uint8_t)(DestPtr >> 8);
    EthBuffer[1] = (uint8_t)DestPtr;
    EthReg(Socket, 0, Sn_TX_WR, 2, EthBuffer, 1);
    EthBuffer[0] = SEND; // Complete transmit process
    EthReg(Socket, 0, Sn_CR, 1, EthBuffer, 1);
    status = 4;
    // SocketClose(Socket);
  }
  return (status);
}

// Close socket
uint8_t SocketClose(uint8_t Socket)
{
  uint8_t status;

  EthBuffer[0] = CLOSE;
  EthReg(Socket, 0, Sn_CR, 1, EthBuffer, 1);
  status = 0;
  return (status);
}

// Read or write to W5500 registers
void EthReg(uint8_t Socket, uint8_t Register, uint16_t Address, uint16_t Size, uint8_t *Data, uint8_t ReadWrite)
{
  uint8_t Control;
  uint8_t AddCon[3];

  if (Socket == 0) // Selects common register
    Control = 0;
  else // Accesing socket registers 1 to 8
  {
    Control = (Socket << 2) - 3 + Register; // Register = 0 - Socket register, Register = 1 - TX Buffer, Register = 2 - RX Buffer
    Control = Control << 3;
  }
  if (ReadWrite == 1) // 0 - Read, 1 - Write
    Control = Control | 0x04;
  Pin_Low(PESS);                         // Enable chip select
  AddCon[0] = ((uint8_t)(Address >> 8)); // Address
  AddCon[1] = ((uint8_t)(Address));
  AddCon[2] = (Control); // Write Control word
  EthSPI_Write(AddCon, 3);
  if (ReadWrite == 0) // Read data
    EthSPI_Read(Data, Size);
  else // Write data
    EthSPI_Write(Data, Size);
  Pin_High(PESS); // Disable chip select
}