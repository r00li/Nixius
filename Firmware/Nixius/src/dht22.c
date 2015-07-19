/*
DHT 22 humidity and temperature sensor library

    Copyright (C) 2013  Fabio Angeletti - fabio.angeletti89@gmail.com

    See attached license.txt file

*/

/* includes --------------------------------------------------------- */
#include "dht22.h"

/* functions -------------------------------------------------------- */

/*
  configure DHT22_DATA as input
 */
void DHT22TimerInit(void) {
  uint16_t PrescalerValue = 0;

  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
  TIM_OCInitTypeDef         TIM_OCInitStructure;

  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_OCStructInit(&TIM_OCInitStructure);

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535 - 1;      //in uSecs
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_Cmd(TIM3, ENABLE);
}

/*
  configure DHT22_DATA as input
 */
void DHT22pinIn(void){
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin =  DHT22_DATA_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(DHT22_GPIO, &GPIO_InitStructure);
}


/*
  configure DHT22_DATA as output
 */
void DHT22pinOut(void){
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin =  DHT22_DATA_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(DHT22_GPIO, &GPIO_InitStructure);
}


void DHT22_Init(void){
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  DHT22pinOut();

  /* hold DHT22 in standby state */
  GPIO_SetBits(DHT22_GPIO, DHT22_DATA_PIN);
}


void DHT22_Read(void){
  uint8_t dataIndex = 0;
  uint8_t counter = 7;
  uint8_t currByte = 0;
  uint8_t index = 0;
  uint32_t startTime = 0;

  /* reset data holder */
  for(index=0; index < 6; index++){
    DHT22data[index] = 0x00;
  }

  /* mcu sends start signal to sensor */
  DHT22pinOut();
  GPIO_ResetBits(DHT22_GPIO, DHT22_DATA_PIN);
  TIM_SetCounter(DHT22_TIM, 0);

  /* wait for at least 20 mSecs */
  while(TIM_GetCounter(DHT22_TIM) < 20000);

  /* switch to input and wait for sensor response */
  DHT22pinIn();
  while(GPIO_ReadInputDataBit(DHT22_GPIO, DHT22_DATA_PIN));

  /* DHT22 sends response signal */
  while(!GPIO_ReadInputDataBit(DHT22_GPIO, DHT22_DATA_PIN));
  while(GPIO_ReadInputDataBit(DHT22_GPIO, DHT22_DATA_PIN));

  /* DHT22 sends, finally, data */
  for (dataIndex=0; dataIndex<40; dataIndex++) {
    TIM_SetCounter(DHT22_TIM, 0);
    while(!GPIO_ReadInputDataBit(DHT22_GPIO, DHT22_DATA_PIN));
    startTime = TIM_GetCounter(DHT22_TIM);
    while(GPIO_ReadInputDataBit(DHT22_GPIO, DHT22_DATA_PIN));

    if ((TIM_GetCounter(DHT22_TIM) - startTime) > 40){
      DHT22data[currByte] |= (1 << counter);
    }

    if (counter == 0) {   // next byte?
      counter = 7;        // restart at MSB
      currByte++;         // next byte!
    } else {
      counter--;
    }

  }
}


float DHT22getTemperature(void){
  float retVal;

  retVal = DHT22data[2];
  retVal = DHT22data[2] & 0x7F;
  retVal *= 256;
  retVal += DHT22data[3];
  retVal /= 10;

  if (DHT22data[2] & 0x80)
    retVal *= -1;

  return retVal;
}


float DHT22getHumidity(void){
  float retVal;

  retVal = DHT22data[0];
  retVal *= 256;
  retVal += DHT22data[1];
  retVal /= 10;

  return retVal;
}

float DHT11getTemperature(void){
  float retVal;

  retVal = DHT22data[2];

  return retVal;
}


float DHT11getHumidity(void){
  float retVal;

  retVal = DHT22data[0];

  return retVal;
}


float convertCtoF(float cTemperature){
  return cTemperature * 9 / 5 + 32;
}
