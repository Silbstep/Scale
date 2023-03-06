#ifndef __Structures_h
#define __Structures_h

// Mode commands
// AMWMO-X#       Where X is 0 - Continous, 1 - Single, 2 - Idle, 3 - Power Down
// AMWST-X#       Where X is 0 - off, 1 - on
// AMWCL-X#       Where X is 0 - external crystal, 1 - external clock, 2 - internal clock (tristate), 3 - internal clock
// AMWSI-X#       Where X is 3 - sinc3, 4 - sinc4
// AMWPA-X#       Where X is 0 - Disable, 1 - Enable
// AMWSN-X#       Where X is 0 - Disable, 1 - Enable
// AMWRE-X#       Where X is 50 - REJ50, 60 - REJ60
// AMWRA-X#       Where X is a floating number from 4.69Hz to 4800.00Hz
struct AD7190Mode
{
  uint8_t Base;
  uint8_t Mode;
  uint8_t StatusTrans;
  uint8_t Clock;
  uint8_t Sinc;
  uint8_t Parity;
  uint8_t Single;
  uint8_t Rej;
  float Rate;
};
// Config commands
// ACWCP-X#      Where X is 0 - chop off, 1 - chop on
// ACWRS-X#      Where x is 1 - Refsel1, 2 - Refsel2
// ACWCH-X#      Where x is 0 - channel 0 to 7 - channel 7
// ACWBU-X#      Where x is 0 - burn off, 1 - burn on
// ACWRD-X#      Where x is 0 - reference detect off, 1 - reference detect on
// ACWBF-X#      Where x is 0 - buffer off, 1 - buffer on
// ACWPO-X#      Where x is 0 - unipolar, 1 - bipolar
// ACWGA-X#      Where x is 1- gain1, 8 - gain8, 16 - gain16, 32 - gain32, 64 - gain64, 128 - gain128

struct AD7190Config
{
  uint8_t Chop;
  uint8_t Refsel;
  uint8_t Channel;
  uint8_t Burn;
  uint8_t RefDet;
  uint8_t Buffer;
  uint8_t Polarity;
  uint8_t Gain;
};

// Scale commands
// ASWZC#       Zero calibrate: Calibrate CalibZero with average over CalibCount amount of times
// ASWFC#       Full calibrate: Calibrate CalibFull with average over CalibCount amount of times
// ASWSS-X#     Software start weighing. 0 - No OK reply, 1 - OK reply
// ASWSP=X#     Software stop weighing. 0 - No OK reply, 1 - OK reply
// ASWSK-X#     Skip: 0 - No skip, 1 - Skip first scale reading
// ASWZT-X#     Zero: trigger: Readings below this value are subject to zeroing
// ASWZY-X#     Zero type: 0 - no zero, 1 - AutoZero, 2 - InputZeroL (Input 3 goes low) 3 - InputZeroH (Input 3 goes high)
// ASWCC-X#     Calibration counter: Calib count can be from 1 to 255
// ASWCW-X#     Calibration weight: In grams, can be from 1 to 1000000000
// ASWTS-X#     Start trigger: 0 - Freeflow, 1 - Weight on, 2 - Input1 LH, 3 - Input1 HL, 4 - Software
// ASWTP-X#     Stop trigger: 0 - Weight stable, 1 - Time, 2 - Input1 HL, 3 - Input1 LH, 4 - Input2 HL, 5 - Input2 LH, 6 - Software
// ASWTZ-X#     Transzero: 0 - Nothing on 0, 1 - Freeflow on 0
// ASWTR-X#     TransRead: : 0 - continous, else transmits number of times as in register
// ASWST-X#     Stop Time: Stops measuring this time after trigger if ASWTP is a 1 (Time). Time is in mS.
// ASWWV-X#     Weight variance: If the difference between WeightCount readings in Start trigger weight on mode is less that this value then stop weighing
// ASWWC-X#     Weight count: number of counts to compare weight variance (from 2 to 99)
// ASWPR-X#     Prefix: X is hex value (in ascii) for ascii character. Up to 8 characters
// ASWSU-X#     Suffix: X is hex value (in ascii) for ascii character. Up to 8 characters

// Scale values
struct ScaleValues
{
  uint8_t CalibCount; // Calib count can be from 1 to 255
  uint8_t ZeroTarget; // Readings below this value are subject to zeroing
  uint8_t ZeroType;   // 0 - no zero, 1 - AutoZero, 2 - InputZeroL (Input 3 goes low) 3 - InputZeroH (Input 3 goes high)
  uint8_t Skip;
  uint32_t CalibZero;   // Zero calibration amount
  uint32_t CalibFull;   // Full calibration amount
  uint32_t CalibWeight; // Calibration weight in grams, can be from 1 to 1000000000
  float CalibRatio;     // Calibration ratio
  uint8_t StartTrigger;
  uint8_t StopTrigger;
  uint8_t TransZero;
  uint8_t TransRead;
  uint8_t WeightVar;
  uint8_t WeightCount;
  uint16_t StopTime;
  char Prefix[9];
  char Suffix[9];
};

// AD5621 values
// A4WZC#       Zero calibrate: can then send A4WCR command to move current up or down to reach 4mA.
// A4WFC#       Full calibrate: can then send A4WCR command to move current up or down to reach 20mA.
// A4WCH-X#     Change current: D go down, U go up. Steps are in 1uA.
// A4WSC#       Stops calibrating and stores results.
// A4WMW-X#     Maximum weight that will be measured. This weight will give an output of 20mA. No weight will give an output of 4mA.
// A4RRA        Read ratio

struct A420Values
{
  uint16_t CalibZero;   // Value of DAC that gives 0mA
  uint16_t CalibFull;   // Value of DAC that gives 20mA
  uint32_t CalibWeight; // Weight represented by CalibFull
  float CalibRatio;     // CalibRatio = (CalibFull - CalibZero) / CalibWeight. DAC output = ratio * Weight + CalibZero.
  uint16_t CalibValue;
};

// Commands
// ESIP-192.168.1.10#
// ESSN-255.255.255.0#
// ESGW-192.168.1.1#
// EDIP-192.168.1.5#
// ESPO-3196#
// EDPO-5000#
// ETYP-X#       Where X is 0 - Server, 1 - Client
struct EthernetValues
{
  uint8_t SelfIp[4];
  uint8_t SelfSubnet[4];
  uint8_t SelfGateway[4];
  uint8_t DestinationIp[4];
  uint8_t SelfPort[2];
  uint8_t DestPort[6][2];
  uint8_t Type;
  uint8_t Dummy1;
  char Prefix[8];
  char Suffix[8];
};

// Uart values
struct UARTValues
{
  uint8_t DataBits;
  uint8_t Parity;
  uint8_t StopBits;
  uint8_t FlowControl;
  uint8_t Mode;
  uint32_t BaudRate;
};

#endif