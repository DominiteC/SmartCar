#ifndef _PID_H_
#define	_PID_H_
#include "zf_common_headfile.h"

#define     MaxSpeed    4500        //电机的最大速度,范围 0 ~ 10000
#define     MinSpeed    600         //电机的最大速度,范围 0 ~ 10000
#define 		MotorSetSpeed   600         //设定轮子速度   单位mm/s


typedef struct 
{
		float SetSpeed; 		//定义设定值
		float ActualSpeed; 		//定义实际值
	
		float err; 				//定义偏差值
		float err_last; 		//定义上一个偏差值
		float err_lalast; 		//定义上一个偏差值
	
		float Kp,Ki,Kd; 		//定义比例、积分、微分系数
	
		float Voltage; 			//定义电压值（控制执行器的变量）
		float Integral; 		//定义积分值
	
		float pwm_out;			//电机的pwm输出
}MotorPID;

typedef struct 
{
	
		int err_last; 				//定义偏差值
	
		float Kp,Ki,Kd; 		//定义比例、积分、微分系数
	
		int Voltage; 			//定义电压值（控制执行器的变量）
		int Integral; 		//定义积分值
	
		int pwm_out;			//电机的pwm输出
		int OutputMin;
		int OutputMax;
	
}AnglePID;


extern MotorPID FLMotorPID; //电机的pid结构体
extern MotorPID FRMotorPID; //电机的pid结构体
extern MotorPID BMotorPID;  //电机的pid结构体

extern AnglePID FLMotorpid;
extern AnglePID FRMotorpid;
extern AnglePID BMotorpid;
extern AnglePID Angle_pid;
extern AnglePID Gyroz_PID;
extern AnglePID Turn_PID;

extern int32 FLmotor_PWM_out; //电机的pwm输出
extern int32 FRmotor_PWM_out; //电机的pwm输出
extern int32 Bmotor_PWM_out;  //电机的pwm输出

extern int SetSpeed;					//总车速度	
extern int FLSetSpeed;				//左前轮速度
extern int FRSetSpeed;	      //右前轮速度
extern int BSetSpeed;         //后轮速度


void PID_Init(void);
void MotorPID_Control(void);
void speedcontrol(void);
void MotorPID_Calculate(MotorPID* pid,float error);
float GetPIDValue(AnglePID *PID, int error);
//void GetPIDValue(AnglePID *PID, int error);

#endif