#ifndef _EULER_H
#define _EULER_H

void init_euler(void);
void update_euler(void);
void update_euler_mpu(void);

extern float Yaw_gyro,Roll_gyro,Pitch_gyro;
extern float Yaw_mag,Roll_accel,Pitch_accel;
extern float Yaw,Roll,Pitch,Yaw_Offset,Pitch_Offset,Roll_Offset,Pitch_m;
extern float delay_speed;

#endif

