#include "car.h"
#include "delay.h"

void TIM3_PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitSturct;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	

	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;

	GPIO_Init(GPIOB,&GPIO_InitStructure);

	
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=arr;
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	
	TIM_OCInitSturct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitSturct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitSturct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitSturct.TIM_Pulse=0;
	
	TIM_OC3Init(TIM3,&TIM_OCInitSturct);
	TIM_OC4Init(TIM3,&TIM_OCInitSturct);
	
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);
	
	TIM_Cmd(TIM3,ENABLE);
	TIM_SetCompare4(TIM3,150);//中点值

}

void TIM4_PWM_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitSturct;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	

	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;

	GPIO_Init(GPIOB,&GPIO_InitStructure);

	
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=arr;
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);
	
	
	TIM_OCInitSturct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitSturct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitSturct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitSturct.TIM_Pulse=0;
	
	TIM_OC4Init(TIM4,&TIM_OCInitSturct);
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_Cmd(TIM4,ENABLE);
	TIM_SetCompare4(TIM4,150);//中点值

}










