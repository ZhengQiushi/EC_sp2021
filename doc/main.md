# main.c

### 头文件引用

```c
#include <stdint.h>
#include <stdio.h>

#include "sp_conf.h"
#include "sp_utility.h"
#include "sp_imu.h"
#include "gimbal.h"
```

### 函数列表

#### main()

##### 函数描述

​	总初始化函数

##### 函数类型

​	int

##### 输入

​	无

##### 输出

​	无

##### 函数流程

​	关闭所有中断([spPortEnterCritical()](sp_task.md))

​		注：该代码使用汇编混合编程方式关闭CMSIS内核的中断，同时对计数变量uxCriticalNesting自增1，即记录进入未退出的关闭中断请求次数。

​	设置NVIC优先级为4位抢占优先级

​	配置电源([Power_Configuration()](./))

​	开随机数生成器的RCC并开启随机数生成器(spRCC_Set_RNG(); RNG_Cmd(ENABLE);)

​	任务全局初始化([TASK_GlobalInit()](./sp_task/TASK_GlobalInit().md))

​	任务定时器初始化([TASK_TimerInit()](./sp_task/TASK_TimerInit().md))

​	根据版型([BOARD_MODE](Pre-define.md))使用蜂鸣器播放音乐

​	点亮红灯，绿灯([LED_G_ON();LED_R_OFF();](sp_utility.md))

​	开始任务循环([TASK_Start()](./sp_task/TASK_Start().md))

​	开启所有中断([spPortExitCritical()](sp_task.md))

​		注：该代码对计数变量uxCriticalNesting自减1，当记录进入未退出的关闭中断请求次数清零时，使用汇编混合编程方式开启CMSIS内核的中断。

​	