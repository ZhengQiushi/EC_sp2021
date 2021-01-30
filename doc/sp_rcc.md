# sp_rcc

#### 代码笔记

​	代码并没有对Std库的RCC函数进行大改，其内容主要有以下两个部分

​	第一，化简RCC外设时钟名称，如

```c++
#define RCC_GPIOF             RCC_AHB1Periph_GPIOF
```

​	这种改动主要删去了外设所属的时钟线路，即AHB1,AHB2,APB1,APB2

​	并非很好的化简，因为不同外设所处的时钟线路分频具有相当程度的差异，需要对照时钟图进行一定程度的了解，不过如果使用stm32cube则另当别论

​	第二，定义RCC外设时钟开启的化简函数

```c
#define spRCC_Set_GPIOG() \
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE)
```
​	Set即置位，置为高电平，开启等含义，之所以没有Reset，DISABLE部分的代码，是因为通常不会出现频繁开关外设时钟来控制外设的情况，因此任何需要用到的外设自程序开始运转起就不会关闭RCC时钟。

```c
//#define spRCC_Set_GPIOx(gpio) \
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_#gpio, ENABLE)
```

​	需要注意的是，代码中出现了这样一段被注释的代码，或许是为了简化GPIO的RCC开启代码，对于这些代码仍需进一步的了解和研究。

​	另，文件本身包含英文Doxygen注释，可以通过工具生成注释帮助文件

#### 包含文件 

​	[sp_rcc.h](../SPLib/BSP/sp_rcc.h)