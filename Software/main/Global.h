#ifndef __Global_h
#define __Global_h

// AD5621
// Outputs
#define AOMOSI (gpio_num_t)21
#define AOCLK (gpio_num_t)47
#define AOSS (gpio_num_t)48
#define PAOMOSI BIT21
#define PAOCLK BIT47
#define PAOSS BIT48
// Values
#define A420FULL 0x3FFF
#define A420ZERO 0

// AD7190
// Outputs
#define AMOSI (gpio_num_t)14
#define ACLK (gpio_num_t)13
#define ASS (gpio_num_t)12
#define PAMOSI BIT14
#define PACLK BIT13
#define PASS BIT12
// Inputs
#define AMISO (gpio_num_t)11
// Mode register
#define CONTINOUS 0x00
#define SINGLE 0x20
#define IDLE 0x40
#define POWERDOWN 0x60
#define CALIBZSINT 0x80
#define CALIBFSINT 0xA0
#define CALIBZSSYS 0xC0
#define CALIBFSSYS 0xE0
#define STATUSTRANSON 0x10
#define STATUSTRANSOFF 0xEF
#define EXTCRYSTAL 0x00
#define EXTCLOCK 0x04
#define INTCLOCKTRI 0x08
#define INTCLOCK 0x0C
#define SINC3 0x80
#define SINC4 0x7F
#define PARITYEN 0x20
#define PARITYDIS 0xDF
#define SINGLEEN 0x08
#define SINGLEDIS 0xF7
#define REJ60 0x04
#define REJ50 0xFB
// Config register
#define CHOPON 0x80
#define CHOPOFF 0x7F
#define REFSEL1 0xEF
#define REFSEL2 0x10
#define CHANNEL0 0x01
#define CHANNEL1 0x02
#define CHANNEL2 0x04
#define CHANNEL3 0x08
#define CHANNEL4 0x10
#define CHANNEL5 0x20
#define CHANNEL6 0x40
#define CHANNEL7 0x80
#define BURNON 0x80
#define BURNOFF 0x7F
#define REFDETON 0x40
#define REFDETOFF 0xBF
#define BUFFERON 0x10
#define BUFFEROFF 0xEF
#define UNIPOLAR 0x08
#define BIPOLAR 0xF7
#define GAIN1 0xF8
#define GAIN8 0x03
#define GAIN16 0x04
#define GAIN32 0x05
#define GAIN64 0x06
#define GAIN128 0x07
// Ethernet
// Timeout
#define ETHERNETRESET 1800
// Outputs
#define ERST (gpio_num_t)7
#define PERST BIT7
#define ESS (gpio_num_t)8
#define PESS BIT8
#define ECLK (gpio_num_t)18
#define EMOSI (gpio_num_t)16
#define PIN_NUM_MOSI 16
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 8
// Inputs
#define EMISO (gpio_num_t)17
#define PIN_NUM_MISO 17
// SPI
#define LCD_HOST SPI2_HOST
// Receive and transmit buffer
#define ETHBUFFER 1024
// W5500 registers
#define SERVER 0
#define CLIENT 0xFF
#define WMR 0x0000
#define GATEWAY 0x0001
#define SUBNET 0x0005
#define MAC 0x0009
#define IP 0x000F
#define RCR 0x001B
#define RTR 0x0019
#define PHYCFGR 0x002E
// Socket registers
#define Sn_MR 0x0000
#define Sn_CR 0x0001
#define Sn_IR 0x0002
#define Sn_SR 0x0003
#define Sn_PORT 0x0004
#define Sn_DIPR 0x000C
#define Sn_DPORT 0x0010
#define Sn_PROTO 0x0014
#define Sn_TX_FSR 0x0020
#define Sn_TX_WR 0x0024
#define Sn_RX_RSR 0x0026
#define Sn_RX_RD 0x0028
#define Sn_IMR 0x002C
#define Sn_KPALVTR 0x002F
// W5500 Register contents
#define OPEN 0x01
#define LISTEN 0x02
#define CONNECT 0x04
#define DISCON 0x08
#define CLOSE 0x10
#define SEND 0x20
#define RECV 0x40
#define SOCK_CLOSED 0x00
#define SOCK_INIT 0x13
#define SOCK_TCP 0x13
#define SOCK_LISTEN 0x14
#define SOCK_SYNSENT 0x15
#define SOCK_SYNRECV 0x16
#define SOCK_ESTABLISHED 0x17
#define SOCK_CLOSE_WAIT 0x1C
#define SOCK_UDP 0x22
#define SOCK_IPRAW 0x32
#define SOCK_MACRAW 0x42
#define READ 0x00
#define WRITE 0x01
#define ICMP 0x01

// RS485
// Pins
#define RS485TX (GPIO_NUM_38)
#define RS485RX (GPIO_NUM_37)
#define RS485RTS (GPIO_NUM_39)
// Definitions
#define BUF_SIZE (127)
#define PORTNUMBER 1
#define PACKET_READ_TICS (100 / portTICK_RATE_MS)

// FRAM (I2C)
// Size
#define FRAMSIZE 8192
// Bi-Directional
#define I2C_MASTER_SDA_IO 1
#define I2C_MASTER_SCL_IO 2
// Parameters
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_TX_BUF_DISABLE 0 // I2C master doesn't need buffer
#define I2C_MASTER_RX_BUF_DISABLE 0 // I2C master doesn't need buffer
#define I2C_MASTER_TIMEOUT_MS 500
#define I2C_MASTER_NUMBER 0
#define FRAM_ADDRESS 0x50
// Memory location lengths
#define ETHSTRUCTL 48
#define ETHSIPL 4
#define ETHSSUBL 4
#define ETHSGATEL 4
#define ETHDIPL 4
#define ETHSPORTL 2
#define ETHDPORT1L 2
#define ETHDPORT2L 2
#define ETHDPORT3L 2
#define ETHDPORT4L 2
#define ETHDPORT5L 2
#define ETHDPORT6L 2
#define ADMODEL 12
#define ADRATEL 4
#define ADGAINL 1
#define CALIBCOUNTL 1
#define ZEROTARGETL 1
#define ZEROTYPEL 1
#define DUMMY3L 3
#define CALIBZEROL 4
#define CALIBFULLL 4
#define CALIBWEIGHTL 4
#define CALIBRATIOL 4
#define ADCONFIGL 8
#define STARTTRIGGERL 1
#define STOPTRIGGERL 1
#define TRANSZEROL 1
#define TRANSREADL 1
#define STOPTIMEL 2
#define SKIPL 1
#define WEIGHTVARL 1
#define WEIGHTCOUNTL 1
#define PREFIXL 9
#define SUFFIXL 9
#define ACALIBZEROL 2
#define ACALIBFULLL 2
#define ACALIBWEIGHTL 4
#define ACALIBRATIOL 4
#define DATABITSL 1
#define PARITYL 1
#define STOPBITSL 1
#define FLOWCONTROLL 1
#define RS485MODEL 1
#define BAUDRATEL 4
// Memory locations
enum
{
  ETHSTRUCT = 0,
  ETHSIP = ETHSTRUCT,
  ETHSSUB = ETHSIP + ETHSIPL,
  ETHSGATE = ETHSSUB + ETHSSUBL,
  ETHDIP = ETHSGATE + ETHSGATEL,
  ETHSPORT = ETHDIP + ETHDIPL,
  ETHDPORT1 = ETHSPORT + ETHSPORTL,
  ETHDPORT2 = ETHDPORT1 + ETHDPORT1L,
  ETHDPORT3 = ETHDPORT2 + ETHDPORT2L,
  ETHDPORT4 = ETHDPORT3 + ETHDPORT3L,
  ETHDPORT5 = ETHDPORT4 + ETHDPORT4L,
  ETHDPORT6 = ETHDPORT5 + ETHDPORT5L,
  ETHTYPE = ETHDPORT6 + ETHDPORT6L,
};
enum
{
  ADBASE = ETHSTRUCT + ETHSTRUCTL,
  ADMODE,
  ADSTRANS,
  ADCLOCK,
  ADSINC,
  ADPARITY,
  ADSINGLE,
  ADREJ,
  ADRATE,
};
enum
{
  ADCONFIG = ADRATE + ADRATEL,
  ADCHOP,
  ADREFSEL,
  ADCHANNEL,
  ADBURN,
  ADREFDET,
  ADBUFFER,
  ADPOLARITY,
  ADGAIN,
};
enum
{
  CALIBCOUNT = ADGAIN + ADGAINL,
  ZEROTARGET = CALIBCOUNT + CALIBCOUNTL,
  ZEROTYPE = ZEROTARGET + ZEROTARGETL,
  SKIP = ZEROTYPE + ZEROTYPEL,
  CALIBZERO = SKIP + SKIPL,
  CALIBFULL = CALIBZERO + CALIBZEROL,
  CALIBWEIGHT = CALIBFULL + CALIBFULLL,
  CALIBRATIO = CALIBWEIGHT + CALIBWEIGHTL,
  STARTTRIGGER = CALIBRATIO + CALIBRATIOL,
  STOPTRIGGER = STARTTRIGGER + STARTTRIGGERL,
  TRANSZERO = STOPTRIGGER + STOPTRIGGERL,
  TRANSREAD = TRANSZERO + TRANSZEROL,
  STOPTIME = TRANSREAD + TRANSREADL,
  WEIGHTVAR = STOPTIME + STOPTIMEL,
  WEIGHTCOUNT = WEIGHTVAR + WEIGHTVARL,
  PREFIX = WEIGHTCOUNT + WEIGHTCOUNTL,
  SUFFIX = PREFIX + PREFIXL,
};
enum
{
  ACALIBZERO = SUFFIX + SUFFIXL,
  ACALIBFULL = ACALIBZERO + ACALIBZEROL,
  ACALIBWEIGHT = ACALIBFULL + ACALIBFULLL,
  ACALIBRATIO = ACALIBWEIGHT + ACALIBWEIGHTL,
};
enum
{
  DATABITS = ACALIBRATIO + ACALIBRATIOL,
  PARITY,
  STOPBITS,
  FLOWCONTROL,
  RS485MODE,
  BAUDRATE,
};
// Memory location update registers Make sure that STARTU is greater than last enum + last enum length
enum
{
  STARTU = 150,
  ETHSTRUCTU,
  ETHSIPU,
  ETHSSUBU,
  ETHSGATEU,
  ETHDIPU,
  ETHSPORTU,
  ETHDPORT1U,
  ETHDPORT2U,
  ETHDPORT3U,
  ETHDPORT4U,
  ETHDPORT5U,
  ETHDPORT6U,
  ETHTYPEU,
  ADMODEU,
  ADSTRANSU,
  ADCLOCKU,
  ADSINCU,
  ADPARITYU,
  ADSINGLEU,
  ADREJU,
  ADRATEU,
  ADCONFIGU,
  ADCHOPU,
  ADREFSELU,
  ADCHANNELU,
  ADBURNU,
  ADREFDETU,
  ADBUFFERU,
  ADPOLARITYU,
  ADGAINU,
  CALIBCOUNTU,
  ZEROTARGETU,
  ZEROTYPEU,
  CALIBZEROU,
  CALIBFULLU,
  CALIBWEIGHTU,
  CALIBRATIOU,
  STARTTRIGGERU,
  STOPTRIGGERU,
  TRANSZEROU,
  TRANSREADU,
  STOPTIMEU,
  SKIPU,
  PREFIXU,
  SUFFIXU,
  WEIGHTVARU,
  WEIGHTCOUNTU,
  ACALIBZEROU,
  ACALIBFULLU,
  ACALIBWEIGHTU,
  ACALIBRATIOU,
  DATABITSU,
  PARITYU,
  STOPBITSU,
  FLOWCONTROLU,
  RS485MODEU,
  BAUDRATEU,
  ENDU
};

// LED
// Outputs
#define LEDG (gpio_num_t)20
#define LEDB (gpio_num_t)19
#define LEDR (gpio_num_t)9
#define PLEDG BIT20
#define PLEDB BIT19
#define PLEDR BIT9

// Inputs
#define DEBOUNCETIME 5 // Gives a 50mS debounce time
// Opto Coupler
#define IN1 (gpio_num_t)40
#define IN2 (gpio_num_t)41
#define IN3 (gpio_num_t)42
#define INP1 1
#define INP2 2
#define INP3 4
// Switch
#define SWITCH (gpio_num_t)10
#define INP4 8

#endif