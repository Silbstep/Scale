#include "InOut.h"

// GPIO functions
void Pin_High(int Pin)
{
  REG_WRITE(GPIO_OUT_W1TS_REG, Pin);
}

void Pin_High1(int Pin)
{
  REG_WRITE(GPIO_OUT1_W1TS_REG, Pin);
}

void Pin_Low(int Pin)
{
  REG_WRITE(GPIO_OUT_W1TC_REG, Pin);
}

void Pin_Low1(int Pin)
{
  REG_WRITE(GPIO_OUT1_W1TC_REG, Pin);
}

void Pin_Level(int Pin, bool Level)
{
  if (Level != 0)
    REG_WRITE(GPIO_OUT_W1TS_REG, Pin);
  else
    REG_WRITE(GPIO_OUT_W1TC_REG, Pin);
}

void Pin_PullUp(gpio_num_t Pin)
{
  gpio_set_pull_mode(Pin, GPIO_PULLUP_ONLY);
}

void Pin_PullDown(gpio_num_t Pin)
{
  gpio_set_pull_mode(Pin, GPIO_PULLDOWN_ONLY);
}

void Pin_Init(gpio_num_t Pin, gpio_mode_t Direction, uint32_t Level)
{
  gpio_reset_pin(Pin);
  gpio_set_direction(Pin, Direction);
  if (Direction == GPIO_MODE_OUTPUT)
    gpio_set_level(Pin, Level);
}
