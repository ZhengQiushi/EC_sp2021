////一般SPI速率控制在6.5MHz就能达到每个字节1.25us左右
#include "WS2812.h"
#include "stm32f1xx_hal.h"
#include "spi.h"

uint8_t LedsArray [(LED_COUNT_NUM+LED_COUNT_SET) * 24];\
int16_t LedsDataCount = 0;
WS2812CtlType WS2812ColorCtlValue[LED_COUNT_NUM+LED_COUNT_SET];

uint8_t WS2812SetColor(uint8_t index,uint8_t r,uint8_t g,uint8_t b)
{
	if(index>=LED_COUNT_NUM)
		return 0;
	r= r>=0xFF ? 0xFF:r;
	g= g>=0xFF ? 0xFF:g;
	b= b>=0xFF ? 0xFF:b;

	WS2812ColorCtlValue[index].channel[0]=b;
	WS2812ColorCtlValue[index].channel[1]=r;
	WS2812ColorCtlValue[index].channel[2]=g;
	return 1;
}
void RGB_LED_Write_Byte(uint32_t byte)
{
	for(uint8_t i=0;i<24;i++)
		{
			if(byte&0x1000000)
				{
				LedsArray[LedsDataCount++] = CODE1;
			}
			else
				{
				LedsArray[LedsDataCount++] = CODE0;
			}
		byte <<= 1;
	}
}


void RGB_LedsArrayTransmit()
{
	uint8_t i;

	for(i=0;i<LED_COUNT_NUM;i++)
		RGB_LED_Write_Byte(WS2812ColorCtlValue[i].raw_value);

	for(i=LedsDataCount;i<LedsDataCount+LED_COUNT_SET*24;i++)
		LedsArray[i] = 0;

	if(HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_READY)
		HAL_SPI_Transmit_DMA(&hspi3, LedsArray, LedsDataCount+LED_COUNT_SET*24);
	LedsDataCount = 0;

}




