# IRQ_Regeiste(...)

#### 函数作用

​	注册中断信息并放入注册中断池[spIRQ_CbPool](spIRQ_CbPool.md)中，同时将中断信息结构体实体的地址链接在同中断类型的spIRQ_CbEntries

#### 函数类型

​	[spIRQ_CbUnit_t*](spIRQ_CbUnit_t.md) 结构体地址类型

#### 输入

| 变量类型                                    | 变量名称 | 实际意义                                       |
| ------------------------------------------- | -------- | ---------------------------------------------- |
| IRQn_Type                                   | irq      | 中断类型，如TIM8_BRK_TIM12_IRQn                |
| uint32_t                                    | it_flag  | 具体的中断标志位，如TIM_FLAG_Trigger           |
| IRQ_Callback_t (实际上是 void (*fun)(void)) | cb       | 回调函数通用类型，void类型无返回值，无输入void |

#### 输出

| 类型            | 作用               | 备注 |
| --------------- | ------------------ | ---- |
| spIRQ_CbUnit_t* | 注册中断变量的地址 |      |

#### 代码流程

​	输入不存在的回调函数(!cb为真)或不存在的中断(irq<0)时退出函数

​	否则定义一个spIRQ_CbUnit_t*指针pCb，将实体池[spIRQ_CbPool](spIRQ_CbPool.md)中第一个[空实体spIRQ_CbNull](spIRQ_CbPool.md)的地址赋给指针。

​	通过指针pCb将irq,it_flag,cb保存进结构体变量实体中

​	将该实体链接在spIRQ_CbEntries[irq]的尾部

​	即将实体的地址pCb保存在同irq中断的spIRQ_CbEntries[irq]对应链表尾部实体的next成员中

#### 代码笔记

​	对于

```c
    if(!cb) return NULL;
```

这段代码，如果函数不存在，cb是不是为0呢？

另外，能不能加入一个static变量，保存当前池中最前一个空位的序号，避免每次通过循环查找空列

更进一步，能不能加入优先级设计，在池满时，用新的高优先级中断覆盖低优先级的中断？