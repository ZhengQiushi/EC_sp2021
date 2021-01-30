# IRQ_ManagerInit()

```c
void IRQ_ManagerInit(void) {
    memset(spIRQ_CbEntries, NULL, sizeof(spIRQ_CbEntries)/sizeof(spIRQ_CbEntries[0]));
    for(uint16_t i=0; i<sizeof(spIRQ_CbPool)/sizeof(spIRQ_CbPool[0]); i++) {
        spIRQ_CbPool[i] = spIRQ_CbNull;
    }
}
```

#### 函数作用

​	IRQ信息池全部清空，包含各中断链表头地址池和中断信息实体池。

```c
static const spIRQ_CbUnit_t             spIRQ_CbNull = {(IRQn_Type)(NonMaskableInt_IRQn-2), 0x00, NULL, NULL};
```

​	上面是空池的基本元素

#### 函数类型

​	void

#### 输入

​	无

#### 输出

​	无

