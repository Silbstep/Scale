#ifndef __Ethernet_h
#define __Ethernet_h

#include "Global.h"
#include "InOut.h"
#include "Structures.h"
#include "esp_log.h"
#include "EthSPI.h"
#include "esp_mac.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"

// Global variables
extern uint8_t EthTimeOut;
extern uint8_t EthBuffer[10]; // Data to be written to and read from ethernet
extern uint8_t EthData[ETHBUFFER];
extern uint16_t LenRX[9];
extern uint16_t SourcePtr;
extern uint16_t LenTX[9];
extern uint16_t DestPtr;

// Initialise Ethernet
uint8_t EthInit(void);
// Configure Ethernet IC once a link is up
uint8_t EthConfigure(uint8_t *EthernetStructure);
// Initialise and open socket
uint8_t SocketInit(uint8_t Socket, uint8_t Protocol, uint8_t *EthernetStructure);
// Make socket a server
uint8_t SocketServer(uint8_t Socket);
// Make socket a client
uint8_t SocketClient(uint8_t Socket, uint8_t *EthernetStructure);
// Establish socket, wait until a SYN (server) or ACK SYN (client) packet is received
uint8_t SocketEstablish(uint8_t Socket);
// Check if data has been received
uint8_t SocketCheckRX(uint8_t Socket, uint8_t Type);
// Receive data and put into EthData
uint16_t SocketRX(uint8_t Socket, uint8_t *EthernetStructure);
// Transmit data from EthData
uint8_t SocketTX(uint8_t Socket, uint8_t Protocol, uint8_t *EthernetStructure);
// Close socket
uint8_t SocketClose(uint8_t Socket);
// Read or write to W5500 registers
void EthReg(uint8_t Socket, uint8_t Register, uint16_t Address, uint16_t Size, uint8_t *Data, uint8_t ReadWrite);

#endif