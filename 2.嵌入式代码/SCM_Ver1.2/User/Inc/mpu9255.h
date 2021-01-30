/*
 * mpu9255.h
 *
 *  Created on: 2020年6月4日
 *      Author: lyq
 */

#ifndef MPU9255_H_
#define MPU9255_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "spi.h"
#include <stdbool.h>
#include "stdio.h"
#include "math.h"

//mpu9255 CS
#define MPU9255_Port_CS  GPIOC
#define MPU9255_Pin_CS   GPIO_PIN_4

#define	SMPLRT_DIV		0x19
#define	CONFIG			0x1A
#define	GYRO_CONFIG		0x1B
#define	ACCEL_CONFIG	0x1C
#define ACCEL_CONFIG_2  0x1D

#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40

#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48


#define MAG_XOUT_L		0x03
#define MAG_XOUT_H		0x04
#define MAG_YOUT_L		0x05
#define MAG_YOUT_H		0x06
#define MAG_ZOUT_L		0x07
#define MAG_ZOUT_H		0x08

#define USER_CTRL       0x6A
#define	PWR_MGMT_1		0x6B
#define	WHO_AM_I		0x75


#define	GYRO_ADDRESS   0xD0
#define MAG_ADDRESS    0x18
#define ACCEL_ADDRESS  0xD0

#define UpdataCycle     0.001
#define Gravity         0.9

typedef struct MPU9255_RAW_DATA
{
    uint16_t accel[3];
    uint16_t temp;
    uint16_t gyro[3];
    uint16_t time;
} MPU9255_raw_data;

typedef struct MPU9255_REAL_DATA
{
    float accel[3];
    float temp;
    float gyro[3];
    float speed[3];
    float angle[3];
} MPU9255_real_data;

extern struct MPU9255_RAW_DATA MPU9255_raw_data_t;
extern struct MPU9255_REAL_DATA MPU9255_real_data_t;

extern struct MPU9255_RAW_DATA MPU9255_zerooffset;
extern struct MPU9255_REAL_DATA MPU9255_zerooffset_real;


void IMU_TASK(void);
void mpuCsEnable();
void mpuCsDisable();
uint8_t MPU9255_Init(void);//mpu9255初始化
uint8_t MPU9255_Write_Reg(uint8_t reg,uint8_t value);//spi发送
uint8_t MPU9255_Read_Reg(uint8_t reg);
void MPU9255_Read(struct MPU9255_RAW_DATA *MPU9255_raw_data);//mpu9255初始数据读取
void MPU9255_ConvertData(struct MPU9255_RAW_DATA raw_data, struct MPU9255_REAL_DATA *real_data);//不加补偿的转换
uint8_t MPU9255_ZeroOffset(void);//去零飘
void MPU9255_ConvertData0(struct MPU9255_RAW_DATA raw_data, struct MPU9255_REAL_DATA *real_data);
float convert(uint16_t data);//uint16_t转float

#endif /* MPU9255_H_ */

