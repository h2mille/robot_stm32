#include <driver.h>
#include <math.h>


uint16_t TimerPeriod = 0;
uint16_t ChannelPulse = 0;
uint32_t tempChannelPulse;
uint16_t Channel1Pulse = 0, Channel2Pulse = 0, Channel3Pulse = 0, Channel4Pulse = 0;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

void Motor_power(Motor ID, int power)
{
     /* Compute the value to be set in ARR regiter to generate signal frequency at 17.57 Khz */
     TimerPeriod = (SystemCoreClock / 17570 ) - 1;
     if ( power<0 ){
    	 tempChannelPulse = ((1000 + power) * (TimerPeriod - 1)) / 1000;
    	 ChannelPulse = (uint16_t) tempChannelPulse;
    	 GPIO_WriteBit(ID.sens->GPIO, ID.sens->GPIO_Pin, Bit_SET);
     }else{
    	 tempChannelPulse = ((power) * (TimerPeriod - 1)) / 1000;
    	 ChannelPulse = (uint16_t) tempChannelPulse;
    	 GPIO_WriteBit(ID.sens->GPIO, ID.sens->GPIO_Pin, Bit_RESET);
     }




     /* Channel 1, 2,3 and 4 Configuration in PWM mode */
     TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
     TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
     TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
     TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
     TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
     TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
     TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

     TIM_OCInitStructure.TIM_Pulse = ChannelPulse;
     switch (ID.channel){
         case (Channel)Ch1:
             TIM_OC1Init(ID.Timer, &TIM_OCInitStructure);
             break;
         case (Channel)Ch2:
             TIM_OC2Init(ID.Timer, &TIM_OCInitStructure);
             break;
         case (Channel)Ch3:
             TIM_OC3Init(ID.Timer, &TIM_OCInitStructure);
             break;
         case (Channel)Ch4:
             TIM_OC4Init(ID.Timer, &TIM_OCInitStructure);
             break;
         default:
             break;
     }

     /* TIM1 counter enable */
     TIM_Cmd(TIM1, ENABLE);

     /* TIM1 Main Output Enable */
     TIM_CtrlPWMOutputs(TIM1, ENABLE);


}

void TIM_main_Init(void)
{
     TIM_DeInit(TIM1);
     TIM_DeInit(TIM2);
     TIM_DeInit(TIM3);
     TimerPeriod = (SystemCoreClock / 17570 ) - 1;
     /* Time Base configuration */
     TIM_TimeBaseStructure.TIM_Prescaler = 0;
     TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
     TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
     TIM_TimeBaseStructure.TIM_ClockDivision = 0;
     TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;


     TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
     TIM_Cmd(TIM1, ENABLE);
	 TIM_CtrlPWMOutputs(TIM1, ENABLE);

	 TIM_TimeBaseStructure.TIM_Prescaler = 0;
	 TIM_TimeBaseStructure.TIM_Period = 65535; // Maximal
	 TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	 TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	 TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	 TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	 TIM_Cmd(TIM2, ENABLE);

	 TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	 TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	 TIM_Cmd(TIM3, ENABLE);

}

void USART3_main_Init(void)
{
	USART_ClockInitTypeDef USART_ClockInitStructure;
	USART_ClockStructInit(&USART_ClockInitStructure);
	USART_ClockInit(USART3, &USART_ClockInitStructure);

	/* Configure USART 9600 8N1 */
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART3, &USART_InitStructure);

	/* Enable USART3 */
	USART_Cmd(USART3, ENABLE);
}


