/*
**
**                           Main.c
**
**
**********************************************************************/
/*

 Nixius
 Created by: Andrej Rolih (www.r00li.com)
 For licensing see included License file
 Revision 1.0

**********************************************************************/

#include "stm32f0xx_conf.h"
#include "essentials.h"
#include "dht22.h"

#define Pin_H1_0 GPIOC,GPIO_Pin_10
#define Pin_H1_1 GPIOC,GPIO_Pin_12
#define Pin_H1_2 GPIOA,GPIO_Pin_15

#define Pin_H2_0 GPIOF,GPIO_Pin_5
#define Pin_H2_1 GPIOF,GPIO_Pin_4
#define Pin_H2_2 GPIOC,GPIO_Pin_8
#define Pin_H2_3 GPIOA,GPIO_Pin_8
#define Pin_H2_4 GPIOA,GPIO_Pin_9
#define Pin_H2_5 GPIOA,GPIO_Pin_11
#define Pin_H2_6 GPIOA,GPIO_Pin_12
#define Pin_H2_7 GPIOA,GPIO_Pin_10
#define Pin_H2_8 GPIOC,GPIO_Pin_9
#define Pin_H2_9 GPIOA,GPIO_Pin_3

#define Pin_M1_0 GPIOB,GPIO_Pin_4
#define Pin_M1_1 GPIOB,GPIO_Pin_5
#define Pin_M1_2 GPIOB,GPIO_Pin_6
#define Pin_M1_3 GPIOB,GPIO_Pin_3
#define Pin_M1_4 GPIOC,GPIO_Pin_11
#define Pin_M1_5 GPIOD,GPIO_Pin_2

#define Pin_M2_0 GPIOB,GPIO_Pin_2
#define Pin_M2_1 GPIOB,GPIO_Pin_1
#define Pin_M2_2 GPIOC,GPIO_Pin_5
#define Pin_M2_3 GPIOA,GPIO_Pin_7
#define Pin_M2_4 GPIOA,GPIO_Pin_6
#define Pin_M2_5 GPIOA,GPIO_Pin_5
#define Pin_M2_6 GPIOA,GPIO_Pin_4
#define Pin_M2_7 GPIOB,GPIO_Pin_10
#define Pin_M2_8 GPIOC,GPIO_Pin_4
#define Pin_M2_9 GPIOB,GPIO_Pin_0

#define Tube_H1 0
#define Tube_H2 1
#define Tube_M1 2
#define Tube_M2 3

#define Pin_Button_1 GPIOB,GPIO_Pin_13
#define Pin_Button_2 GPIOB,GPIO_Pin_14
#define Pin_Button_3 GPIOB,GPIO_Pin_15

#define State_Clock 0
#define State_Off 1
#define State_Temperature 2

// Temeprature sensor settings - enable the sensor and select sensor type
//#define TEMP_SENSOR_ENABLE 1
//#define TEMP_SENSOR_DHT11 1
#define TEMP_SENSOR_DHT22 1

volatile int button1Flag = 0;
volatile int button2Flag = 0;
volatile int button3Flag = 0;
volatile unsigned long button1PressStart = 0;
volatile unsigned long button2PressStart = 0;
volatile unsigned long button3PressStart = 0;

volatile unsigned long secondsPassed = 0;

// State configuration settings
int state = State_Clock;
int autoPowerOnEnabled;
int autoPowerOnHours;
int autoPowerOnMinutes;
int autoPowerOffHours;
int autoPowerOffMinutes;


void EXTI4_15_IRQHandler()
{
    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        delay(100);
        if (GPIO_ReadInputDataBit(Pin_Button_1) == RESET)
        {
            button1Flag = 1;
            button1PressStart = millis();
        }
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    else if (EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
        delay(100);
        if (GPIO_ReadInputDataBit(Pin_Button_2) == RESET)
        {
            button2Flag = 1;
            button2PressStart = millis();
        }
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    else if (EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        delay(100);
        if (GPIO_ReadInputDataBit(Pin_Button_3) == RESET)
        {
            button3Flag = 1;
            button3PressStart = millis();
        }
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}

void RTC_IRQHandler()
{
    if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
    {
        secondsPassed++;

        RTC_ClearITPendingBit(RTC_IT_ALRA);
        EXTI_ClearITPendingBit(EXTI_Line17);
    }
}

void blankTube(int tube)
{
    if (tube == Tube_H1)
    {
        GPIO_ResetBits(Pin_H1_0);
        GPIO_ResetBits(Pin_H1_1);
        GPIO_ResetBits(Pin_H1_2);
    }
    else if (tube == Tube_H2)
    {
        GPIO_ResetBits(Pin_H2_0);
        GPIO_ResetBits(Pin_H2_1);
        GPIO_ResetBits(Pin_H2_2);
        GPIO_ResetBits(Pin_H2_3);
        GPIO_ResetBits(Pin_H2_4);
        GPIO_ResetBits(Pin_H2_5);
        GPIO_ResetBits(Pin_H2_6);
        GPIO_ResetBits(Pin_H2_7);
        GPIO_ResetBits(Pin_H2_8);
        GPIO_ResetBits(Pin_H2_9);
    }
    else if (tube == Tube_M1)
    {
        GPIO_ResetBits(Pin_M1_0);
        GPIO_ResetBits(Pin_M1_1);
        GPIO_ResetBits(Pin_M1_2);
        GPIO_ResetBits(Pin_M1_3);
        GPIO_ResetBits(Pin_M1_4);
        GPIO_ResetBits(Pin_M1_5);
    }
    else if (tube == Tube_M2)
    {
        GPIO_ResetBits(Pin_M2_0);
        GPIO_ResetBits(Pin_M2_1);
        GPIO_ResetBits(Pin_M2_2);
        GPIO_ResetBits(Pin_M2_3);
        GPIO_ResetBits(Pin_M2_4);
        GPIO_ResetBits(Pin_M2_5);
        GPIO_ResetBits(Pin_M2_6);
        GPIO_ResetBits(Pin_M2_7);
        GPIO_ResetBits(Pin_M2_8);
        GPIO_ResetBits(Pin_M2_9);
    }
}

void displayNumber(int tube, int number)
{
    blankTube(tube);

    if (tube == Tube_H1)
    {
        switch (number)
        {
            case 0: GPIO_SetBits(Pin_H1_0); break;
            case 1: GPIO_SetBits(Pin_H1_1); break;
            case 2: GPIO_SetBits(Pin_H1_2); break;
        }
    }
    else if (tube == Tube_H2)
    {
        switch (number)
        {
            case 0: GPIO_SetBits(Pin_H2_0); break;
            case 1: GPIO_SetBits(Pin_H2_1); break;
            case 2: GPIO_SetBits(Pin_H2_2); break;
            case 3: GPIO_SetBits(Pin_H2_3); break;
            case 4: GPIO_SetBits(Pin_H2_4); break;
            case 5: GPIO_SetBits(Pin_H2_5); break;
            case 6: GPIO_SetBits(Pin_H2_6); break;
            case 7: GPIO_SetBits(Pin_H2_7); break;
            case 8: GPIO_SetBits(Pin_H2_8); break;
            case 9: GPIO_SetBits(Pin_H2_9); break;
        }
    }
    else if (tube == Tube_M1)
    {
        switch (number)
        {
            case 0: GPIO_SetBits(Pin_M1_0); break;
            case 1: GPIO_SetBits(Pin_M1_1); break;
            case 2: GPIO_SetBits(Pin_M1_2); break;
            case 3: GPIO_SetBits(Pin_M1_3); break;
            case 4: GPIO_SetBits(Pin_M1_4); break;
            case 5: GPIO_SetBits(Pin_M1_5); break;
        }
    }
    else if (tube == Tube_M2)
    {
        switch (number)
        {
            case 0: GPIO_SetBits(Pin_M2_0); break;
            case 1: GPIO_SetBits(Pin_M2_1); break;
            case 2: GPIO_SetBits(Pin_M2_2); break;
            case 3: GPIO_SetBits(Pin_M2_3); break;
            case 4: GPIO_SetBits(Pin_M2_4); break;
            case 5: GPIO_SetBits(Pin_M2_5); break;
            case 6: GPIO_SetBits(Pin_M2_6); break;
            case 7: GPIO_SetBits(Pin_M2_7); break;
            case 8: GPIO_SetBits(Pin_M2_8); break;
            case 9: GPIO_SetBits(Pin_M2_9); break;
        }
    }
}

int numbericInput(int tubeConfig, int minimum, int maximum)
{
    int number = minimum;
    char buffer[4];

    blankTube(Tube_H1);
    blankTube(Tube_H2);
    blankTube(Tube_M1);
    blankTube(Tube_M2);

    while (button3Flag != 1)
    {
        if (button1Flag > 0)
        {
            number++;
            number = (number > maximum) ? minimum : number;
            button1Flag = 0;
        }

        if (button2Flag > 0)
        {
            number--;
            number = (number < minimum) ? maximum : number;
            button2Flag = 0;
        }

        if (tubeConfig == 1)
        {
            blankTube(Tube_H1);
            blankTube(Tube_H2);
            delay(300);
            sprintf(buffer,"%02d", number);
            displayNumber(Tube_H1, buffer[0]-48);
            displayNumber(Tube_H2, buffer[1]-48);
            delay(300);
        }
        else if (tubeConfig == 2)
        {
            blankTube(Tube_M1);
            blankTube(Tube_M2);
            delay(300);
            sprintf(buffer,"%02d", number);
            displayNumber(Tube_M1, buffer[0]-48);
            displayNumber(Tube_M2, buffer[1]-48);
            delay(300);
        }
        else if (tubeConfig == 3)
        {
            blankTube(Tube_H2);
            delay(300);
            sprintf(buffer,"%01d", number);
            displayNumber(Tube_H2, buffer[0]-48);
            delay(300);
        }

    }

    button3Flag = 0;

    return number;
}

void initRTC(int restart)
{
    //
    // Initializes the internal RTC on the micro
    //

    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

    if (restart)
    {
        //Do this part only if the user chose to set the date/time
        RCC_BackupResetCmd(ENABLE);
        RCC_BackupResetCmd(DISABLE);

        //RCC_LSICmd(ENABLE);
        RCC_LSEConfig(RCC_LSE_ON);

        /* Wait until LSE is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

        /* RTC Clock Source Selection */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        /* Enable the RTC */
        RCC_RTCCLKCmd(ENABLE);

        RTC_WaitForSynchro();

        RTC_InitTypeDef RTC_InitStruct;
        RTC_StructInit(&RTC_InitStruct);
        RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
        RTC_Init(&RTC_InitStruct);
    }
    else
    {
        RTC_WaitForSynchro();
    }
}

void setRTC()
{
    int hours = numbericInput(1, 0, 23);
    int minutes = numbericInput(2, 0, 59);

    initRTC(1);

    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_TimeStruct.RTC_Hours = hours;
    RTC_TimeStruct.RTC_Minutes = minutes;
    RTC_TimeStruct.RTC_Seconds = 0;
    RTC_SetTime(RTC_Format_BIN,&RTC_TimeStruct);

    RTC_DateTypeDef RTC_DateStruct;
    RTC_DateStruct.RTC_WeekDay = 1;
    RTC_DateStruct.RTC_Date = 1;
    RTC_DateStruct.RTC_Month = 1;
    RTC_DateStruct.RTC_Year = 15;
    RTC_SetDate(RTC_Format_BIN,&RTC_DateStruct);
}

void initRTCAlarm()
{
    //
    // Sets the RTC alarm that is used to toggle between clock/temperature display
    //

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable the RTC Alarm Interrupt */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    PWR_BackupAccessCmd(ENABLE);
    RTC_WriteProtectionCmd(DISABLE);

    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

    RTC_AlarmTypeDef alrm;
    alrm.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
    alrm.RTC_AlarmMask = RTC_AlarmMask_All;


    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &alrm);
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

    /* Enable Alarm interrupt */
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);

    RTC_WriteProtectionCmd(ENABLE);
}

void setAutoPower()
{
    autoPowerOnEnabled = numbericInput(3, 0, 1);
    RTC_WriteBackupRegister(RTC_BKP_DR0, autoPowerOnEnabled);
    if (!autoPowerOnEnabled)
    {
        return;
    }

    autoPowerOffHours = numbericInput(1, 0, 23);
    autoPowerOffMinutes = numbericInput(2, 0, 59);
    RTC_WriteBackupRegister(RTC_BKP_DR3, autoPowerOffHours);
    RTC_WriteBackupRegister(RTC_BKP_DR4, autoPowerOffMinutes);

    autoPowerOnHours = numbericInput(1, 0, 23);
    autoPowerOnMinutes = numbericInput(2, 0, 59);
    RTC_WriteBackupRegister(RTC_BKP_DR1, autoPowerOnHours);
    RTC_WriteBackupRegister(RTC_BKP_DR2, autoPowerOnMinutes);
}

void initButtons()
{
    //
    // Configures the EXTI and NVIC to receive interrupts
    //

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource14);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);

    EXTI_InitTypeDef EXTI_struct;
    EXTI_struct.EXTI_Line = EXTI_Line13 | EXTI_Line14 | EXTI_Line15;
    EXTI_struct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_struct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_struct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_struct);

    //Enable NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

int main(void)
{
    init_essentials_time();
    initRTC(0);
    initRTCAlarm();
    initButtons();
    DHT22TimerInit();

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure_A;
    GPIO_InitStructure_A.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure_A.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure_A.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure_A.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_InitTypeDef GPIO_InitStructure_B = GPIO_InitStructure_A;
    GPIO_InitTypeDef GPIO_InitStructure_C = GPIO_InitStructure_A;
    GPIO_InitTypeDef GPIO_InitStructure_D = GPIO_InitStructure_A;
    GPIO_InitTypeDef GPIO_InitStructure_F = GPIO_InitStructure_A;

    GPIO_InitStructure_A.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
    GPIO_InitStructure_B.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_10;
    GPIO_InitStructure_C.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure_D.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure_F.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;

    GPIO_Init(GPIOC, &GPIO_InitStructure_C);
    GPIO_Init(GPIOF, &GPIO_InitStructure_F);
    GPIO_Init(GPIOD, &GPIO_InitStructure_D);
    GPIO_Init(GPIOB, &GPIO_InitStructure_B);
    GPIO_Init(GPIOA, &GPIO_InitStructure_A);

    autoPowerOnEnabled = RTC_ReadBackupRegister(RTC_BKP_DR0);
    autoPowerOnHours = RTC_ReadBackupRegister(RTC_BKP_DR1);
    autoPowerOnMinutes = RTC_ReadBackupRegister(RTC_BKP_DR2);
    autoPowerOffHours = RTC_ReadBackupRegister(RTC_BKP_DR3);
    autoPowerOffMinutes = RTC_ReadBackupRegister(RTC_BKP_DR4);

    RTC_TimeTypeDef RTC_TimeStruct;
    char time_buff[4];
    unsigned int lastTempDisplay = millis() + 30000;
    int temperature = 0;

    while(1)
    {
        RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);

        if (state == State_Clock)
        {
            sprintf(time_buff,"%02d", RTC_TimeStruct.RTC_Hours);
            displayNumber(Tube_H1, time_buff[0]-48);
            displayNumber(Tube_H2, time_buff[1]-48);

            sprintf(time_buff,"%02d", RTC_TimeStruct.RTC_Minutes);
            displayNumber(Tube_M1, time_buff[0]-48);
            displayNumber(Tube_M2, time_buff[1]-48);
        }
        else if (state == State_Off)
        {
            blankTube(Tube_H1);
            blankTube(Tube_H2);
            blankTube(Tube_M1);
            blankTube(Tube_M2);
        }
        else if (state == State_Temperature)
        {
            DHT22_Read();

            #ifdef TEMP_SENSOR_DHT11
            temperature = DHT11getTemperature();
            #elif defined TEMP_SENSOR_DHT22
            temperature = DHT22getTemperature();
            #endif

            blankTube(Tube_H1);
            blankTube(Tube_M1);

            sprintf(time_buff,"%02d", temperature);
            displayNumber(Tube_M2, time_buff[0]-48);
            displayNumber(Tube_H2, time_buff[1]-48);

            delay(5000);

            state = State_Clock;
        }

        // Auto power button handling
        // Sets the time for auto power off and auto power down
        if (button1Flag == 1 && abs(millis()-button1PressStart) > 300 && GPIO_ReadInputDataBit(Pin_Button_1) == RESET)
        {
            button1Flag = 0;
            setAutoPower();
        }
        else if (button1Flag == 1 && GPIO_ReadInputDataBit(Pin_Button_1) != RESET)
        {
            button1Flag = 0;
        }

        // Display off button handling
        // Turns the display off or on when pressed - useful at nights
        // Button needs to be held down for more than 0.3s
        if (button2Flag == 1 && abs(millis()-button2PressStart) > 300 && GPIO_ReadInputDataBit(Pin_Button_2) == RESET)
        {
            state = (state == State_Clock || state == State_Temperature) ? State_Off : State_Clock;
            button2Flag = 0;
            delay(500);
        }
        else if (button2Flag == 1 && GPIO_ReadInputDataBit(Pin_Button_2) != RESET)
        {
            button2Flag = 0;
        }

        // Set time button handling
        // Button needs to be held down for more than 4s to set the time
        if (button3Flag == 1 && abs(millis()-button3PressStart) > 4000 && GPIO_ReadInputDataBit(Pin_Button_3) == RESET)
        {
            button3Flag = 0;
            setRTC();
            state = State_Clock;
        }
        else if (button3Flag == 1 && GPIO_ReadInputDataBit(Pin_Button_3) != RESET)
        {
            button3Flag = 0;
        }

        #ifdef TEMP_SENSOR_ENABLE
        if (secondsPassed > 30)
        {
            if (state == State_Clock)
            {
                state = State_Temperature;
            }
            secondsPassed = 0;
        }
        #endif

        if (autoPowerOnEnabled && autoPowerOffHours == RTC_TimeStruct.RTC_Hours && autoPowerOffMinutes == RTC_TimeStruct.RTC_Minutes && RTC_TimeStruct.RTC_Seconds < 15)
        {
            state = State_Off;
        }
        if (autoPowerOnEnabled && autoPowerOnHours == RTC_TimeStruct.RTC_Hours && autoPowerOnMinutes == RTC_TimeStruct.RTC_Minutes && RTC_TimeStruct.RTC_Seconds < 15)
        {
            state = State_Clock;
        }
    }
}

