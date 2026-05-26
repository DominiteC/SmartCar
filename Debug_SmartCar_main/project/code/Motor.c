#include "Motor.h"

int FLSpeed = 0;                                     //前左电机的实际速度
int FRSpeed = 0;                                     //前右电机的实际速度
int BSpeed 	= 0;  																	 //后电机的实际速度

float  X_Speed = 0;
float	 Y_Speed = 0;
 
MotorStruct FL_Motor;
MotorStruct FR_Motor;
MotorStruct B_Motor;

float FL_LastValue;
float FR_LastValue;
float B_LastValue;

int16 Encoder_Pules_Buffer[4];//脉冲数组
int32 Distance_Buffer[4];//距离缓存数组
float Encoer_Speed[4];//速度数组,顺序为LF RF LB RB
float Encoder_Distance[4];//实际距离数组
const float Cm_Per = 0.0085859f;


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     电机控制信号初始化
// 参数说明     无
// 返回参数     void
// 使用示例     Sign_Init();
// 备注信息     对电机信号控制引脚初始化,默认高电平,推挽输出.
//-------------------------------------------------------------------------------------------------------------------
void Sign_Init(void)
{
    gpio_init(FL_DIR, GPO, GPIO_HIGH, GPO_PUSH_PULL);//方向初始化
    gpio_init(FR_DIR, GPO, GPIO_HIGH, GPO_PUSH_PULL);
		gpio_init(B_DIR,  GPO, GPIO_HIGH, GPO_PUSH_PULL);
}            


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     电机初始化
// 参数说明     无
// 返回参数     void
// 使用示例     Motor_Init();
// 备注信息     对电机信号引脚进行初始化,
// 备注信息     对电机编码器引脚进行初始化,
// 备注信息     在对应引脚启用PWM输出，默认频率17000Hz，占空比0%.
//-------------------------------------------------------------------------------------------------------------------
void Motor_Init(void)
{
    Sign_Init();
    MyEncoder_Init();
		FL_Motor.encoder=0,FL_Motor.k=0,FL_Motor.new_speed=0.0f,FL_Motor.old_speed=0.0f,FL_Motor.Speed[0]=0.0f,FL_Motor.distance=0;
		FR_Motor.encoder=0,FR_Motor.k=0,FR_Motor.new_speed=0.0f,FR_Motor.old_speed=0.0f,FR_Motor.Speed[0]=0.0f,FR_Motor.distance=0;
		B_Motor.encoder=0,B_Motor.k=0,B_Motor.new_speed=0.0f,B_Motor.old_speed=0.0f,B_Motor.Speed[0]=0.0f,B_Motor.distance=0;


	//    //直流电机初始化
    pwm_init(FLMotor, 17000, 0);                                                // PWM 通道 2 初始化频率 17KHz 占空比初始为 0
    pwm_init(FRMotor, 17000, 0);                                                // PWM 通道 2 初始化频率 17KHz 占空比初始为 0
    pwm_init(BMotor , 17000, 0);                                                // PWM 通道 2 初始化频率 17KHz 占空比初始为 0
         

}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     电机编码器初始化
// 参数说明     无
// 返回参数     void
// 使用示例     MyEncoder_Init();
// 备注信息     对编码器引脚初始化.
//-------------------------------------------------------------------------------------------------------------------
void MyEncoder_Init(void)
{
    encoder_quad_init(FLENCODER, FLENCODER_A, FLENCODER_B);                     // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_quad_init(FRENCODER, FRENCODER_A, FRENCODER_B);                     // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_quad_init(BENCODER , BENCODER_A , BENCODER_B );                     // 初始化编码器模块与引脚 正交解码编码器模式
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     电机速度、方向设置
// 参数说明     LDuty   左电机速度设置. 最大可设置占空比为 PWM_DUTY_MAX = 10000,相反则是-10000.
// 参数说明     RDuty   右电机速度设置. 最大可设置占空比为 PWM_DUTY_MAX = 10000,相反则是-10000.
// 返回参数     void
// 使用示例     Motor_Set(5000, 5000); 将左右电机速度都设置为50%占空比,方向向前.
// 备注信息     给入参数为负数时电机向前,正数时电机向后,速度取给入参数的绝对值.
//-------------------------------------------------------------------------------------------------------------------
void Motor_Set(int FLDuty, int FRDuty, int BDuty)
{

    if(FLDuty >= 0)										//逆
    {
        gpio_set_level(FL_DIR, 1);
        pwm_set_duty(FLMotor, FLDuty);
    }
    else															//顺
    { 
        gpio_set_level(FL_DIR, 0);
        pwm_set_duty(FLMotor, -FLDuty);
    }

    if(FRDuty >= 0)										//逆
    {
        gpio_set_level(FR_DIR, 1);
        pwm_set_duty(FRMotor, FRDuty);
    }
    else															//顺
    {
        gpio_set_level(FR_DIR, 0);
        pwm_set_duty(FRMotor, -FRDuty);
    }
		
		if(BDuty >= 0)										//逆
    {
        gpio_set_level(B_DIR, 0);
        pwm_set_duty(BMotor, BDuty);
    }
    else															//顺
    {
        gpio_set_level(B_DIR, 1);
        pwm_set_duty(BMotor, -BDuty);
    }

}

//-------------------------------------------------------------------------------------------------------------------
//* 函数说明: 一阶滤波 本次滤波值 = a*本次采样值 + （1-a)*上次的滤波输出值
//* 参数:    编码器测得值
//* 返回值:   滤波后的值
//-------------------------------------------------------------------------------------------------------------------
float First_Order_Filtering(int value, float A)
{
    float out_value;

    if(A == A_FL)
    {
        out_value = A * (float)value - (1-A) * FL_LastValue;
        FL_LastValue = out_value;
    }
		
    else if(A == A_FR)
    {
        out_value = A * (float)value - (1-A) * FR_LastValue;
        FR_LastValue = out_value;
    }

		 else if(A == A_B)
    {
        out_value = A * (float)value - (1-A) * B_LastValue;
        B_LastValue = out_value;
    }

    return out_value;
}


/**@brief   X轴速度获取
-- @param   无
-- @author  陈锦辉
-- @date    2023/12/7
**/
float Get_X_Speed()
{
    return (FL_Motor.new_speed + FR_Motor.new_speed + B_Motor.new_speed)/4.0f;
}

/**@brief   Y轴速度获取
-- @param   无
-- @author  陈锦辉
-- @date    2023/12/7
**/
float Get_Y_Speed()
{
    return (FL_Motor.new_speed + FR_Motor.new_speed)/2;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取编码器计数
// 参数说明     无
// 返回参数     voids
// 使用示例     Get_Speed();
// 备注信息     虽说是10次才计算一次，不过是求平均值，所以还是平均5ms的编码器值
//-------------------------------------------------------------------------------------------------------------------
void Get_Speed(MotorStruct * Motor, float A)
{
		
		static short i,j;
		float temp=0.0;	
		if(Motor==&FL_Motor)
		{
			Motor->encoder=encoder_get_count(FLENCODER);
			 if(Navigation.Start_Flag)//开启惯性导航，开始距离解算
				Motor->distance += Motor->encoder*Cm_Per;
			 else Motor->distance = 0;
			encoder_clear_count(FLENCODER);
		}
		
		else if(Motor==&FR_Motor)
		{
			Motor->encoder=encoder_get_count(FRENCODER);
				if(Navigation.Start_Flag)//开启惯性导航，开始距离解算
				Motor->distance += Motor->encoder*Cm_Per;
			 else Motor->distance = 0;
			
			encoder_clear_count(FRENCODER);
		}

		else if(Motor==&B_Motor)
		{
			Motor->encoder=encoder_get_count(BENCODER);		
			 if(Navigation.Start_Flag)//开启惯性导航，开始距离解算
				Motor->distance += Motor->encoder*Cm_Per;
			 else Motor->distance = 0;
			encoder_clear_count(BENCODER);
		}

		Motor->Speed[Motor->k++]=(Motor->encoder*200/512/REDUCTION_RATIO)*Car_R*PI*2;//1000/1024
	
		if (Motor->k==10)//冒泡法求平均速度
		{
			Motor->k=0;
			for (i=0;i<9;i++)
			{
			   for (j=0;j<9-i;j++)
			    {
						if (Motor->Speed[j]<Motor->Speed[j+1]) 			
						 {
							 temp=Motor->Speed[j];
							 Motor->Speed[j]=Motor->Speed[j+1];
							 Motor->Speed[j+1]=temp;
						 }
				 }
			 }
			temp=0.0;
			for (i=2;i<8;i++)Motor->new_speed =Motor->new_speed+Motor->Speed[i];
			
			Motor->new_speed=Motor->new_speed/6;	
						 
			Motor->old_speed=Motor->new_speed;
	    
	  }		
		
		    //编码器一阶滤波
    First_Order_Filtering(Motor->new_speed, A);



}

///**@brief   获取距离
//-- @param   无
//-- @author  陈锦辉
//-- @date    2023/12/6
//**/
//static void Get_Distance()
//{
//    if(1)//Navigation.Start_Flag)//开启惯性导航，开始距离解算
//    {
//        for(int i = 0;i<=3;i++)
//        {   
//            Distance_Buffer[i] = Encoder_Pules_Buffer[i];
//        }
//        for(int i = 0;i<=3;i++)
//        {   
//            Encoder_Distance[i] += Distance_Buffer[i]*Cm_Per;
//        }
//    }
//    else
//    {
//        for(int i = 0;i<=3;i++)//清空
//        {   
//            Distance_Buffer[i] = 0;
//            Encoder_Distance[i] = 0;
//        }
//    }

//}
