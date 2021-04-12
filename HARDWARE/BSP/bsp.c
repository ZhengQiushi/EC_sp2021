#include "main.h"
LPF_FirstOrder_type filter_206;
LPF_FirstOrder_type filter_205;
LPF_FirstOrder_type filter_207;
float gx_offset=0.0f,gy_offset=0.0f,gz_offset=0.0f,ax_offset=0.0f,ay_offset=0.0f,az_offset=0.0f;
float gx_offset_fin=0.0f,gy_offset_fin=0.0f,gz_offset_fin=0.0f;
void BSP_Init(void)	//注意初始化的顺序
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(180);
	Buzzer_Init();
	Led_Configuration();
	Power_Configuration();
	MiniPC_Configuration();
	LPF_FirstOrder_Init(&filter_206,10,1000);//10 1000
	LPF_FirstOrder_Init(&filter_205,30,1000);	
	LPF_FirstOrder_Init(&filter_207,30,1000);
	
	SPI4Init();
	SPI5Init();
	ADIS16470_Init();                                           //陀螺仪初始化
	MPU6500_Init();                                             //MPU初始化
	delay_ms(3000);
	init_euler();
	#ifdef AutoAim_USB
		USB_TaskInit();
	#endif
	USART6_Configuration(115200); 
	UART8_Init();
	
	USART6_DMA_Init();
	USART2_DMA_Init();
	USART3_DMA_Init();
	
	
	TIM9_Init(); //duoji
	TIM2_Init();  
	RC_Init();
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_2tq,CAN_BS1_6tq,5,CAN_Mode_Normal);	
	CAN2_Mode_Init(CAN_SJW_1tq,CAN_BS2_2tq,CAN_BS1_6tq,5,CAN_Mode_Normal);

	WholeInitTask();	
	delay_ms(1000);
	TIM6_Init();

	Laser_Configuration();
	for (int i=0;i<1500;i++)
	{			
		MPU6500_Read();
		gx_offset+=mpu6500_real_data.Gyro_X;
		gy_offset+=mpu6500_real_data.Gyro_Y;
		gz_offset+=mpu6500_real_data.Gyro_Z;
	}
	gx_offset_fin=gx_offset/1500;
	gy_offset_fin=gy_offset/1500;
	gz_offset_fin=gz_offset/1500;	
	Buzzer_toggle();	
	
}




