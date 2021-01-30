#include <stm32f4xx.h>
#include "main.h"



int main(void)
{
		BSP_Init();            //ALL Init
	while(1)
	{
		M_pid();  		//PID Control
		//Elc_Push();
	}
}


