#ifndef MENUS_H_
#define MENUS_H_

#include "zf_common_headfile.h"

void Menu_Display();
void Flash_Init();


//行的宏定义
#define Row_0  (16* 0)//第零行
#define Row_1  (16* 1)//第一行
#define Row_2  (16* 2)//第二行
#define Row_3  (16* 3)//第三行
#define Row_4  (16* 4)//第四行
#define Row_5  (16* 5)//第五行
#define Row_6  (16* 6)//第六行
#define Row_7  (16* 7)//第七行
#define Row_8  (16* 8)//第八行
#define Row_9  (16* 9)//第九行
#define Row_10 (16*10)//第十行
#define Row_11 (16*11)//第十一行
#define Row_12 (16*12)//第十二行
#define Row_13 (16*13)//第十三行
#define Row_14 (16*14)//第十四行
#define Row_15 (16*15)//第十五行
#define Row_16 (16*16)//第十六行
#define Row_17 (16*17)//第十六行
#define Row_18 (16*18)//第十六行
#define Row_19 (16*19)//第十六行
#define Row_20 (16*20)//第十六行


//列的宏定义
#define Col_0  (8* 0)//第零列
#define Col_1  (8* 1)//第一列
#define Col_2  (8* 2)//第二列
#define Col_3  (8* 3)//第三列
#define Col_4  (8* 4)//第四列
#define Col_5  (8* 5)//第五列
#define Col_6  (8* 6)//第六列
#define Col_7  (8* 7)//第七列
#define Col_8  (8* 8)//第八列
#define Col_9  (8* 9)//第九列
#define Col_10 (8*10)//第十列
#define Col_11 (8*11)//第十一列
#define Col_12 (8*12)//第十二列
#define Col_13 (8*13)//第十三列
#define Col_14 (8*14)//第十四列
#define Col_15 (8*15)//第十五列
#define Col_16 (8*16)//第十六列
#define Col_17 (8*17)//第十五列
#define Col_18 (8*18)//第十六列
#define Col_19 (8*19)//第十六列
#define Col_20 (8*20)//第十六列
#define Col_21 (8*21)//第十六列
#define Col_22 (8*22)//第十六列
#define Col_23 (8*23)//第十六列
#define Col_24 (8*24)//第十六列
#define Col_25 (8*25)//第十六列
#define Col_26 (8*26)//第十六列
#define Col_27 (8*27)//第十六列
#define Col_28 (8*28)//第十六列
#define Col_29 (8*29)//第十六列
#define Col_30 (8*30)//第十六列

typedef enum
{
	Page0 		 = 0,
	Box_Page   = 1,
	Motor_Page = 2,
	Image_page = 3,
	Page4 		 = 4,
	Page5 		 = 5,
	Page6 		 = 6,
	Page7 		 = 7,
	Page8 		 = 8,
	Page9 		 = 9,
	Page_Select = 10,//页面选择
}Menu_Handle;

typedef enum
{
	Normal_Mode = 0,
	Flash_Mode,
}Mode_Handle;

typedef struct
{
	uint8 Set_Line;//设置的行
	uint8 Flash_Set;
	Mode_Handle Set_Mode;
	bool  Image_Show;//是否为图像显示
	bool  IPS_Show;//是否为图像显示
	uint16 Ex_Time;//曝光时间
	uint8 Turn_Point;
	uint16 Zebra_First_Dis;//斑马前距离
}Menu_;

extern Menu_ Menu;

extern bool Start;//发车标志
#endif
