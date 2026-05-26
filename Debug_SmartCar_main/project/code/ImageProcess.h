#ifndef _IMAGEPROCESS_H_
#define _IMAGEPROCESS_H_
#include "zf_common_headfile.h"


#define  Image_color  1  //边线上色，调试用


////裁剪范围由zf_device_mt9v03x.h中的MT9V03X_W和MT9V03X_H宏定义决定.
//#define MyImage_W       (188) //- 94)          //裁剪之后图像的宽度
//#define MyImage_H       (120)// - 60)          //裁剪之后图像的高度
//#define Race_Center     (MyImage_W/2)

#define MyImage_W       (141)          //裁剪之后图像的宽度
#define MyImage_H       (90)          //裁剪之后图像的高度
#define Race_Center     (MyImage_W/2)

#define Bin_Threshold   150               //二值化固定阈值

#define Box_W   30               //箱子大概边长


#define USE_num Image_H*3   //定义找点的数组成员个数按理说300个点能放下，但是有些特殊情况确实难顶，多定义了一点

extern uint16 Len_Left;//统计左边找到点的个数
extern uint16 Len_Right;//统计右边找到点的个数

extern uint16 Black_Pnum;//统计黑点的个数
extern uint16 White_Pnum;//统计白点的个数

extern uint8_t last_White_num;
extern uint8_t last_Black_num;

extern uint8 MyImage[MyImage_H][MyImage_W];                                  //裁剪得到的图像数据(仍然是灰度)

extern int Threshold;

extern uint8_t Left_Start_flag;                             								//左线起始点标志位
extern uint8_t Right_Start_flag;                             								//右线起始点标志位
extern uint8_t Stop_flag;                             											//停止标志位
extern uint8_t Leftline_Jump_flag;																					//左线跳变标志位
extern uint8_t Rightline_Jump_flag;																					//右线跳变标志位

extern uint8_t SpecialSpeed_flag;

extern float white_Per;//列
extern float black_Per;//列

extern int SpecialSpeed;
extern float Cross_Col_Per;//列
extern float Cross_Row_Per;//行
extern float Stop_Row_Per;
extern float Left_Row_Per;
extern float Right_Row_Per;
extern float Right_Round_Per;
extern float L_S;
extern float R_S;

extern int Round_peak_Row;                                                     //环岛顶点数据
extern int Round_peak_Col;                                                     //环岛顶点数据


extern float ref;

//十字路口左上顶点和十字路口右上顶点,记录的数据是该点的列


extern int Image_Erro;




//边线结构体,负责保存边线上的点的位置
struct SIDELINE
{
	int Row[5*MyImage_H];
	int Col[5*MyImage_H];
};


//边线顶点结构体,负责保存边线上的点的位置
struct PEAK
{
	int Row;
	int Col;
	int pin;
};


//十字路口左上顶点和十字路口右上顶点,记录的数据是该点的列
struct CROSSPEAK
{
    int Row;
    int Col;
};


//丢线点结构体,负责保存边线上的点的位置
struct LOSS
{
    int Row;
    int Col;
    int Len;
};


//跳变点结构体,负责保存边线上的点的位置
struct Jump
{
    int Row;
    int Col;
};


//箱子位置结构体
struct BOX_PLACE
{
    uint8_t X;
    uint8_t Y;
    uint8_t W;
		uint8_t H;
		uint16 S;
};


typedef enum
{
    Gray_mode,                          //灰度图像模式
    Bin_Threshold_mode,                 //固定阈值法二值化图像处理
    Bin_OTSU_mode,                      //最大类间方差算法(大津法)二值化图像处理
		Bin_adaptive_mode,									//自适应阈值法
		sobel_mode													//索贝尔边界
}transfer_mode_enum;


typedef enum
{
    Fix_Left,                           //左边线拟合
    Fix_Right,                          //右边线拟合
}fix_line;


//小车工作状态枚举
typedef enum
{
    No_Duel,
		Dur_Cross,
		Dur_Round,
		Dur_Box,
}work_state;


//十字路口状态枚举
typedef enum
{
    No_Cross,//0
    Before_Turn,//1
		Stop_State,
    In_Cross,//2
    Cross_Turn,//3
    Out_Cross,//4
}cross_flag;

typedef enum
{
    No_Into,                //未进入十字
    Stright_Into,           //正入十字
    LeftSide_Into,          //右转弯斜入十字,拟合左边线
    RightSide_Into,         //左转弯斜入十字,拟合右边线
}cross_dirflag;

//环岛状态枚举
typedef enum
{
    No_Round,								//未入圆环
    Find_Round,							//找到圆环
    Check_Round,						//环岛检查
    Into_Round,							//入环3
    Round_Turn,							//转弯段
    Out_Round,							//出环岛
}roundabout_flag;

typedef enum
{
    NonDir,
    LeftRound,
    RightRound,
}roundabout_dirflag;

typedef enum
{
    No_box,
		Rec_Angle,
    Find_box,
		Turn_box,
		Box_scan,
		Back_Turn,
		Turn_aim,
		Pull_box,		
		Back_road,
		Back_aim,
    Finish_box,
}box_flag;




//图像画点颜色
typedef enum
{
    black_point = 0,
    red_point,
    blue_point,
    purple_point,
    none = 254,         //none表示无数据
    white_point = 255,  //逐飞 摄像头-ips200 显示函数导致的奇怪枚举值,请自行翻阅逐飞库函数学习
}point_color;

extern uint8_t Start_Flag;


//-----------------------------------------------------
extern   uint8_t             	 Zebra_flag;
extern   work_state       		 Work_State;
extern   cross_flag				     Cross_State;
extern   cross_dirflag    		 Cross_DirFlag;
extern   roundabout_flag  		 Roundabout_State;
extern   roundabout_dirflag    Roundabout_DirFlag;
extern   box_flag 						 Box_State;
extern   uint8_t             	 Origin_State;


extern int Center_Line_Col[5*MyImage_H];
extern int Center_Line_Row[5*MyImage_H];


extern float Left_Side_Tan;		//左边界tan值
extern float Left_Side_Angle;	//左边界角度
extern float Right_Side_Tan;	//右边界tan值
extern float Right_Side_Angle;//右边界角度
extern float Left_Road_Tan;		//左边界tan值
extern float Left_Road_Angle;	//左边界角度
extern float Right_Road_Tan;	//右边界tan值
extern float Right_Road_Angle;//右边界角度



extern struct SIDELINE Left_line;
extern struct SIDELINE Right_line;
extern struct PEAK Left_peak;
extern struct PEAK Right_peak;
extern struct PEAK Round_peak;
extern struct LOSS Left_loss;
extern struct LOSS Right_loss;
extern struct Jump Left_jump;
extern struct Jump Right_jump;
extern struct CROSSPEAK LCrossPeak;
extern struct CROSSPEAK RCrossPeak;
extern struct BOX_PLACE Box;



//---------------------------外部调用函数--------------------------------------
void MyImage_Init(void);
void Data_Init(void);
float Cross_Col(void);
float Cross_Row(uint8_t RowCenter);
float LCross_Row(void);
float RCross_Row(void);
void 	Line_Angle();
//void Image_SuitCut(void);
void Image_Compress(void);
void Image_Transfer(transfer_mode_enum transfer_mode);
void GetLRSide(void);
void SideLine_Peak(void);
void LossLine_Check(void);
void JumpPoint_Check(void);
void Image_BinFilter(void);
void adaptiveThreshold(uint8_t clip_value);
void image_CBH(int16 Threshold_cbh);
void Image_Filter();
void Middle_Line(void);
void Loss_line_Protect(void);
void All_Check(void);
void All_handler(void);


//---------------------------Sobel--------------------------------------

void GetLR(void);
void Side_Peak(void);
void Loss_Protect(void);

//---------------------------Sobel--------------------------------------

//---------------------------外部调用函数--------------------------------------


//以下函数仅供内部调用
//====================================================图像处理方法====================================================
void Bin_Threshold_Transfer(void);
void Bin_OTSU_mode_Transfer(void);
uint8 OTSU();

//====================================================图像处理方法====================================================


#endif
