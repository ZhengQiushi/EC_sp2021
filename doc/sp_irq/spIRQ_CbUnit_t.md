# spIRQ_CbUnit_t

```c
struct __spIRQ_CbUnit {
    IRQn_Type                           irq_type;
    uint32_t                            interrupt_request_flag; /*!< Interrupt request flags for IT status checking. */
    IRQ_Callback_t                      callback;               /*!< Callback function. */
    struct __spIRQ_CbUnit*              next;                   /*!< Next callback unit. */
};
```

#### 代码笔记

​	中断关键结构体，为链表结构

​	包含中断类型，中断标志位，回调函数及下一位结构体所处位置

​	之所以要保存中断标志位，是因为有时同一个中断可能对应多个标志位，如TIM1_TRG_COM_TIM11_IRQHandler这类涉及到两个中断的公共中断触发，可能有多个标志位(flag)对应，因此需要具体执行部分的进一步判断。

​	flag与IRQn_TYPE之间有一定的关系，例如TIM8_BRK_TIM12_IRQn这一IRQn_TYPE，需要通过FLAG(TIM_FLAG_Update还是TIM_FLAG_Break？)进行进一步的标志位判断，避免共用中断错误触发回调函数。

​	采用链表结构可以帮助同类型(IRQn_Type)中断查找回调函数加快，类似于计算机保存数据的结构

```c
typedef void (*IRQ_Callback_t)(void);
```

​	上面这段代码定义了一类可以赋给IRQ_Callback_t类型变量，通过变量进行等价调用的函数，输入为void无输入，输出为void无返回值，实际作用为回调函数通用函数变量类型，相关描述参考[sp_irq.md](../sp_irq.md)。

​	