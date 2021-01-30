​	从源代码上看，仅仅是对SPI和相关针脚进行正常初始化的过程。

​	主要涉及函数见[sp_gpio](../sp_gpio.md) 中的spGPIO结构体



```c
void SPI5_Init(void){
    SPI_InitTypeDef     spi_initer;
    
	spRCC_Set_SPI5();
	spRCC_Set_GPIOF();

    GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF_SPI5); 
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_SPI5);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_SPI5);
    spGPIO.alternal_config(GPIOF, 
        GPIO_Pin_7 |  GPIO_Pin_8|  GPIO_Pin_9, GPIO_OType_PP,
        GPIO_PuPd_UP, GPIO_Speed_100MHz);

    spGPIO.output_config(GPIOF, 
        GPIO_Pin_6, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
    GPIO_SetBits(GPIOF, GPIO_Pin_6);

    /* f_APB2 = 84MHz, f_MPU_SCLK_Max = 1MHz */
    spi_initer.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    spi_initer.SPI_CPOL         = SPI_CPOL_High;
    spi_initer.SPI_CPHA         = SPI_CPHA_2Edge;
    spi_initer.SPI_DataSize     = SPI_DataSize_8b;
    spi_initer.SPI_Direction    = SPI_Direction_2Lines_FullDuplex;
    spi_initer.SPI_FirstBit     = SPI_FirstBit_MSB;
    spi_initer.SPI_Mode         = SPI_Mode_Master;
    spi_initer.SPI_NSS          = SPI_NSS_Soft;
    // spi_initer.SPI_CRCPolynomial = 0x00;
    SPI_Init(SPI5, &spi_initer);

    SPI_Cmd(SPI5, ENABLE);
}
```
```c
void SPI4_Init(void){
    spRCC_Set_SPI4();
    spRCC_Set_GPIOE();

    /* Config pins */
    GPIO_PinAFConfig(SPI4_Pins.sck.gpio, SPI4_Pins.sck.pin_source, GPIO_AF_SPI4);
    GPIO_PinAFConfig(SPI4_Pins.miso.gpio, SPI4_Pins.miso.pin_source, GPIO_AF_SPI4);
    GPIO_PinAFConfig(SPI4_Pins.mosi.gpio, SPI4_Pins.mosi.pin_source, GPIO_AF_SPI4);

    spGPIO.alternal_config(SPI4_Pins.sck.gpio, 
        spGPIO_PinFromPinSource(SPI4_Pins.sck.pin_source), GPIO_OType_PP,
        GPIO_PuPd_UP, GPIO_Speed_100MHz);
    spGPIO.alternal_config(SPI4_Pins.miso.gpio, 
        spGPIO_PinFromPinSource(SPI4_Pins.miso.pin_source), GPIO_OType_PP,
        GPIO_PuPd_UP, GPIO_Speed_100MHz);
    spGPIO.alternal_config(SPI4_Pins.mosi.gpio, 
        spGPIO_PinFromPinSource(SPI4_Pins.mosi.pin_source), GPIO_OType_PP,
        GPIO_PuPd_UP, GPIO_Speed_100MHz);

    spGPIO.output_config(SPI4_Pins.nss.gpio, 
        spGPIO_PinFromPinSource(SPI4_Pins.nss.pin_source), GPIO_OType_PP, 
        GPIO_PuPd_UP, GPIO_Speed_100MHz);
    spSPI.release(&SPI4_Pins);

    /* f_APB2 = 84MHz, f_MPU_SCLK_Max = 1MHz */
    SPI4_Pins.spi_initer.SPI_BaudRatePrescaler    = SPI_BaudRatePrescaler_128;
    SPI4_Pins.spi_initer.SPI_CPOL                 = SPI_CPOL_High;
    SPI4_Pins.spi_initer.SPI_CPHA                 = SPI_CPHA_2Edge;
    SPI4_Pins.spi_initer.SPI_DataSize             = SPI_DataSize_16b;
    SPI4_Pins.spi_initer.SPI_Direction            = SPI_Direction_2Lines_FullDuplex;

    SPI4_Pins.spi_initer.SPI_FirstBit             = SPI_FirstBit_MSB;
    SPI4_Pins.spi_initer.SPI_Mode                 = SPI_Mode_Master;
    SPI4_Pins.spi_initer.SPI_NSS                  = SPI_NSS_Soft;
    // spi_initer.SPI_CRCPolynomial = 0x00;
    SPI_Init(SPI4_Pins.this, &SPI4_Pins.spi_initer);
    SPI_Cmd(SPI4_Pins.this, ENABLE);
}
```

