# TASK_TimerInit()

##### 函数描述

​	总初始化函数

##### 函数类型

​	int

##### 输入

​	无

##### 输出

​	无

##### 函数流程

​	开启系统时钟SYSTIMER的RCC

​	初始化时钟，频率为1000Hz(Period 1ms)

​	时钟中断配置，开启NVIC终端，开启定时器

​	注册定时器回调函数([spIRQ.registe(a,b,c)](./sp_irq.md))

​		注：自编代码库对单一中断难以执行多个函数的情况进行了改进，通过加入回调函数池，使得一个中断可以以触发多个回调函数。

##### 代码笔记

​	这段代码本身问题不大，但是其中涉及的重复代码(以下spSYSTIMER_IRQn与spCLOCKTIMER_IRQn实际意义一致)

```c
#define spCLOCKTIMER                            spSYSTIMER
#define spRCC_Set_CLOCKTIMER                    spRCC_Set_SYSTIMER
#define spCLOCKTIMER_IRQn                       spSYSTIMER_IRQn
```

```c
NVIC_IRQEnable(spSYSTIMER_IRQn, 0, 0);
NVIC_IRQEnable(spCLOCKTIMER_IRQn, 0, 0);
```

  	及对时钟的选择

```
#if defined(sp_choose_clock)
    #if sp_choose_clock==1
        #define spSYSTIMER                      TIM13
        #define spRCC_Set_SYSTIMER              spRCC_Set_TIM13
        #define spSYSTIMER_IRQn                 TIM8_UP_TIM13_IRQHandler
    #elif sp_choose_clock==2
        #define spSYSTIMER                      TIM14
        #define spRCC_Set_SYSTIMER              spRCC_Set_TIM14
        #define spSYSTIMER_IRQn                 TIM8_TRG_COM_TIM14_IRQn
    #endif
#else
    #define spSYSTIMER                          TIM14
    #define spRCC_Set_SYSTIMER                  spRCC_Set_TIM14
    #define spSYSTIMER_IRQn                     TIM8_TRG_COM_TIM14_IRQn
#endif
```

​	这些操作的目的仍然令人疑惑

