 /*
  ******************************************************************************
  * @author         : Crazt
  * @brief          : Super capacitor module voltage current power monitor program
  * 				  I use two ina226 Bi-Directional current monitor,the address
  * 				  of them are set as following:
  * 				  Battery Monitor     R15   1000100
  * 				  Capacitor Monitor   R20   1000000
  *					  Search datasheet for more information.
  ******************************************************************************
							For Tongji SuperPower 2020
  ******************************************************************************
 */
#include "ina226.h"

#include "i2c.h"

uint8_t Buffer[2];
//Storage the value read from ina226
int16_t InaVol[2],InaPow[2],InaCur[2];

void Ina226_Init(void){
	//Set averages number: 512 times
	Buffer[0]|=1<<11; //AVG2
	Buffer[0]|=1<<10; //AVG2
	Buffer[0]|=1<<9;  //AVG2
	//Set Bus Voltage Conversion Time: 1.1ms
	Buffer[0]|=1<<8; //AVG2
	Buffer[1]|=0<<7; //AVG2
	Buffer[1]|=0<<6; //AVG2
	//Set Shunt Voltage Conversion Time: 1.1ms
	Buffer[1]|=1<<5; //AVG2
	Buffer[1]|=0<<4; //AVG2
	Buffer[1]|=0<<3; //AVG2
	//Set Operation Mode
	Buffer[1]|=1<<2; //AVG2
	Buffer[1]|=1<<1; //AVG2
	Buffer[1]|=1;    //AVG2

	HAL_I2C_Mem_Write(&hi2c1,InaAdd1,ConfigReg,I2C_MEMADD_SIZE_8BIT,Buffer,2,100);
	HAL_I2C_Mem_Write(&hi2c1,InaAdd2,ConfigReg,I2C_MEMADD_SIZE_8BIT,Buffer,2,100);

	//Set Calibration Register 1mA/bit
	Buffer[0]=0x0A;
	Buffer[1]=0x00;
	HAL_I2C_Mem_Write(&hi2c1,InaAdd1,CalibrationReg,I2C_MEMADD_SIZE_8BIT,Buffer,2,100);
	HAL_I2C_Mem_Write(&hi2c1,InaAdd2,CalibrationReg,I2C_MEMADD_SIZE_8BIT,Buffer,2,100);

}
void Ina226_Read(void){
	//Read bus voltage
	HAL_I2C_Mem_Read(&hi2c1,InaAdd1,VoltageReg,I2C_MEMADD_SIZE_8BIT,Buffer,2,100);
	InaVol[0]=Buffer[0]<<8|Buffer[1];

	HAL_I2C_Mem_Read(&hi2c1,InaAdd2,VoltageReg,I2C_MEMADD_SIZE_8BIT,Buffer,2,100);
	InaVol[1]=Buffer[0]<<8|Buffer[1];

	//Read power
	HAL_I2C_Mem_Read(&hi2c1,InaAdd1,PowerReg,I2C_MEMADD_SIZE_8BIT,Buffer,2,100);
	InaPow[0]=Buffer[0]<<8|Buffer[1];

	HAL_I2C_Mem_Read(&hi2c1,InaAdd2,PowerReg,I2C_MEMADD_SIZE_8BIT,Buffer,2,100);
	InaPow[1]=Buffer[0]<<8|Buffer[1];

	//Read current
	HAL_I2C_Mem_Read(&hi2c1,InaAdd1,CurrentReg,I2C_MEMADD_SIZE_8BIT,Buffer,2,100);
	InaCur[0]=Buffer[0]<<8|Buffer[1];

	HAL_I2C_Mem_Read(&hi2c1,InaAdd2,CurrentReg,I2C_MEMADD_SIZE_8BIT,Buffer,2,100);
	InaCur[1]=Buffer[0]<<8|Buffer[1];

}




