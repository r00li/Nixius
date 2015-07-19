/*
DHT 22 humidity and temperature sensor library

    Copyright (C) 2013  Fabio Angeletti - fabio.angeletti89@gmail.com

    See attached license.txt file

*/

/* Define to prevent recursive inclusion -----------------------------------*/
#ifndef __DHT22_H
#define __DHT22_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ----------------------------------------------------------------*/
#include "stm32f0xx.h"

/* defines -----------------------------------------------------------------*/
#define DHT22_DATA_PIN  GPIO_Pin_12
#define DHT22_GPIO		GPIOB
#define DHT22_TIM		TIM3

/* variables ---------------------------------------------------------------*/
uint8_t DHT22data[6];

/* functions prototypes ----------------------------------------------------*/
void DHT22TimerInit(void);

void DHT22pinIn(void);
void DHT22pinOut(void);
void DHT22_Init(void);
void DHT22_Read(void);
float DHT22getTemperature(void);
float DHT22getHumidity(void);
float DHT11getTemperature(void);
float DHT11getHumidity(void);
float convertCtoF(float cTemperature);

#ifdef __cplusplus
}
#endif

#endif
