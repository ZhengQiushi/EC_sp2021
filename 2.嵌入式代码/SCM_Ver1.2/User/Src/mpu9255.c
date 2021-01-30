#include <mpu9255.h>

struct MPU9255_RAW_DATA MPU9255_raw_data_t;
struct MPU9255_REAL_DATA MPU9255_real_data_t;

struct MPU9255_RAW_DATA MPU9255_zerooffset;
struct MPU9255_REAL_DATA MPU9255_zerooffset_real;

float BUF[7];

float data1,data2,data3;
float accel1;

void IMU_TASK(void)
{
	MPU9255_Read(&MPU9255_raw_data_t);
	MPU9255_ConvertData( MPU9255_raw_data_t, &MPU9255_real_data_t);

}
uint8_t MPU9255_ZeroOffset(void)
{
	int i=0;
	for(i=0;i<50;i++)
	{
		MPU9255_Read(&MPU9255_zerooffset);
		MPU9255_ConvertData0(MPU9255_zerooffset,&MPU9255_zerooffset_real);
//		BUF[0]+=MPU9255_zerooffset_real.accel[0];
		BUF[1]+=MPU9255_zerooffset_real.accel[1];
		BUF[2]+=MPU9255_zerooffset_real.accel[2];
		BUF[3]+=MPU9255_zerooffset_real.gyro[0];
		BUF[4]+=MPU9255_zerooffset_real.gyro[1];
		BUF[5]+=MPU9255_zerooffset_real.gyro[2];
		HAL_Delay(100);
	}
	for(i=1;i<6;i++)
	{
		BUF[i]/=50;
	}
	BUF[0]=0;
	BUF[6]=Gravity-BUF[2];
//	printf("%f,%f,%f\n",BUF[0],BUF[1],BUF[2]);
	return 1;

}

uint16_t BUF_cnt=0;
uint16_t speed_cnt=0;

void MPU9255_ConvertData(struct MPU9255_RAW_DATA raw_data, struct MPU9255_REAL_DATA *real_data)
{
	accel1=convert(raw_data.accel[0])/16384;
	real_data->accel[0]=(((convert(raw_data.accel[0]))/16384)+1*sinf(real_data->angle[1]*3.14/180))*cosf(real_data->angle[1]*3.14/180);//单位g +0.95*sinf(real_data->angle[1]*3.14/180)
//	real_data->accel[1]=(convert(raw_data.accel[1]))/16384-BUF[1];
//	real_data->accel[2]=(convert(raw_data.accel[2]))/16384-BUF[2]*cosf(real_data->angle[1]*3.14/180)-BUF[6];
//	real_data->gyro[0]=(convert(raw_data.gyro[0]))/131.2-BUF[3];//65.6
//	real_data->gyro[1]=(convert(raw_data.gyro[1]))/131.2-BUF[4];
//	real_data->gyro[2]=(convert(raw_data.gyro[2]))/131.2-BUF[5];
	if(fabs(real_data->gyro[0])>=0.5)
		real_data->angle[0]+=real_data->gyro[0]*UpdataCycle;
	if(fabs(real_data->gyro[1])>=0.5)
		real_data->angle[1]+=real_data->gyro[1]*UpdataCycle;
	if(fabs(real_data->gyro[2])>=0.5)
		real_data->angle[2]+=real_data->gyro[2]*UpdataCycle;
	if(fabs(real_data->accel[0])>=0.03)
		real_data->speed[0]+=real_data->accel[0]*11*UpdataCycle;
	if(fabs(real_data->accel[1])>=0.03)
		real_data->speed[1]+=real_data->accel[1]*11*UpdataCycle;
	if(fabs(real_data->accel[2])>=0.03)
		real_data->speed[2]+=real_data->accel[2]*11*UpdataCycle;

	if(fabs(accel1-BUF[0])<=0.02)
		BUF_cnt++;
	else BUF_cnt=0;
	if(BUF_cnt>=100)
	{
		real_data->angle[1]=0;
		BUF_cnt=0;
	}

	if(fabs(real_data->accel[0])<=0.1)
		speed_cnt++;
	else speed_cnt=0;
	if(speed_cnt>=100)
	{
		real_data->speed[0]=0;
		speed_cnt=0;
	}

}

void MPU9255_ConvertData0(struct MPU9255_RAW_DATA raw_data, struct MPU9255_REAL_DATA *real_data)
{
	real_data->accel[0]=(convert(raw_data.accel[0]))/16384;//单位g
	real_data->accel[1]=(convert(raw_data.accel[1]))/16384;
	real_data->accel[2]=(convert(raw_data.accel[2]))/16384;
	real_data->gyro[0]=(convert(raw_data.gyro[0]))/131.2;
	real_data->gyro[1]=(convert(raw_data.gyro[1]))/131.2;
	real_data->gyro[2]=(convert(raw_data.gyro[2]))/131.2;
}


uint8_t MPU9255_Write_Reg(uint8_t reg,uint8_t value)

{

	uint8_t status;

	mpuCsEnable();											  //使能SPI传输

	status = HAL_SPI_Transmit(&hspi1, &reg, 1, 0xFFFF);

	status = HAL_SPI_Transmit(&hspi1, &value, 1, 0xFFFF);

	mpuCsDisable();										  	//禁止MPU9255


	return(status);													//返回状态值

}


uint8_t MPU9255_Read_Reg(uint8_t reg)

{

	uint8_t reg_val;

	mpuCsEnable();

	reg = reg|0x80;

	HAL_SPI_Transmit(&hspi1, &reg, 1, 0xFFFF);	 	//发送读命令+寄存器号

 	HAL_SPI_Receive(&hspi1, &reg_val, 1, 0xFFFF);				//读取寄存器值

 	mpuCsDisable();																//禁止SPI传输


	return(reg_val);

}


float convert(uint16_t data)
{
	float data_out=0;
	if(data>=0x8000)
		data_out=-(float)((data-0x8001)^0x7fff);
	else
		data_out=(float)data;
	return data_out;


}

uint8_t MPU9255_Init(void)
{
	HAL_Delay(200);
	uint8_t ack=MPU9255_Read_Reg(WHO_AM_I);
	if(ack==112)
	{
		MPU9255_Write_Reg(PWR_MGMT_1,0x80);  //解除休眠模式
//		HAL_Delay(200);
		MPU9255_Write_Reg(PWR_MGMT_1,0x00);
		MPU9255_Write_Reg(USER_CTRL,0x10 );
		MPU9255_Write_Reg(CONFIG, 0x07);

/*******************Init GYRO and ACCEL******************************/
		MPU9255_Write_Reg(SMPLRT_DIV, 0x00);      //1kHz
		MPU9255_Write_Reg(GYRO_CONFIG, 0x01);     //500deg/s
		MPU9255_Write_Reg(ACCEL_CONFIG, 0x10);    //8g
		MPU9255_Write_Reg(ACCEL_CONFIG_2, 0x08);  //高通滤波频率 0x08(1.13kHz)

		printf("Initialize completed\n");
		return true;
	}
	else
	{
		printf("Initialize failed\n");
		return false;
	}

}


void MPU9255_Read(struct MPU9255_RAW_DATA *MPU9255_raw_data)
{
	uint8_t BUF[2];
	BUF[1]=MPU9255_Read_Reg(ACCEL_XOUT_H);
	BUF[0]=MPU9255_Read_Reg(ACCEL_XOUT_L);
	MPU9255_raw_data->accel[0]=(((uint16_t)BUF[1])<<8)|BUF[0];
	BUF[1]=MPU9255_Read_Reg(ACCEL_YOUT_H);
	BUF[0]=MPU9255_Read_Reg(ACCEL_YOUT_L);
	MPU9255_raw_data->accel[1]=(((uint16_t)BUF[1])<<8)|BUF[0];
	BUF[1]=MPU9255_Read_Reg(ACCEL_ZOUT_H);
	BUF[0]=MPU9255_Read_Reg(ACCEL_ZOUT_L);
	MPU9255_raw_data->accel[2]=(((uint16_t)BUF[1])<<8)|BUF[0];
	BUF[1]=MPU9255_Read_Reg(GYRO_XOUT_H);
	BUF[0]=MPU9255_Read_Reg(GYRO_XOUT_L);
	MPU9255_raw_data->gyro[0]=(((uint16_t)BUF[1])<<8)|BUF[0];
	BUF[1]=MPU9255_Read_Reg(GYRO_YOUT_H);
	BUF[0]=MPU9255_Read_Reg(GYRO_YOUT_L);
	MPU9255_raw_data->gyro[1]=(((uint16_t)BUF[1])<<8)|BUF[0];
	BUF[1]=MPU9255_Read_Reg(GYRO_ZOUT_H);
	BUF[0]=MPU9255_Read_Reg(GYRO_ZOUT_L);
	MPU9255_raw_data->gyro[2]=(((uint16_t)BUF[1])<<8)|BUF[0];
	BUF[1]=MPU9255_Read_Reg(TEMP_OUT_H);
	BUF[0]=MPU9255_Read_Reg(TEMP_OUT_L);
	MPU9255_raw_data->temp=(((uint16_t)BUF[1])<<8)|BUF[0];
}



void mpuCsEnable()
{
	HAL_GPIO_WritePin(MPU9255_Port_CS, MPU9255_Pin_CS, GPIO_PIN_RESET);
}
void mpuCsDisable()
{
	HAL_GPIO_WritePin(MPU9255_Port_CS, MPU9255_Pin_CS, GPIO_PIN_SET);
}
