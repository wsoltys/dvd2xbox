#pragma once
#include "LED.h"

// LED settings
#define LED_COLOUR_NO_CHANGE		0
#define LED_COLOUR_GREEN			1
#define LED_COLOUR_ORANGE			2
#define LED_COLOUR_RED				3
#define LED_COLOUR_CYCLE			4
#define LED_COLOUR_OFF				5

class ILED
{
public:
  static void CLEDControl(int ixLED);
};