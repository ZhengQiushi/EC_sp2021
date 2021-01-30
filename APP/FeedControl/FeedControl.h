#ifndef _FEEDCONTROL_H
#define _FEEDCONTROL_H

#include "main.h"
extern int PIDOut_Feed;
void FeedJudge();
void PidFeedInit(void);
void FeedControlLoop(void);
void FeedControlInit(void);
void Shoot_target_Cal(void);
void TargetRenew(u8 flag);
void ShootMove(int SetPos);
void Shoot_Para_Moni(void);
#endif



