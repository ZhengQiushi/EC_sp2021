#include "stm32f1xx_hal.h"
#define CODE0 0xC0
#define CODE1 0xF8
#define LED_COUNT_NUM  3
#define LED_COUNT_SET  5


typedef union {
	uint32_t raw_value;
	uint8_t channel[4];
}WS2812CtlType;

extern WS2812CtlType WS2812ColorCtlValue[LED_COUNT_NUM+LED_COUNT_SET];

uint8_t WS2812SetColor(uint8_t index,uint8_t r,uint8_t g,uint8_t b);
void RGB_LED_Write_Byte(uint32_t byte);
void RGB_LedsArrayTransmit();
