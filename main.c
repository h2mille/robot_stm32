
#include <stdio.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x.h>
#include <misc.h>
#include <driver.h>
#include <CoOS.h>
#include <position.h>
#include <control.h>
#include <global.h>
#include <serial.h>



#define STACK_SIZE_TASKA 256                       /*!< Define "taskA" task size 128 bytes */
#define STACK_SIZE_TASKB 256                     /*!< Define "taskB" task size 128 bytes */
#define STACK_SIZE_TASKC 256                       /*!< Define "taskC" task size 128 bytes */
#define STACK_SIZE_SCHEDULER 128                       /*!< Define "taskC" task size 128 bytes */

void RCC_Configuration(void);
void GPIO_Configuration(void);
void IRQ_Init(void);

OS_MutexID Mutex_task_1;
OS_MutexID Mutex_task_2;
OS_MutexID Mutex_task_3;
extern OS_EventID Semaphore_rx;
OS_STK     taskA_stk[STACK_SIZE_TASKA];      /*!< Define "taskA" task stack */
OS_STK     taskB_stk[STACK_SIZE_TASKB];      /*!< Define "taskB" task stack */
OS_STK     taskC_stk[STACK_SIZE_TASKC];      /*!< Define "led" task stack   */
OS_STK     scheduler_stk[STACK_SIZE_SCHEDULER];      /*!< Define "led" task stack   */
static int red_led_state=0;
static int green_led_state=0;
PIN Pin_moteur_1 = { GPIOC, GPIO_Pin_1};
PIN Pin_moteur_2 = { GPIOC, GPIO_Pin_0};
Motor MOT1 = {TIM1, &Pin_moteur_1, Ch1};
Motor MOT2 = {TIM1, &Pin_moteur_2, Ch2};


void taskA (void* pdata)
{
// place your code here.
	int i=0;
	char character;
	while(1){
		character = USART_ReceiveData(USART3);
		red_led_state=!red_led_state;
		GPIO_WriteBit(GPIOC, GPIO_Pin_8, red_led_state ? Bit_SET : Bit_RESET);
		CoEnterMutexSection(Mutex_task_1);
//		short_printf("%c", character);
		update_position(TIM3->CNT, TIM2->CNT);
		i++;
	}
}

void taskB (void* pdata)
{
	while(1){
		CoEnterMutexSection(Mutex_task_2);
		GPIO_WriteBit(GPIOC, GPIO_Pin_9, green_led_state ? Bit_SET : Bit_RESET);
		green_led_state=!green_led_state;
		control(MOT1, MOT2);
	}
}

void taskC (void* pdata)
{
// place your code here.
	//get ready for half billion of year time to count until 2^64 at 1000 tick per second
	char command;
	char order;
	double value;
	while(1){
//		CoEnterMutexSection(Mutex_task_3);
//		scanf("%s",chaine);
//		short_printf("chaine: %s\n\r",chaine);
		CoPendSem(Semaphore_rx, 0);
		short_scanf(&command, &order, &value);
		short_printf ("%i %i %i \n\r", (int) (robot_position.x*1000), (int) (robot_position.y * 1000), (int) (robot_position.theta * 1000));
		short_printf ("%c %c %d \n\r", command, order, (int)(value *1000));
		short_printf ("Je suis le roi du monde!!!!!! :D \n\r");
//		short_printf ("1 2 3 \n\r", (int) (robot_position.x*1000), (int) (robot_position.y * 1000), (int) (robot_position.theta * 1000));

	}
}
void scheduler (void* pdata)
{
// place your code here.
	//get ready for half billion of year time to count until 2^64 at 1000 tick per second
	U64 timer_OS;
	int task_1_period = 10;
	int task_2_period = 10;
	int task_3_period = 100;
	while(1){
		CoTickDelay(1);
		timer_OS = CoGetOSTime ();
		if ( (U64) (timer_OS % task_1_period) == 0)
			CoLeaveMutexSection(Mutex_task_1);
		if ( (U64) (timer_OS % task_2_period) == 0)
			CoLeaveMutexSection(Mutex_task_2);
//		if ( (U64) (timer_OS % task_3_period) == 0)
//			CoLeaveMutexSection(Mutex_task_3);

	}

}
//Motor MOT3 = {TIM1,Ch3};
//Motor MOT4 = {TIM1,Ch4};


int main ()
{

   CoInitOS ();                           /*!< Initial CooCox CoOS          */
   RCC_Configuration();
   /* GPIO Configuration */
   GPIO_Configuration();
   TIM_main_Init();
   USART3_main_Init();
   IRQ_Init();
   init_global();
   Mutex_task_1 = CoCreateMutex( );
   Mutex_task_2 = CoCreateMutex( );
//   Mutex_task_3 = CoCreateMutex( );
   Semaphore_rx = CoCreateSem(0, (U16) 1<<15, EVENT_SORT_TYPE_FIFO);

   USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

   /*!< Create three tasks    */
   CoCreateTask (taskA,0,1,&taskA_stk[STACK_SIZE_TASKA-1],STACK_SIZE_TASKA);
   CoCreateTask (taskB,0,2,&taskB_stk[STACK_SIZE_TASKB-1],STACK_SIZE_TASKB);
   CoCreateTask (taskC,0,3,&taskC_stk[STACK_SIZE_TASKC-1],STACK_SIZE_TASKC);
   CoCreateTask (scheduler,0,63,&scheduler_stk[STACK_SIZE_SCHEDULER-1],STACK_SIZE_SCHEDULER);

   CoStartOS ();                                                  /*!< Start multitask               */
   while(1);


   while (1);                                                        /*!< The code can not reach here       */
}


void RCC_Configuration(void)
{
  /* TIM1, GPIOA, GPIOB, GPIOE and AFIO clocks enable */
	  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_USART3  , ENABLE);
      RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//TIM1 (motors)
	/* GPIOA Configuration: Channel 1, 2 and 3 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	//TIM2(encoder L)
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//TIM3(encoder R)
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;

	// Green+Red LEDs
	GPIO_Init(GPIOC, &gpio);
	gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_StructInit(&gpio);


	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_StructInit(&gpio);
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // Motor L
	GPIO_Init(GPIOC, &gpio);

	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART3 Rx (PB.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void IRQ_Init(void)
{
      NVIC_InitTypeDef NVIC_InitStructure;
	  /* Enable USART3 IRQ */
	  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
}

