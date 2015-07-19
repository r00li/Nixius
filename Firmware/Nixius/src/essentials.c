/*
**
**                           essentials.c
**
**
**********************************************************************/
/*
   Author:                 Andrej Rolih
                           www.r00li.com
   Version:                0.1
   License:                GNU GPL v3
                           See attached LICENSE file for details
                           External library files do not include such header and
                           are released under GPL v3 or their specific license.
                           Check those files for more details.

**********************************************************************/

#include "essentials.h"





//******************************************************************************************
//TIMER FUNCTIONS

void init_TIM4()
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = UINT32_MAX;
    TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 1000000) -1; //(SYS_CLK / DELAY_TIM_FREQUENCY) - 1 -> delay time is 1MHz = 1us
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    //Disable timer if it is not needed
    TIM_Cmd(TIM2,ENABLE);
}


void init_nvic_tim4()
{
}

void delay(uint32_t ms)
{
    delay_micro(ms * 1000);
}

void delay_micro(uint32_t micro)
{
  uint32_t start = TIM_GetCounter(TIM2);

  /* use 16 bit count wrap around */
  while ((uint32_t)(TIM_GetCounter(TIM2) - start) <= micro) {}
}

unsigned int millis()
{
    return TIM_GetCounter(TIM2)/1000;
}

unsigned int micros()
{
    return TIM_GetCounter(TIM2);
}

void init_essentials_time()
{
    init_TIM4();
    init_nvic_tim4();
}
