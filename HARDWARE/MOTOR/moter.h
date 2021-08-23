#ifndef __MOTER_H
#define	__MOTER_H


#include "sys.h"

/*

#define motor1IN1 PBout(4) 
#define motor1IN2 PAout(7) 
#define motor2IN3 PBout(0) 
#define motor2IN4 PBout(1) 

void MOTOR_Init(void);//???

void MotorForward(void);//????
void MotorReverse(void);//????
void MotorStop(void);//????

*/

void TIM3_PWM_Init(void);
void CarGo(void);
void CarStop(void);
void CarBack(void);
void CarLeft(void);
void CarBigLeft(void);  //大右转
void CarRight(void);
void CarBigRight(void);   //大左转

#endif


