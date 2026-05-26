#ifndef _MOTOR_h_
#define _MOTOR_h_
#include "zf_common_headfile.h"
#include "zf_driver_pwm.h"

#define REDUCTION_RATIO 				4.4  		 /* 减速比4.4:1 */
#define Car_R        						28       //底盘轮子的半径28mm
#define Encoder_R    						13       //编码器和电机接驳的齿轮半径13mm


//可用PWM输出引脚pwm_channel_enum枚举在zf_driver_pwm.h
#define FLMotor  						 	 PWM2_MODULE0_CHA_C6 //左前轮pwm信号输出引脚 
#define FRMotor  						   PWM2_MODULE3_CHB_D3 //右前轮pwm信号输出引脚 
#define BMotor   						   PWM2_MODULE2_CHB_C11 //后轮pwm信号输出引脚 

//电机方向信号
#define FL_DIR     								C7//左前方向信号输出引脚 
#define FR_DIR     						 	  D2//右前方向信号输出引脚  
#define B_DIR	 	   							  C10//后轮方向信号输出引脚 
																										
																										
//编码器
//右前
#define FRENCODER                 	(QTIMER2_ENCODER1) 
#define FRENCODER_A               	(QTIMER2_ENCODER1_CH1_C3) 
#define FRENCODER_B               	(QTIMER2_ENCODER1_CH2_C4)

//左前
#define FLENCODER                  (QTIMER1_ENCODER1)       
#define FLENCODER_A                (QTIMER1_ENCODER1_CH1_C0)
#define FLENCODER_B                (QTIMER1_ENCODER1_CH2_C1)

//后轮
#define BENCODER                   (QTIMER1_ENCODER2)        
#define BENCODER_A                 (QTIMER1_ENCODER2_CH1_C2) 
#define BENCODER_B                 (QTIMER1_ENCODER2_CH2_C24)

//编码器滤波系数
//a的取值决定了算法的灵敏度
//a越大,新采集的值占的权重越大,算法越灵敏,但平顺性差;相反,a越小,新采集的值占的权重越小,灵敏度差,但平顺性好
#define A_FR                 0.90f           //前右电机A参数
#define A_FL                 0.93f           //前左电机A参数
#define A_B                 0.90f           //后电机A参数

typedef struct  motor//电机的各个速度
{
	int k;//用于计算Speed[10]的循环个数
	float Speed[10];
  float new_speed;
	float old_speed;
	int encoder;
	float distance;
}MotorStruct;

extern MotorStruct FL_Motor;
extern MotorStruct FR_Motor;
extern MotorStruct B_Motor;

extern float Encoder_Distance[4];

//电机控制信号前进与后退的值，需要反转时将以下枚举数值对调。
typedef enum
{
    back = 1,
    ahead = 0,
}sign_dir;



void Motor_Init(void);
void MyEncoder_Init(void);
void Motor_Set(int FLDuty, int FRDuty, int BDuty);
void Get_Speed(MotorStruct * Motor, float A);
float Get_X_Speed();
float Get_Y_Speed();

extern  float  X_Speed;
extern  float	 Y_Speed;


#endif