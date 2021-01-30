#ifndef _FRICTIONCONTROL_H
#define _FRICTIONCONTROL_H

#include "main.h"
#define Friction_OFF 0
#define Friction_ON  1
void FrictionJudge(void);
void FrictionControlLoop(void);
void PidFrictionInit(void);
void FrictionControlInit(void);
void Cover_Moni(void);

#endif