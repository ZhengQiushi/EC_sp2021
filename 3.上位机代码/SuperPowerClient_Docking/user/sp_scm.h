#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_plot.hpp"
#include "imgui_custom.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "stb_image.h"
#include "Serial.h"

//头文件到底应该怎么组织啊，现在感觉好混乱
#define  SCM_DATASENNDING_PERIOD 0.2f

//当前的数据帧种类以及帧总长定义
#define	 SP_DATA_NUM 4

#define  SP00_LenTx 3
#define  SP01_LenTx 9
#define  SP02_LenTx 4
#define  SP03_LenTx 6

#define  SP00_LenRx 3
#define  SP01_LenRx 9
#define  SP02_LenRx 10
#define  SP03_LenRx 6

struct SCM_StatusType {
	//Settings
	bool Buck;
	bool Mode;
	bool ForceUseCap;
	//Status
	float SetPow;
	float CapVoltage;
	float CapCurrent;
	float CapPower;
	float BatteryVoltage;
	float BatteryCurrent;
	float BatteryPower;
	//Advanced Settings
	bool  AdvancedSettingEnable;//default false,active when receive data from scm
	float Cap_MaxVol_H;
	float Cap_MaxVol_L;
	float Cap_MinVol_H;
	float Cap_MinVol_L;
	//IMU Status
	float Accel[3];
	float Gyro[3];
	float Speed[3];
	float Temp;
	//Extra Status
	bool Online;
	double OnlineTime;

	double ErrorCalTime;
	int FrameCount;
	int ErrorFrameCount;
};

typedef union
{
	float value;
	uint32_t value_i;
	unsigned char data[4];
}SerialDataType;

void SP_SCM_MainWindow();
void SP_EmptyWindow(bool* p_open);
void SP_SCM_Display(SCM_StatusType* SCM_Status);
void SP_SCM_PlotPower(bool* p_open, SCM_StatusType* SCM_Status);
void SP_SCM_PlotIMU(bool* p_open, SCM_StatusType* SCM_Status);
void SP_SCM_Settings(SCM_StatusType* SCM_Status, SCM_StatusType* SCM_StatusSet);
void SP_SerialConnection(bool* p_open, CSerial* Serial, SCM_StatusType* SCM_Status);
void SP_SerialDataReceive(CSerial* Serial, SCM_StatusType* SCM_Status, SCM_StatusType* SCM_StatusSet);
void SP_SerialDataTransmit(CSerial* Serial, SCM_StatusType* SCM_StatusSet, SCM_StatusType* SCM_Status);
bool crc_check(SerialDataType* Data, uint16_t Len);