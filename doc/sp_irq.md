# sp_irq

#### 代码笔记

​	将单一的终端控制函数扩展为对回调函数池中所有函数依次进行运行

​	出现头文件嵌套引用的情况，其引用链如下：sp_irq.h->sp_conf.h->stm32f4xx_conf.h->各外设库文件

​	实际上通过将注册中断与当前中断进行对比，满足则使用对应的回调函数池依次进行

#### 文件

​	[sp_irq.c](../SPLib/APP/sp_irq.c)

​	[sp_irq.h](../SPLib/APP/sp_irq.c)

#### 成员变量/结构体

| 变量名                                         | 作用                                                         | 备注                                                    |
| ---------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------- |
| [spIRQ_CbUnit_t](./sp_irq/spIRQ_CbUnit_t.md)   | 保存有注册中断的各种信息<br />如中断位、回调函数地址、中断位清除函数地址 | typedef from __spIRQ_CbUnit                             |
| [spIRQ_CbEntries](./sp_irq/spIRQ_CbEntries.md) | 注册中断的索引保存<br />保存有各类型中断的第一个注册信息的地址<br />余下注册信息可通过链表结构进行查找 | spIRQ_CbUnit_t* 结构体指针类型                          |
| [spIRQ_CbPool](./sp_irq/spIRQ_CbPool.md)       | 注册中断信息的实际保存区                                     | spIRQ_CbUnit_t   结构体类型                             |
| IRQ_Callback_t                                 | IRQ回调函数类型，实际上只是函数类型的typedef                 | typedef void (*IRQ_Callback_t)(void);                   |
| spIRQ_GetITStatus                              | 获取中断状态，实际为typedef函数类型                          | typedef ITStatus (*spIRQ_GetITStatus)(void*, uint32_t); |
| spIRQ_ClearPending                             | 清除中断位，实际为typedef函数类型                            | typedef void (*spIRQ_ClearPending)(void*, uint32_t);    |

#### 函数(公有)

​	中断函数重定义部分略

​	其示例代码如下

	void UART4_IRQHandler (void) {
	spIRQ.invoke(UART4_IRQn, UART4, 
	    (spIRQ_GetITStatus)USART_GetITStatus, 
	    (spIRQ_ClearPending)USART_ClearITPendingBit);}
​	将中断函数重定义至invoke函数，同时将外设中断，外设，外设获取中断状态函数及中断位清除函数一并传输进invoke函数进行处理。

#### 函数(私有)

```
struct __IRQ_Manager_Type spIRQ = {
.init = IRQ_ManagerInit,
.registe = IRQ_Regeiste,
.invoke = IRQ_Invoke,};
```

以下函数通过上面的结构体对外引出，不能直接使用。

| 函数名                                             | 函数作用                           | 备注                                                         |
| -------------------------------------------------- | ---------------------------------- | ------------------------------------------------------------ |
| [IRQ_ManagerInit()](./sp_irq/IRQ_ManagerInit().md) | 将用于存储中断信息的回调函数池清空 | 并非单纯的NULL，而是根据结构体类型进行构建的"空"<br />       |
| [IRQ_Regeiste(...)](./sp_irq/IRQ_Regeiste(...).md) | 中断注册函数                       | 函数是否有进一步优化的可能？<br />可能的优化思路见函数内代码笔记 |
| [IRQ_Invoke(...)](./sp_irq/IRQ_Invoke(...).md)     | 中断回调函数                       |                                                              |

#### 代码笔记

​	应当说是一段相当优秀和经典的代码

​	采用预设数组空间[spIRQ_CbPool[USING_IRQ_POOL_SIZE]](./sp_irq/spIRQ_CbPool.md)作为保留空间，规避了使用malloc或calloc时链表出现异常。也就是说，最多支持USING_IRQ_POOL_SIZE个中断回调函数的注册

​	使用一个指针数组[spIRQ_CbEntries[91]](./sp_irq/spIRQ_CbEntries.md)用于保存所有中断的注册信息的链表头。

​	数组只是作为保留空间，实际触发中断时不通过数组，而是通过链表查找。

​	对于函数地址重定义部分，也就是

```c
typedef void (*IRQ_Callback_t)(void);
```

这段代码，根据目前掌握到的规律，大致重定义规律如下

对于返回类型为A，函数参数列表为(B C,D E)，其中B,D为变量类型，C,E为变量实体，这一类的函数

```c
A fun(B C,D E) {...}
```

其函数通用形式的重typedef为

```c
typedef A (*New_Name)(B,D);
```

其中New_Name定义的变量可以直接赋值该类型某一函数的函数名，实际上可以通过该变量直接调用函数，例如

```c
New_Name New_fun = fun;
```

则以下两种写法等价

```c
A fun_value = fun(B_Type_Value,D_Type_Value);
```

```C
A fun_value = New_fun(B_Type_Value,D_Type_Value);
```

