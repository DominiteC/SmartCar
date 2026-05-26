#include "PID.h"

MotorPID FLMotorPID;                         //左电机PID
MotorPID FRMotorPID;                         //右电机PID
MotorPID BMotorPID;                         //右电机PID

AnglePID FLMotorpid;
AnglePID FRMotorpid;
AnglePID BMotorpid;

AnglePID Angle_pid;

AnglePID Gyroz_PID;
AnglePID Turn_PID;

int32 FLmotor_PWM_out;
int32 FRmotor_PWM_out;
int32 Bmotor_PWM_out;

//int SetSpeed=-220;                                //差速前的设置速度
//int FLSetSpeed=-220;                               //左电机设置速度
//int FRSetSpeed=-220;                               //右电机设置速度
//int BSetSpeed=-220;                               //右电机设置速度

int SetSpeed=-MotorSetSpeed;                                //差速前的设置速度
int FLSetSpeed=-MotorSetSpeed;                               //左电机设置速度
int FRSetSpeed=-MotorSetSpeed;                               //右电机设置速度
int BSetSpeed=-MotorSetSpeed;                               //右电机设置速度

 
void PID_init(AnglePID *PID, float Kp, float Ki, float Kd ,float OutputMin ,float OutputMax)
{
	PID->Kp = Kp;
	PID->Ki = Ki;
	PID->Kd = Kd;
	PID->err_last = 0;
	PID->Integral = 0;
	PID->pwm_out = 0;
	PID->Voltage = 0;
	PID->OutputMin = OutputMin;
	PID->OutputMax = OutputMax;

}

void Incremental_PID_Init(MotorPID *PID, float Kp, float Ki, float Kd )
{
	PID->Kp = Kp;
	PID->Ki = Ki;
	PID->Kd = Kd;
	PID->ActualSpeed = 0;
  PID->SetSpeed = 0;
  PID->err_last = 0;
  PID->err_lalast = 0;
  PID->Integral = 0;
  PID->pwm_out = 0;

}

void PID_Init(void)
{
		//前左电机参数//前左电机PID初始化
		Incremental_PID_Init(&FLMotorPID,-5.1,-0.18,-2.5);
	
    //前右电机参数 //前右电机PID初始化
		Incremental_PID_Init(&FRMotorPID,-5.1,-0.18,-2.5);

		//后电机参数//后电机PID初始化
		Incremental_PID_Init(&BMotorPID,-5.1,-0.18,-2.5);

		PID_init(&Angle_pid,0.44f,0,2,50.0f,-50.0f);
	
		PID_init(&FLMotorpid,-0.001f,-0.00005f,-0.02f,0,0);

		PID_init(&FLMotorpid,-0.001f,-0.00005f,-0.02f,0,0);
		PID_init(&FRMotorpid,-0.001f,-0.00005f,-0.02f,0,0);
		PID_init(&BMotorpid,-0.001f,-0.00005f,-0.02f,0,0);
    PID_init(&Turn_PID,3.55f,0,0.6f,500,-500);   
    PID_init(&Gyroz_PID,1.75f,0,0.25f,200,-200);   

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     电机速度设定
// 参数说明     void
// 返回参数     无
// 使用示例     Speed_Change();
// 备注信息     该函数会设定电机在各种情况下的速度
//							目前没用到
//-------------------------------------------------------------------------------------------------------------------
void Speed_Change(void)
{
//    //正常行驶
//    if(SpecialSpeed_flag == 0)
//    {
//        SetSpeed = (int)(MaxSpeed * Cross_Col_Per);

//        //保持保底速度前进
//        if(SetSpeed <= MinSpeed)
//        {
//            SetSpeed = MinSpeed;
//        }
//    }
//    //特殊速度标志位立起
//    else
//    {
//        SetSpeed = SpecialSpeed;
//    }

    //保持保底速度前进
    if(SetSpeed <= MinSpeed)
    {
        SetSpeed = MinSpeed;
    }
    //保持保底速度前进
    if(SetSpeed >= MaxSpeed)
    {
        SetSpeed = MaxSpeed;
    }

//    //停车标志位立起
//    if(Stop_flag == 1)
//    {
//        SetSpeed = 0;
//    }
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     电机PID控制(增量式)
// 参数说明     无
// 返回参数     void
// 使用示例     MotorPID_Control();
// 备注信息     将本函数放置于周期中断中
//-------------------------------------------------------------------------------------------------------------------
void MotorPID_Calculate(MotorPID * pid,float error)
{
	float P = 0.0f, I=0.0f, D=0.0f;
	
	static float Increment = 0;//增量
	
	pid->err = error;//获取误差
	
	
	P = pid->err - pid->err_last;//当前误差-上一次误差
	I = pid->err;
	
	D = pid->err - 2*pid->err_last + pid->err_lalast;

	pid->Voltage = pid->Kp*P + pid->Ki*I + pid->Kd*D;
	
	pid->err_lalast = pid->err_last;
	pid->err_last = pid->err;
	
	
	pid->pwm_out += 0.5*pid->Voltage;
	    //电机限幅
    if(pid->pwm_out > 0 )
    {
			if(pid->pwm_out >MaxSpeed)pid->pwm_out = MaxSpeed;
//			if(pid->pwm_out <MinSpeed)pid->pwm_out = MinSpeed;

    }
    else if(pid->pwm_out < 0)
    {
			if(pid->pwm_out <-MaxSpeed)pid->pwm_out = -MaxSpeed;
//			if(pid->pwm_out >-MinSpeed)pid->pwm_out = -MinSpeed;
    }


}

///**@brief     位置式PID获取值
//-- @param     Pid_TypeDef *PID PID结构体
//-- @param     float error 误差值
//-- @return    float outPut 输出
//-- @author    陈锦辉
//-- @date      2023/9/6
//**/
//void GetPIDValue(AnglePID *PID, int error)
//{
//	float P = 0.0f, D=0.0f;
//	static float I=0.0f;
//	
//	P = error;
//	I = PID->Integral = PID->Integral + error;
//	D = error - PID->err_last;

//	PID->Voltage = PID->Kp * P+ PID->Ki * PID->Integral+ PID->Kd * D;
//	PID->err_last = error;
//	
//    if(PID->pwm_out > 1000)
//    {
//        PID->pwm_out = 1000;
//    }
//    else if(PID->pwm_out < -1000)
//    {
//        PID->pwm_out = -1000;
//    }
//	PID->pwm_out += 0.5*PID->Voltage;

//	  if(PID->pwm_out > 1000)
//    {
//        PID->pwm_out = 1000;
//    }
//    else if(PID->pwm_out < -1000)
//    {
//        PID->pwm_out = -1000;
//    }

//}

/**@brief     位置式PID获取值
-- @param     Pid_TypeDef *PID PID结构体
-- @param     float error 误差值
-- @return    float outPut 输出
-- @author    陈锦辉
-- @date      2023/9/6
**/
float GetPIDValue(AnglePID *PID, int error)
{
	float P = 0.0f, D=0.0f;
	static float I=0.0f;

	P = error;
	PID->Integral = PID->Integral + error;
	D = error - PID->err_last;

	PID->Voltage = PID->Kp * P+ PID->Ki * PID->Integral+ PID->Kd * D;
	PID->err_last = error;
	
	PID->pwm_out += 0.5*PID->Voltage;
	
	if(PID->pwm_out > PID->OutputMax)
	{
		PID->pwm_out = PID->OutputMax;
	}
	else if(PID->pwm_out < PID->OutputMin)
	{
		PID->pwm_out = PID->OutputMin;
	}

	return PID->pwm_out;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     电机PID控制(增量式)
// 参数说明     无
// 返回参数     void
// 使用示例     MotorPID_Control();
// 备注信息     将本函数放置于周期中断中
//-------------------------------------------------------------------------------------------------------------------
void MotorPID_Control(void)
{
    //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
    register int32 FLMotorPID_error,FRMotorPID_error,BMotorPID_error;                     //当前误差

				Get_Speed(&FL_Motor,A_FL);                                                        //周期获取电机的速度
				Get_Speed(&FR_Motor,A_FR);                                                        //周期获取电机的速度
				Get_Speed(&B_Motor,A_B);                                                        //周期获取电机的速度
			  X_Speed = Get_X_Speed();
		 	  Y_Speed = Get_Y_Speed();
			
	if(Start)
	{
		if(Box_State != No_box)
		{	
			Box_Car_Speed(Car.Speed_X,Car.Speed_Y,Car.Speed_Z);
			if((Turn.Offset == 0&&Move.Offset == 0) && Car.Speed_X ==0&&Car.Speed_Y==0&&Car.Speed_Z==0){}
			else if(Car.Speed_X ==0&&Car.Speed_Y==0&&Car.Speed_Z==0 && (Turn.Offset != 0||Move.Offset != 0))
			{
				FLMotorPID.pwm_out = 0;
				FRMotorPID.pwm_out = 0;
				BMotorPID.pwm_out  = 0;
			}

		}
		else
		{
			Car_run(SetSpeed);
			Box_Car_Speed(Car.Speed_X,Car.Speed_Y,Car.Speed_Z);
		}		

			MotorPID_Calculate(&FLMotorPID,FLSetSpeed-FL_Motor.new_speed);
			MotorPID_Calculate(&FRMotorPID,FRSetSpeed-FR_Motor.new_speed);
			MotorPID_Calculate(&BMotorPID,BSetSpeed-B_Motor.new_speed);
	
			Motor_Set(FLMotorPID.pwm_out, FRMotorPID.pwm_out, BMotorPID.pwm_out);

	}
	else
	{
		Box_Car_Speed(0,0,0);
		MotorPID_Calculate(&FLMotorPID,FLSetSpeed-FL_Motor.new_speed);
		MotorPID_Calculate(&FRMotorPID,FRSetSpeed-FR_Motor.new_speed);
		MotorPID_Calculate(&BMotorPID,BSetSpeed-B_Motor.new_speed);
	
		Motor_Set(FLMotorPID.pwm_out, FRMotorPID.pwm_out, BMotorPID.pwm_out);

	}

	
//-------------------------------调试用---------------------------------------
//		Run_State(State);
//		V_Car_V_Wheel(Angle);

//	Box_Car_Speed(0,0,200);
//	MotorPID_Calculate(&FLMotorPID,FLSetSpeed-FL_Motor.new_speed);
//	MotorPID_Calculate(&FRMotorPID,FRSetSpeed-FR_Motor.new_speed);
//	MotorPID_Calculate(&BMotorPID,BSetSpeed-B_Motor.new_speed);
//	
//	Motor_Set(FLMotorPID.pwm_out, FRMotorPID.pwm_out, BMotorPID.pwm_out);


//		Motor_Set(800 , 800, 800);//调试用
//-------------------------------调试用---------------------------------------
}



