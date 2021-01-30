/************************************************************
 *File		:	euler.c
 *Author	:  @YangTianhao ,490999282@qq.com,@TangJiaxin ,tjx1024@126.com
 *Version	: V1.0
 *Update	: 2017.03.02
 *Description: 	euler caluation functions
 ************************************************************/

#include "euler.h"
#include "math.h"
#include "main.h"
#define euler_dt 0.001f
#define sampleFreq	512.0f		// sample frequency in Hz
#define betaDef		0.1f		// 2 * proportional gain
volatile float beta = betaDef;								// 2 * proportional gain (Kp)
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;	// quaternion of sensor frame relative to auxiliary frame
float Yaw_gyro,Roll_gyro,Pitch_gyro;
float Yawdelta;
float Yaw_mag,Roll_accel,Pitch_accel;
float Yaw,Roll,Pitch,Yaw_Offset,Pitch_Offset,Roll_Offset;
float dt=0.001;//??:dt????kalman???????

float angle, angle_dot;//??????

float P[2][2] = {{ 1, 0 },

                 { 0, 1 }};

float Pdot[4] ={ 0,0,0,0};

float Q_angle=0.001, Q_gyro=0.005; //???????,????????

float R_angle=0.5 ,C_0 = 1;

float q_bias, angle_err, PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;

int count = 0;
float delay_speed=0;
int euler_count=0;
double yaw_zero_float=0;
double zf_result=0.000035;

float invSqrt(float num);
void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az) ;
void init_euler(void){
	
	unsigned char i;
	float ax,ay,az,a;
	ax=ay=az=0.0f;
	if(ax==0&&ay==0&&az==0)
	{
			for(i=0;i<5;i++){
			MPU6500_Read();
			ax+=mpu6500_real_data.Accel_X;
			ay+=mpu6500_real_data.Accel_Y;
			az+=mpu6500_real_data.Accel_Z;
			
			delay_ms(5);
		}
	}
	a=1.0f/sqrt(ax*ax+ay*ay+az*az);
	ax*=a;
	ay*=a;
	az*=a;
	Roll=atan2(-ay,az);
	Pitch=asin(ax);
	Yaw=0;
}
/*


*/

//void update_euler(void){
//	if(euler_count<=1500)
//	{
//		euler_count++;
//	}
//	if(euler_count==1000)
//	{
//		yaw_zero_float=Yaw/2000.0f;
//	}
//	
//	real_data_update();
//	MPU6500_Read();
//	
//	float Roll_sin,Roll_cos,Pitch_sin,Pitch_cos;
//	float a,gx,gy,gz,dx,dy,dz;
//	static const float K=1.0f;  //?????????? ?? ????????
//	static const float KP=0.5f; //?????P????? ?????????
//	Roll_sin=sin(Roll);
//	Roll_cos=cos(Roll);
//	Pitch_sin=sin(Pitch);
//	Pitch_cos=cos(Pitch);
//	delay_speed=adis16470_real_data.gyro_z*57.3f;


//	a=invSqrt(adis16470_real_data.acce_x*adis16470_real_data.acce_x
//		+adis16470_real_data.acce_y*adis16470_real_data.acce_y
//	+adis16470_real_data.acce_z*adis16470_real_data.acce_z);
//	adis16470_real_data.acce_x*=a;
//	adis16470_real_data.acce_y*=a;
//	adis16470_real_data.acce_z*=a;

//	dx=adis16470_real_data.acce_y*Pitch_cos*Roll_cos-adis16470_real_data.acce_z*Pitch_cos*Roll_sin;
//	dy=adis16470_real_data.acce_z*(-Pitch_sin)-adis16470_real_data.acce_x*Pitch_cos*Roll_cos;
//	dz=adis16470_real_data.acce_x*Pitch_cos*Roll_sin+adis16470_real_data.acce_y*Pitch_sin;

//	gx=adis16470_real_data.gyro_x+KP*dx;
//	gy=adis16470_real_data.gyro_y+KP*dy;
//	gz=adis16470_real_data.gyro_z+KP*dz;
//	
//	
//	Roll=Roll+
//	(Pitch_cos*gx+Pitch_sin*Roll_sin*gy+Pitch_sin*Roll_cos*gz)/Pitch_cos*euler_dt;
//	
//	Pitch=Pitch+(Roll_cos*gy-Roll_sin*gz)*euler_dt;
//	
//	Yaw_gyro=Yaw+
//	(Roll_sin*gy+Roll_cos*gz)/Pitch_cos*euler_dt;
//	

//	if(Roll>PI){	
//		Roll-=2.0f*PI;
//	}else if(Roll<-PI){
//		Roll+=2.0f*PI;
//	}
//	
//	if(Pitch>PI/2.0f){	
//		Pitch-=PI;
//	}else if(Pitch<-PI/2.0f){
//		Pitch+=PI;
//	}
//	
//	if(euler_count<500)
//		Yaw=0;
//	else
//		Yaw=K*Yaw_gyro+(1.0f-K)*Yaw_mag-dynamic_zero_float_offset;
//	//Yaw=K*Yaw_gyro+(1.0f-K)*Yaw_mag-yaw_zero_float-dynamic_zero_float_offset;
//	
//	if(count <= 10)    //??offset?
//	{	
//		Yaw_Offset = Yaw ;
//		Pitch_Offset = 0;
//		Roll_Offset = 0;
//		
//		count++;
//	}
//	
//}

/*
void update_euler_mpu(void){
	MPU6500_Read();
	float gx,gy,gz,ax,ay,az;
	gx=mpu6500_real_data.Gyro_X;
	gy=mpu6500_real_data.Gyro_Y;
	gz=mpu6500_real_data.Gyro_Z;
	ax=mpu6500_real_data.Accel_X;
	ay=mpu6500_real_data.Accel_Y;
	az=mpu6500_real_data.Accel_Z;
	MadgwickAHRSupdateIMU(gx,gy,gz,ax,ay,az);

}
*/

/************************************
*Original
************************************/


void update_euler_mpu(void){
	float oldYaw=Yaw;
	float Roll_sin,Roll_cos,Pitch_sin,Pitch_cos;
	float a,gx,gy,gz,dx,dy,dz;
	static const float K=1.0f;  //?????????? ?? ????????
	static const float KP=0.5f; //?????P????? ?????????
	Roll_sin=sin(Roll);
	Roll_cos=cos(Roll);
	Pitch_sin=sin(Pitch);
	Pitch_cos=cos(Pitch);
	
	MPU6500_Read();
	delay_speed=mpu6500_real_data.Gyro_Z * 57.3f;
	a=invSqrt(mpu6500_real_data.Accel_X*mpu6500_real_data.Accel_X
		+mpu6500_real_data.Accel_Y*mpu6500_real_data.Accel_Y
	+mpu6500_real_data.Accel_Z*mpu6500_real_data.Accel_Z);
	mpu6500_real_data.Accel_X*=a;
	mpu6500_real_data.Accel_Y*=a;
	mpu6500_real_data.Accel_Z*=a;

	dx=mpu6500_real_data.Accel_Y*Pitch_cos*Roll_cos-mpu6500_real_data.Accel_Z*Pitch_cos*Roll_sin;
	dy=mpu6500_real_data.Accel_Z*(-Pitch_sin)-mpu6500_real_data.Accel_X*Pitch_cos*Roll_cos;
	dz=mpu6500_real_data.Accel_X*Pitch_cos*Roll_sin+mpu6500_real_data.Accel_Y*Pitch_sin;

	gx=mpu6500_real_data.Gyro_X+KP*dx;
	gy=mpu6500_real_data.Gyro_Y+KP*dy;
	gz=mpu6500_real_data.Gyro_Z+KP*dz;
	
	
	Roll=Roll+(Pitch_cos*gx+Pitch_sin*Roll_sin*gy+Pitch_sin*Roll_cos*gz)/Pitch_cos*euler_dt;
	
	Pitch=Pitch+(Roll_cos*gy-Roll_sin*gz)*euler_dt;
	


	Yawdelta=	(Roll_sin*gy+Roll_cos*gz)/Pitch_cos*euler_dt;
	if (fabs(Yawdelta)>0.0003)
			Yaw_gyro=Yaw+(Roll_sin*gy+Roll_cos*gz)/Pitch_cos*euler_dt;
	if(Roll>PI){	
		Roll-=2.0f*PI;
	}else if(Roll<-PI){
		Roll+=2.0f*PI;
	}
	
	if(Pitch>PI/2.0f){	
		Pitch-=PI;
	}else if(Pitch<-PI/2.0f){
		Pitch+=PI;
	}
//	if(remoteState == KEY_REMOTE_STATE)
////	{
////		Yaw=K*Yaw_gyro+(1.0f-K)*Yaw_mag-dynamic_zero_float_offset;
////		//Yaw=-Yaw;
////	}
// if(remoteState == KEY_REMOTE_STATE)
//	{
		
		Yaw=K*Yaw_gyro+(1.0f-K)*Yaw_mag+zf_result;


//	else if(remoteState == NORMAL_REMOTE_STATE)
//	{
//		Yaw=K*Yaw_gyro+(1.0f-K)*Yaw_mag-dynamic_zero_float_offset;
//	}
	if(count <= 10)    //??offset?
	{	
		Yaw_Offset = Yaw ;
		Pitch_Offset = 0;
		Roll_Offset = 0;
		
		count++;
	}
	
}



/**********************************************
*Kalman_filter
**********************************************/
/*
void update_euler_mpu(void){
	float angle_m;
	float gyro_m;
	MPU6500_Read();
	angle_m=mpu6500_real_data.Accel_X;
	gyro_m=mpu6500_real_data.Gyro_X;
	
	angle+=(gyro_m-q_bias) * dt;

        angle_err = angle_m - angle;

        Pdot[0]=Q_angle - P[0][1] - P[1][0];

        Pdot[1]=- P[1][1];

        Pdot[2]=- P[1][1];

        Pdot[3]=Q_gyro;

        P[0][0] += Pdot[0] * dt;

        P[0][1] += Pdot[1] * dt;

        P[1][0] += Pdot[2] * dt;

        P[1][1] += Pdot[3] * dt;

        PCt_0 = C_0 * P[0][0];

        PCt_1 = C_0 * P[1][0];

        E = R_angle + C_0 * PCt_0;

        K_0 = PCt_0 / E;

        K_1 = PCt_1 / E;

        t_0 = PCt_0;

        t_1 = C_0 * P[0][1];

        P[0][0] -= K_0 * t_0;

        P[0][1] -= K_0 * t_1;

        P[1][0] -= K_1 * t_0;

        P[1][1] -= K_1 * t_1;

        angle += K_0 * angle_err; //????

        q_bias += K_1 * angle_err;

        angle_dot = gyro_m-q_bias;//?????

 

         Yaw=-angle;

}
*/

/******************************
*quaternion
******************************/
/*

void update_euler_mpu(void){
	float Kp=2;
	float Ki=0.01;
	float norm;
	float hx, hy, hz, bx, bz;
	float vx, vy, vz, wx, wy, wz;
	float ex, ey, ez, halfT;
	float tempq0,tempq1,tempq2,tempq3;
	float gx,gy,gz,ax,ay,az,mx,my,mz;
	float q0q0 = q0*q0;
	float q0q1 = q0*q1;
	float q0q2 = q0*q2;
	float q0q3 = q0*q3;
	float q1q1 = q1*q1;
	float q1q2 = q1*q2;
	float q1q3 = q1*q3;
	float q2q2 = q2*q2;   
	float q2q3 = q2*q3;
	float q3q3 = q3*q3;  
  volatile float        exInt, eyInt, ezInt;   	
  MPU6500_Read();
	gx = mpu6500_real_data.Gyro_X;
	gy = mpu6500_real_data.Gyro_Y;
	gz = mpu6500_real_data.Gyro_Z;
	ax = mpu6500_real_data.Accel_X;
	ay = mpu6500_real_data.Accel_Y;
	az = mpu6500_real_data.Accel_Z;
	mx = 0;
	my = 0;
	mz = 0;


	halfT       = (2/ 2000.0f);

	

	norm = inv_sqrt(ax*ax + ay*ay + az*az);       
	ax = ax * norm;
	ay = ay * norm;
	az = az * norm;
	
	#ifdef IST8310
		norm = inv_sqrt(mx*mx + my*my + mz*mz);          
		mx = mx * norm;
		my = my * norm;
		mz = mz * norm; 
	#else
		mx = 0;
		my = 0;
		mz = 0;		
	#endif

	hx = 2.0f*mx*(0.5f - q2q2 - q3q3) + 2.0f*my*(q1q2 - q0q3) + 2.0f*mz*(q1q3 + q0q2);
	hy = 2.0f*mx*(q1q2 + q0q3) + 2.0f*my*(0.5f - q1q1 - q3q3) + 2.0f*mz*(q2q3 - q0q1);
	hz = 2.0f*mx*(q1q3 - q0q2) + 2.0f*my*(q2q3 + q0q1) + 2.0f*mz*(0.5f - q1q1 - q2q2);         
	bx = sqrt((hx*hx) + (hy*hy));
	bz = hz; 
	

	vx = 2.0f*(q1q3 - q0q2);
	vy = 2.0f*(q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3;
	wx = 2.0f*bx*(0.5f - q2q2 - q3q3) + 2.0f*bz*(q1q3 - q0q2);
	wy = 2.0f*bx*(q1q2 - q0q3) + 2.0f*bz*(q0q1 + q2q3);
	wz = 2.0f*bx*(q0q2 + q1q3) + 2.0f*bz*(0.5f - q1q1 - q2q2);  
	

	ex = (ay*vz - az*vy) + (my*wz - mz*wy);
	ey = (az*vx - ax*vz) + (mz*wx - mx*wz);
	ez = (ax*vy - ay*vx) + (mx*wy - my*wx);


	if(ex != 0.0f && ey != 0.0f && ez != 0.0f)
	{
		exInt = exInt + ex * Ki * halfT;
		eyInt = eyInt + ey * Ki * halfT;	
		ezInt = ezInt + ez * Ki * halfT;
		
		gx = gx + Kp*ex + exInt;
		gy = gy + Kp*ey + eyInt;
		gz = gz + Kp*ez + ezInt;
	}
	
	tempq0 = q0 + (-q1*gx - q2*gy - q3*gz) * halfT;
	tempq1 = q1 + (q0*gx + q2*gz - q3*gy) * halfT;
	tempq2 = q2 + (q0*gy - q1*gz + q3*gx) * halfT;
	tempq3 = q3 + (q0*gz + q1*gy - q2*gx) * halfT;  


	norm = inv_sqrt(tempq0*tempq0 + tempq1*tempq1 + tempq2*tempq2 + tempq3*tempq3);
	q0 = tempq0 * norm;
	q1 = tempq1 * norm;
	q2 = tempq2 * norm;
	q3 = tempq3 * norm;	
	

	Yaw = -atan2(2*q1*q2 + 2*q0*q3, -2*q2*q2 - 2*q3*q3 + 1); 

	Pitch = -asin(-2*q1*q3 + 2*q0*q2);   

	Roll =  atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1);
}

*/
/*
void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az) {
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
	qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
	qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

		// Normalise accelerometer measurement
		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;   

		// Auxiliary variables to avoid repeated arithmetic
		_2q0 = 2.0f * q0;
		_2q1 = 2.0f * q1;
		_2q2 = 2.0f * q2;
		_2q3 = 2.0f * q3;
		_4q0 = 4.0f * q0;
		_4q1 = 4.0f * q1;
		_4q2 = 4.0f * q2;
		_8q1 = 8.0f * q1;
		_8q2 = 8.0f * q2;
		q0q0 = q0 * q0;
		q1q1 = q1 * q1;
		q2q2 = q2 * q2;
		q3q3 = q3 * q3;

		// Gradient decent algorithm corrective step
		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
		s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
		s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * (1.0f / sampleFreq);
	q1 += qDot2 * (1.0f / sampleFreq);
	q2 += qDot3 * (1.0f / sampleFreq);
	q3 += qDot4 * (1.0f / sampleFreq);

	// Normalise quaternion
	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
	
	Pitch = asin(2 * q1 * q3 + 2 * q0 * q2) ;
  Roll = atan2(2 * q1 * q2 - 2 * q0 * q3, 2 * q0 * q0 + 2 * q1 * q1 - 1) ;
  Yaw = -atan2(2 * q2 * q3 - 2 * q0 * q1, 2 * q0 * q0 + 2 * q3 * q3 - 1) ;//µÃµ½½Ç¶È
}

*/
float invSqrt(float num) {
	float halfnum = 0.5f * num;
	float y = num;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfnum * y * y));
	return y;
}
