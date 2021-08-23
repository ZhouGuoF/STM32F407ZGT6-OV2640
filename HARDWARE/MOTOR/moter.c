#include "moter.h"
#include "sys.h"
/*
void MotorForward(void)//??
{
		motor1IN1=1;
		motor1IN2=0;
		motor2IN3=1;
		motor2IN4=0;
}
void MotorReverse(void)//??
{
		motor1IN1=0;
		motor1IN2=1;
		motor2IN3=0;
		motor2IN4=1;
}
void MotorStop(void)//??
{
		motor1IN1=0;
		motor1IN2=0;
		motor2IN3=0;
		motor2IN4=0;
}

void MOTOR_Init(void)//???
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
}
*/



void TIM3_PWM_Init(void)  //TIM3的pwm设置和相应的引脚设置
{
  GPIO_InitTypeDef GPIO_InitStructure;

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure; 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
  
	//端口复用 TIM3_CH1,2,3,4
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_TIM3); 
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM3); 
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3); 
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM3); 

  
  TIM_TimeBaseStructure.TIM_Period = 899;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseInit(TIM3 , &TIM_TimeBaseStructure);
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;   //初始化要用的A6/A7口
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_4;   //初始化要用的B0/B1口
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);   
  
  //PWM通道1
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = 900;
  TIM_OC1Init(TIM3 , &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3 , TIM_OCPreload_Enable);
  
  //PWM通道2
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = 900;
  TIM_OC2Init(TIM3 , &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM3 , TIM_OCPreload_Enable);
  
  //PWM通道3
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = 900;
  TIM_OC3Init(TIM3 , &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM3 , TIM_OCPreload_Enable);
  
  //PWM通道4
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_Pulse = 900;
  TIM_OC4Init(TIM3 , &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM3 , TIM_OCPreload_Enable);
  
  TIM_Cmd(TIM3 , ENABLE);
}


void CarGo(void)
{
  TIM_SetCompare1(TIM3 , 500);  //数值越大速度越慢
  TIM_SetCompare2(TIM3 , 900);
  TIM_SetCompare3(TIM3 , 500);  
  TIM_SetCompare4(TIM3 , 900);	
}

void CarStop(void)
{
  TIM_SetCompare1(TIM3 , 900);
  TIM_SetCompare2(TIM3 , 900);
  TIM_SetCompare3(TIM3 , 900);	
  TIM_SetCompare4(TIM3 , 900);
}

void CarBack(void)
{
  TIM_SetCompare1(TIM3 , 900);
  TIM_SetCompare2(TIM3 , 400);
  TIM_SetCompare3(TIM3 , 400);	
  TIM_SetCompare4(TIM3 , 300);
}

void CarLeft(void)
{
  TIM_SetCompare1(TIM3 , 900);
  TIM_SetCompare2(TIM3 , 350);
  TIM_SetCompare3(TIM3 , 350);
  TIM_SetCompare4(TIM3 , 900);
}

void CarBigLeft(void)
{
  TIM_SetCompare1(TIM3 , 900);
  TIM_SetCompare2(TIM3 , 200);
  TIM_SetCompare3(TIM3 , 200);
  TIM_SetCompare4(TIM3 , 900);
}

void CarRight(void)
{
  TIM_SetCompare1(TIM3 , 350);
  TIM_SetCompare2(TIM3 , 900);
  TIM_SetCompare3(TIM3 , 900);
  TIM_SetCompare4(TIM3 , 350);
  
}

void CarBigRight(void)
{
  TIM_SetCompare1(TIM3 , 200);
  TIM_SetCompare2(TIM3 , 900);
  TIM_SetCompare3(TIM3 , 900);
  TIM_SetCompare4(TIM3 , 200);
  
}
