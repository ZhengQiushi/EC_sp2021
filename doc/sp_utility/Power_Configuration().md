# Power_Configuration()

#### 函数作用

​	

#### 函数类型

​	void

#### 输入

​	无

#### 输出

​	无

#### 函数流程

	#if defined(SP_USING_BOARD_TYPEA)
	spRCC_Set_GPIOH();
	spGPIO.output_config(GPIOH, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5, 
	    GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	GPIO_SetBits(GPIOH,GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	#endif
​	使用A板的情况下([SP_USING_BOARD_TYPEA](../Pre-define.md))

​		开CPIOH的RCC时钟，H2,H3,H4,H5以推挽上拉电阻100MHz速度初始化并置位

​	否则什么都不做

#### 笔记

