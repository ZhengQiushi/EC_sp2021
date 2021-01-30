#ifndef _TIMER_H
#define _TIMER_H
#include "stm32f4xx.h"

//////////////////////////////////////////////////////////////////////////////////	 
extern u8 flag_Ready;
void TIM6_Init(void);
extern u16 TIM6_time_count;
extern u16 TIM6_time_count_1;



#endif
