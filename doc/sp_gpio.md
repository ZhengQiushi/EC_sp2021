# sp_gpio

#### 包含文件

​	[sp_gpio.h](../SPLib/BSP/sp_gpio.h)

​	[sp_gpio.c](../SPLib/BSP/sp_gpio.c)

#### 代码笔记

​	自建库对标准库仅做了一定程度的修改和化简，这一思路大量出现在各处代码之中

​	比较典型的是，该gpio库将部分寄存器中的位直接引出define，故可以通过直接对IDR输入数据寄存器和ODR输出数据寄存器进行写入和读取。另外，将一些常用配置封装成为固定函数，并将大量

​	代码实际上仍然具有一定的修改空间，例如以GPIO_Config(...)函数为核心，构建剩余的GPIO_IN_Config(...)等函数，实现函数嵌套的效果。

​	代码有一段很有趣，

```c
/*********from sp_gpio.c*********/
#ifdef spRCC_Set_GPIOA
if(GPIOx==GPIOA) {
    spRCC_Set_GPIOA();
}
#endif

/*********from sp_rcc.h*********/
#define spRCC_Set_GPIOA() \
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
```
​	可以发现，sp_rcc.h中的预定义为spRCC_Set_GPIOA()，但是实际#ifdef spRCC_Set_GPIOA也可以得到真，这样的原因仍需进一步查找资料。

​	另外，这个库实际上没有存在的必要，多写几个参数比直接定义一个莫名其妙的函数有用多了，这样的代码能够自注释，而不需要回到库中查看源码。同时，这个库仍然是基于Std库进行的进一步扩展，而非完全的重写。

#### 函数列表

#### 位于[sp_gpio.c](../SPLib/BSP/sp_gpio.c)中的函数

​	由于函数具有高度的类同性，采用简略的写法完成

​	基本上都是开启RCC，构建GPIO_InitStructure并把该填入的都填进去，然后初始化

​	只是根据函数的用途有些属性在函数内填写

```c
void GPIO_Config(GPIO_TypeDef* GPIOx, uint16_t Pinx, GPIOMode_TypeDef Modex, 
    GPIOOType_TypeDef OTyperx, GPIOPuPd_TypeDef PuPdx, GPIOSpeed_TypeDef Speedx) 
```

构建GPIO_InitStructure并把该填入的都填进去

```c
void GPIO_OUT_Config(GPIO_TypeDef* GPIOx, uint16_t Pinx, GPIOOType_TypeDef OTyperx, 
    GPIOPuPd_TypeDef PuPdx, GPIOSpeed_TypeDef Speedx)
```

模式为输出模式，其他的照常填入，常用于输出数字逻辑高低电平的Pin

```c
void GPIO_IN_Config(GPIO_TypeDef* GPIOx, uint16_t Pinx, GPIOPuPd_TypeDef PuPdx, GPIOSpeed_TypeDef Speedx)
```

推挽(PP,Push-Pull)输入模式，另外，GPIOOType_TypeDef包含推挽(PP)和开漏(OD)两种方式，参照数电课程的说法，后者需要外接上拉电阻(也可能初始化时上拉电阻即可，这个还需要查资料)才能使用，但是电流更大

常用于输入逻辑电平的针脚Pin

```c
void GPIO_AF_Config(GPIO_TypeDef* GPIOx, uint16_t Pinx, 
        GPIOOType_TypeDef OTyperx, GPIOPuPd_TypeDef PuPdx, GPIOSpeed_TypeDef Speedx)
```

模式为复用(AF,alternate-function)模式，其他的照常填入，常用于外设特殊功能的针脚

```c
void GPIO_AN_Config(GPIO_TypeDef* GPIOx, uint16_t Pinx)
```

模拟电平模式，开RCC，使用无上下拉电阻，模拟电平模式

可能由于模拟电平模式下不需要考虑输出类型(PP/OD)和输出速度(xMHz)，所以并没有对这两个属性进行赋值

```c
void GPIO_SetMode(GPIO_TypeDef* GPIOx, uint16_t Pinx, GPIOMode_TypeDef mode) {
    GPIOx->MODER  &= ~(GPIO_MODER_MODER0 << (Pinx * 2));
    GPIOx->MODER |= (((uint32_t)mode) << (Pinx * 2));
}
```

没什么用的修改GPIO运行模式的函数，直接采用寄存器控制方式，建议对照手册进行阅读

```c
void GPIO_ChangeOutput(GPIO_TypeDef* GPIOx, uint16_t Pinx, bool high)
```

没什么用的改变输出函数，只是将SetBits和ResetBits合并。

以上函数均需要通过下面的结构体才能访问

```c
const struct GPIO_Controllers_Type spGPIO = {
    .general_config = GPIO_Config,
    .output_config = GPIO_OUT_Config,
    .input_config = GPIO_IN_Config,
    .alternal_config = GPIO_AF_Config,
    .analog_config = GPIO_AN_Config,
    
    .set = GPIO_ChangeOutput,
    .mode = GPIO_SetMode,
    
    .toggle = GPIO_ToggleBits,
    .get_output = GPIO_ReadOutputDataBit,
    .get_input = GPIO_ReadInputDataBit
};
```

实际上实现了一个类似于类对象的访问方法，对于各函数的介绍，可以具体参考[sp_gpio.h](../SPLib/BSP/sp_gpio.h)中的Doxygen注释。

#### 位于[sp_gpio.h](../SPLib/BSP/sp_gpio.h)中的函数及变量

将GPIO_PinSourcex直接转换为GPIO_Pin_x

```c
#define spGPIO_PinFromPinSource(ps)         (0x0001<<(ps))  /* Resolve GPIO_Pin_X from GPIO_PinSourceX */
```

部分函数具有高度类同性，采用简化记叙方法

直接引出寄存器的输入输出，并以[整个寄存器的data形式和可以对各位读取和写入的bits形式](sp_type.md)两种形式引出

```c
#define spPinOut(gpio)         (((spInt32_t*)&(gpio->ODR))->data)
#define spPinIn(gpio)          (((spInt32_t*)&(gpio->IDR))->data)
#define spPinOutB(gpio)        (((spInt32_t*)&(gpio->ODR))->bits)
#define spPinInB(gpio)         (((spInt32_t*)&(gpio->IDR))->bits)
```

```c
#ifdef GPIOA
    #define spPAOut         (((spInt32_t*)&(GPIOA->ODR))->data)
    #define spPAIn          (((spInt32_t*)&(GPIOA->IDR))->data)
    #define spPAOutB        (((spInt32_t*)&(GPIOA->ODR))->bits)
    #define spPAInB         (((spInt32_t*)&(GPIOA->IDR))->bits)
#endif
......
//以下部分为从GPIOB到最后的所有GPIO寄存器
```

显得没什么用的结构体，可以将GPIO和Pin_Source绑定在一起，表示GPIO的某一针脚（Pin和PinSource可以通过预定义函数进行转换）

```c
typedef struct {
    GPIO_TypeDef*       gpio;
    uint32_t            pin_source;                 /*!< Select pin number, as GPIO_PinSource[0~15] or 0~15 */
} spPinSet;
static const spPinSet spNullPin = {NULL};
```

结构体变量对外引出，实际上完全没有必要建立这样一个结构体

```c
extern const struct GPIO_Controllers_Type {...} spGPIO;
```

