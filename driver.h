#ifndef DRIVER_H
#define DRIVER_H

#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
typedef enum Channel Channel;
enum Channel {
	Ch1,Ch2,Ch3,Ch4
};

typedef struct
{
	GPIO_TypeDef* GPIO;
	uint16_t GPIO_Pin;
} PIN;

typedef struct
{
	TIM_TypeDef * Timer;
	PIN* sens;
	Channel channel;
} Motor;

void Motor_power(Motor ID, int power);

void TIM_main_Init(void);
void USART3_main_Init(void);



#endif
