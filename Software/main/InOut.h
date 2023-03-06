#ifndef __InOut_h
#define __InOut_h

#include "driver/gpio.h"

// Function prototypes
void Pin_High(int Pin);
void Pin_High1(int Pin);
void Pin_Low(int Pin);
void Pin_Low1(int Pin);
void Pin_Level(int Pin, bool Level);
void Pin_PullUp(gpio_num_t Pin);
void Pin_PullDown(gpio_num_t Pin);
void Pin_Init(gpio_num_t Pin, gpio_mode_t Direction, uint32_t Level);

#endif