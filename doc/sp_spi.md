# sp_spi

#### 代码笔记

​	建议阅读SPI通信后阅读本文档

​	stm32已将SPI通信尽可能化简至最简，在正确初始化SPI后，只需要将数据放入DR中，即可完成发送，当可以接收时，亦只需从DR寄存器中读取。

​	以下代码相较于SPI官方库相对而言较为简单，唯一的缺憾是将SPIx与片选NSSx进行了绑定，使得每个SPI设备只能通过一个SPI接口进行通信，浪费了SPI支持多个设备使用同一MISO/MOSI/SCK线进行通信的优势。由于代码中对SPI通信采用即发即收的形式，故因为其他设备发送消息造成DR数据错误的情况概率很低。

​	从源代码中亦发现了通过DMA直接读取SPI传输数据的被注释代码。这部分代码的确没有存在的必要，因SPI完全可以在使用时迅速收发，使用DMA可能会造成额外的错误。

#### 文件

​	[sp_spi.c](../SPLib/BSP/sp_spi.c)

​	[sp_spi.h](../SPLib/BSP/sp_spi.c)

#### 成员变量/结构体

| 变量名                                                       | 类型       | 备注                                                         |
| ------------------------------------------------------------ | ---------- | ------------------------------------------------------------ |
| typedef struct {<br/>    SPI_TypeDef*        this;<br/>    spPinSet            sck;<br/>    spPinSet            miso;<br/>    spPinSet            mosi;<br/>    spPinSet            nss;<br/>    SPI_InitTypeDef     spi_initer;<br/>} SPI_PinsType; | 结构体     | 保存初始化的SPI，对应针脚信息([sp_PinSet](../SPLib/BSP/sp_gpio.h))以及对应初始化信息 |
| SPIx_Pins                                                    | 结构体变量 | 定义各SPIx的针脚<br />省略大量查找手册的时间                 |
| extern struct __SPI_Manager_Type {<br/>    uint16_t (*read_write_b)(SPI_TypeDef* spi, uint16_t data);<br/>    uint16_t (*write_b)(SPI_TypeDef* spi, uint16_t data);<br/>    void     (*select)(SPI_PinsType* spi_pins);<br/>    void     (*release)(SPI_PinsType* spi_pins);<br/>} spSPI; | 结构体变量 | 类似于类的结构体，建立spi自建函数与外界的接口                |

#### 函数(公有)

##### 宏定义形式函数(多余函数)

```c
#define SPI5_NSS_Select     GPIO_ResetBits(GPIOF, GPIO_Pin_6)
#define SPI5_NSS_Release    GPIO_SetBits(GPIOF, GPIO_Pin_6)
```

根据SPI通信协议，多个设备可以接在同一个SPI上，共用MOSI,MISO,SCK，设备通过NSS/CS片选端进行寻址。当片选端置为低电平时，对应设备有效 ，可以通过公共数据和时钟线进行数据传输。

#### 函数(私有)

```c
struct __SPI_Manager_Type spSPI = {
    .read_write_b = spSPI_ReadWriteByte,
    .write_b = spSPI_Write,
    .select = spSPI_ChipSelect,
    .release = spSPI_ChipRelease
};
```

以下函数通过上面的结构体对外引出，不能直接使用。

| 函数名                                               | 函数作用              | 备注                                                         |
| ---------------------------------------------------- | --------------------- | ------------------------------------------------------------ |
| spSPI_Write(SPI_TypeDef* spi, uint16_t data)         | 向某一SPI写入数据     | 代码没有加入保护，如果某一SPI设备掉线，则很可能持续处于发送状态 |
| spSPI_ReadWriteByte(SPI_TypeDef* spi, uint16_t data) | 与某一SPI进行数据交换 | 函数是否有进一步优化的可能？<br />结合spSPI_Write进行代码优化 |
| spSPI_ChipSelect(SPI_PinsType* spi_pins)             | 片选对应SPI外设       | 将SPIx绑定的针脚中的NSS针脚置为低电平有效                    |
| spSPI_ChipSelect(SPI_PinsType* spi_pins)             | 释放对应SPI外设的片选 | 将SPIx绑定的针脚中的NSS针脚置为高电平无效                    |
| void SPIx_Init(void) (x=4,5)                         | 对对应SPI进行初始化   | 仅对SPI4和SPI5进行初始化，其代码无特殊之处                   |

#### 代码笔记

​	修改空间很大，完全可以采用一个SPI对应多个外设，通过不同CS寻址的方式节省针脚。将片选与SPIx绑定的操作实属无意义。

​	代码采用纯寄存器读写方式，其涉及到的寄存器如下：

​	SR-Status Register 状态寄存器，不同的外设对应的状态寄存器在位与功能的对应，状态寄存器地址等方面均有差异，具体需要参考stm32用户手册(User Mamual)

​		用到的标志位为

​			SPI_SR_TXE- TX Empty 传输端空闲，可以进行传输操作

​			SPI_SR_RXNE- RX Not Empty 接收端非空，可以进行接收操作

​	DR-Data Register 数据寄存器，不同外设的数据寄存器数目，大小均可能有所不同。在SPI中，由于数据采用的是移位寄存器方式进行传输，即主从设备同时进行数据的传输，将数据放入数据寄存器DR中，则数据会自动进行传输至外设，且获得来自SPI外设接收到的消息并保存在DR寄存器中。

##### 只进行写入的函数

```c
static inline uint16_t spSPI_Write(SPI_TypeDef* spi, uint16_t data) {
    while(!(spi->SR & SPI_SR_TXE));
    spi->DR = data;
    return 0;
}
```

在设备发送端为空时，将数据放入数据寄存器中，数据会自动发送

##### 同时进行读写的函数

```c
static inline uint16_t spSPI_ReadWriteByte(SPI_TypeDef* spi, uint16_t data) {
    uint16_t retry=0;
    while(!(spi->SR & SPI_SR_TXE));
    spi->DR = data;
    retry=0;
    while(!(spi->SR & SPI_SR_RXNE) && retry<10000) retry++;
    spi->SR  &= ~SPI_SR_RXNE;
    return spi->DR;
}
```

