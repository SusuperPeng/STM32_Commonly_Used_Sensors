#ifndef __Little_Pass_H
#define __Little_Pass_H
 
#include "stm32f10x.h"

extern char new_flag;

float Filter_RC_Simple(float NEW_DATA,float OLD_DATA);
float Filter_RC(float NEW_DATA,float OLD_DATA,char filter_flag);

#endif
