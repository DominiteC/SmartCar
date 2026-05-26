#ifndef _CONTROL_H_
#define _CONTROL_H_
#include "zf_common_headfile.h"
#include "math.h"

#define R_Car   13.279		 //车的圆周半径
#define PAI     3.1415926	 //pi值
#define GEN32    0.866025  //2分之根号3


//小车工作状态枚举
typedef enum
{
		Stop=0,
		Back=1,					//后退
		Ahead=2,				//前进
    Left_RUN=3,			//左平移
		Right_RUN=4,		//右平移			
		Leftaround=5,    //左旋
	Rightaround=6   //右旋
	
}work_state1;

typedef struct 
{
    int16 Speed_X;//小车X轴速度
    int16 Speed_Y;//小车Y轴速度
    int16 Speed_Z;//小车Z轴速度

}Car_Handle;

typedef struct
{
    float Start_Position_X;//初始X坐标
    float Start_Position_Y;//初始Y坐标
    float Cur_Position_X;//当前X坐标
    float Cur_Position_Y;//当前Y坐标
    float Start_Angle;//初始角度
    float Cur_Angle;//当前角度
    float End_Angle;//结束角度
    float Target_Position_X;//目标X坐标
    float Target_Position_Y;//目标Y坐标
    float X_Speed;//X轴速度
    float Y_Speed;//Y轴速度
    bool Start_Flag;//启动惯性导航
    bool Finish_Flag;//惯性导航结束
    bool X_Finish;
    bool Y_Finish;
	
}Navigation_Handle;

typedef enum
{
    Start_State = 0,//开始状态，记录初始值
    Move_State,//移动状态
    Move_Finish,//移动结束
}Navigation_State;

typedef enum  
{
		no = -1,
		No_Curve=0,//直道
    Left_Curve=1,//左弯
		Right_Curve=2,//右弯	
	
}Box_Curve_Flag;


typedef struct 
{
    float Offset;//累计转动的角度
    float Angle_Last;//上一次的角度
    float Current_Angle; //当前角度
    volatile bool Finish;//转动是否完成
}Turn_Handle;


//------------------------车体运动函数--------------------------------
void Run_State(work_state1 a);
void Box_Run(void);
void Car_run(float Speed);
bool Turn_Angle(float Target_Angle);
//------------------------车体运动函数--------------------------------

//---------------------------箱子--------------------------------------
void Box_Curve_Check(void);
bool Box_Curve_aim(void);
//---------------------------箱子--------------------------------------


//------------------------设速函数--------------------------------
void Box_Car_Speed(int16 Speed_X,int16 Speed_Y,int16 Speed_Z);
void X_Y_Z_Speed_Set(int16 Speed,int16 Angle,int16 Yaw);
//------------------------设速函数--------------------------------


//------------------------计时函数--------------------------------
bool Bufcnt1(bool Cond,uint16 Cnt);
bool Bufcnt2(bool Cond,uint16 Cnt);
//------------------------计时函数--------------------------------

//------------------------惯导函数--------------------------------
static void Enable_Navigation();
void 	Reset_Navigation();
//------------------------惯导函数--------------------------------


float Angle_Control(float Start_Angle);

void Get_LeftRight();
void Box_Line_Peak(void);

float tan_to_Angle(float tan);


//------------------------计时变量声明--------------------------------
extern uint16 Time_Cnt1;
extern uint16 Time_Cnt2;
//------------------------计时变量声明--------------------------------

//------------------------箱子道路矫正变量声明--------------------------------
extern float 	Angle_Erro;
extern   			Box_Curve_Flag   Box_Curve_State;
extern int 		Start_Angle;
extern int 		Rec_Now_Angle;
extern int8 	Start_Curve;
extern uint8 	Start_Curve_Flag;
//------------------------箱子道路矫正变量声明--------------------------------

//------------------------箱子运动变量声明--------------------------------
extern uint8 					X_flag			;
extern uint8 					Y_flag			;
extern int8 					X_err				;
extern int8 					Y_err				;
extern Turn_Handle 		Turn				;
extern Turn_Handle 		Move				;
extern Turn_Handle 		Aim					;

extern int8 					Box_Road_Err;
//------------------------箱子运动变量声明--------------------------------

//------------------------设速变量声明--------------------------------
extern Car_Handle Car;
//------------------------设速变量声明--------------------------------

//------------------------惯导变量声明--------------------------------
extern Navigation_Handle Navigation;
extern Navigation_State Navigation_state;
//------------------------惯导变量声明--------------------------------

// 结构体存储拟合结果
typedef struct {
    double a;  // x2 的系数
    double b;  // x 的系数
    double c;  // 常数项
} PolyFitResult;

PolyFitResult polyfit2(int *x, int *y, int n);
void polyval2(PolyFitResult *coefficients, int* x);

#endif
