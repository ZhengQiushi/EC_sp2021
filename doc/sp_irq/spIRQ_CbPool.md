# spIRQ_CbPool

#### 变量类型

​	[spIRQ_CbUnit_t](spIRQ_CbUnit_t.md)

​	结构体实体

```c
spIRQ_CbUnit_t                   spIRQ_CbPool[USING_IRQ_POOL_SIZE];
```

#### 变量作用

​	用于保存所有的注册中断，注册中断位，回调函数及清除中断位函数

​	与[spIRQ_CbEntries](spIRQ_CbEntries.md)配合使用

​	在实际使用时，仅保存时会通过数组进行，调用时是通过保存有所有中断对应的链表头的[spIRQ_CbEntries](spIRQ_CbEntries.md)数组进行索引。

#### 代码笔记

​	USING_IRQ_POOL_SIZE定义在[sp_conf.h](../../User/sp_conf.h)

