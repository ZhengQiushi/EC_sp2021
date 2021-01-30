 /*
  ******************************************************************************
  * @author         : Crazt
  * @brief          : Super capacitor module voltage current power monitor program
  * 				  I use two ina226 Bi-Directional current monitor,the address
  * 				  of them are set as following:
  * 				  Battery Monitor     R15   0100 0100  0x82
  * 				  Capacitor Monitor   R20   0100 0000  0x80
  *
  * 				  For Tongji SuperPower 2020
  ******************************************************************************

  ******************************************************************************
 */
#include "stm32f1xx.h"
//The device 7 bits address value in datasheet must be shifted to the left before calling the interface
//???
#define InaAdd1 0x82  //1000 100 Battery1`
#define InaAdd2 0x80  //1000 000 Capacitor

#define ConfigReg   0x00
#define VoltageReg  0x02
#define PowerReg	0x03
#define CurrentReg  0x04
#define CalibrationReg 0x05
extern int16_t InaVol[2],InaPow[2],InaCur[2];

void Ina226_Init(void);
void Ina226_Read(void);
