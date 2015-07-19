/*
**
**                           essentials.h
**
**
**********************************************************************/
/*
   Helper timing functions for basic timing and control
   Author:                 Andrej Rolih
                           www.r00li.com
   Version:                0.1

**********************************************************************/

#ifndef essentials_h
#define essentials_h

#include "stm32f0xx_conf.h"


void init_essentials_time();
void delay(uint32_t ms);
void delay_micro(uint32_t micro);

unsigned int millis();
unsigned int micros();

#endif
