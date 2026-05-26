/**
  ******************************************************************************
  * @file    Menu.c
  * @author  陈锦辉
  * @brief   菜单
  * @date    11/13/2023
    @verbatim
    菜单显示
    @endverbatim
  * @{
**/



/* Includes ------------------------------------------------------------------*/
#include "Menus.h"
/* Define\Declare ------------------------------------------------------------*/

const char*    		  PLACE_TABLE_STR[16] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O"};

bool Start = 0;

uint8 Menu_Mode = Image_page;//Box_Page;//Image_page;
Menu_ Menu = 
{
    .Set_Line = 0,
    .Image_Show = true,
		.IPS_Show = true,
};
#define Exit_Dis ips200_show_string(Col_1,Row_0,"Exit")
#define FLASH_SECTION_INDEX (127)// 存储数据用的扇区 倒数第一个扇区
#define FLASH_PAGE_INDEX (FLASH_PAGE_3)// 存储数据用的页码 倒数第一个页码

/**
 ******************************************************************************
 *  @defgroup 内部调用
 *  @brief
 *
**/

/**@brief   显示箭头
-- @param   int Row 需要显示的行号
-- @author  陈锦辉
-- @date    2023/11/13
**/
static void Arrow_Display(int Row)
{
    for(int Row_Num = 0;Row_Num <= 14;Row_Num++)
    {
        if(Row == Row_Num)
        {
            ips200_show_string(Col_0,Row_Num*16,">");
        }
        else
        {
					  ips200_show_string(Col_0,Row_Num*16," ");

//            if(!Menu.Image_Show)
//            {
//                ips200_show_string(Row_0,Col_Num*16," ");
//            }
//            else if(Col_Num >=6 && Menu.Image_Show)
//            {
//                ips200_show_string(Row_0,Col_Num*16," ");
//            }
        }

    }
}

/**@brief   行切换
-- @param   无
-- @author  陈锦辉
-- @date    2024/2/16
**/
static void Row_Change()
{
    if((Button_Value[0] == 2) && (Menu.Set_Line > 0))//向上
    {
        Button_Value[0] = 0;
        Menu.Set_Line--;
    }
    else if((Button_Value[1] == 2) && (Menu.Set_Line < 14))//向下
    {
        Button_Value[1] = 0;
        Menu.Set_Line++;
    }
		else if((Button_Value[1] == 2) && (Menu.Set_Line == 14))
		{
				Button_Value[1] = 0;
        Menu.Set_Line=0;
		}
		else if((Button_Value[0] == 2) && (Menu.Set_Line == 0))
		{
				Button_Value[0] = 0;
        Menu.Set_Line=14;
		}
}

/**@brief   IPS屏幕显示开关
-- @param   无
-- @author  陈锦辉
-- @date    2024/2/16
**/
static void IPS_ON_OFF()
{
    if((Button_Value[3] == 2))
    {
        Button_Value[3] = 0;
        Menu.IPS_Show = !Menu.IPS_Show;
				ips200_clear();
    }
}

/**@brief   页面选择模式
-- @param   无
-- @author  陈锦辉
-- @date    2024/2/16
**/
static void Page_Select_Mode()
{
    ips200_show_string(Col_1,Row_0,"Car_Go");
    ips200_show_uint	(Col_8,Row_0, Start,1);
    ips200_show_string(Col_1,Row_1, "Box"); 
    ips200_show_string(Col_1,Row_2, "Motor"); 
    ips200_show_string(Col_1,Row_3, "Image"); 
    ips200_show_string(Col_1,Row_4, "Page4"); 
    if(Button_Value[2] == 2)//按键按下
    {
        Button_Value[2] = 0;
        Menu_Mode = Menu.Set_Line;
        Menu.Set_Line = 0;
        if(Menu_Mode==0)
        {
            Start = !Start;
						Menu.IPS_Show = 0;//0;
            ips200_clear();
						Menu_Mode=Box_Page;
						Menu.Image_Show = 0;
//            Set_Beepfreq(1);
            flash_buffer_clear();
        }
        ips200_clear();
    }   
	  Row_Change();//行切换
    Arrow_Display(Menu.Set_Line);//箭头显示

}

/**@brief    第零页显示
-- @param    无
-- @verbatim 用于查看箱子信息
-- @author   陈锦辉
-- @date     2024/2/16
**/
static void Box_Mode()
{
	  Exit_Dis;
	
		ips200_show_string(Col_1,  Row_1 , "Box 1:");																  
    ips200_show_string(Col_1,  Row_2 , "Box 2:");																	  
    ips200_show_string(Col_1,  Row_3 , "Box 3:");																	  
    ips200_show_string(Col_1,  Row_4 , "Box 4:");																		
    ips200_show_string(Col_1,  Row_5 , "Box 5:");																	  
    ips200_show_string(Col_1,  Row_6 , "Box 6:");																	  
    ips200_show_string(Col_1,  Row_7 , "Box 7:");																	  
	  ips200_show_string(Col_1,  Row_8 , "Box 8:");																	  
    ips200_show_string(Col_1,  Row_9 , "Box 9:");																	  
    ips200_show_string(Col_1,  Row_10, "Box10:");
    ips200_show_string(Col_1,  Row_11, "Box11:");
    ips200_show_string(Col_1,  Row_12, "Box12:");
		ips200_show_string(Col_14, Row_1 , "Box13:");																  
    ips200_show_string(Col_14, Row_2 , "Box14:");																		
    ips200_show_string(Col_14, Row_3 , "Box15:");																	  
    ips200_show_string(Col_14, Row_4 , "Box16:");																	  
    ips200_show_string(Col_14, Row_5 , "Box17:");																	  
	  ips200_show_string(Col_14, Row_6 , "Box18:");																	  
    ips200_show_string(Col_14, Row_7 , "Box19:");																	  
    ips200_show_string(Col_14, Row_8 , "Box20:");
    ips200_show_string(Col_14, Row_9 , "Box21:");
    ips200_show_string(Col_14, Row_10, "Box22:");
    ips200_show_string(Col_14, Row_11, "Box23:");
    ips200_show_string(Col_14, Row_12, "Box24:");
                                   
	
	
	for(uint8 i=0;i<=23;i++)
	{
		if(Box_DATA[i].small-101>=0)
		{
			ips200_show_chinese(Col_8+Col_13*(i/12), Row_1*(i%12 + 1),16,(const uint8 *)hanzi[Box_DATA[i].small-101], 3, black_point); // 显示汉字
		}
		else if(Box_DATA[i].small>=1 && Box_DATA[i].small<=100)
		{
			ips200_show_uint(Col_8+Col_13*(i/12), Row_1*(i%12 + 1),Box_DATA[i].small,3);
		}
	}

    if(Menu.Set_Mode == Normal_Mode)
    {
        if((Menu.Set_Line == 0) && (Button_Value[2] == 2))//退出
        {
            Button_Value[2] = 0;
            Menu_Mode = Page_Select;//退出到第一页
            Menu.Set_Line = 0;
            ips200_clear();
        }
				else if((Menu.Set_Line) && (Button_Value[2] == 2))
        {
            Button_Value[2] = 0;
        } 

        Row_Change();//行切换
    }
    Arrow_Display(Menu.Set_Line);//箭头显示

}


/**@brief   第一页显示
-- @param   无
-- @author  陈锦辉
-- @date    2024/2/18
**/
static void Page2_Mode()
{
	 Exit_Dis;
	
	  ips200_show_string(Col_1, Row_1,"Image_Show:");
	  ips200_show_uint	(Col_12,Row_1,Menu.Image_Show,5);//图像调试用
	  ips200_show_string(Col_1, Row_17,"Menu_Mode:");
		ips200_show_uint	(Col_11,Row_17,Menu_Mode,1);	 //图像调试用

	
	// 在OLED显示屏上显示图像，参数为图像数据的指针以及显示相关的参数
	if(Menu.Image_Show)IPS200_Show_MyImage(Col_1, Row_2, (uint8 *)MyImage, MyImage_W, MyImage_H);

	
	 ips200_show_string(Col_1,Row_8 ,"FLMotor");
	 ips200_show_int	 (Col_1,Row_9 ,FLSetSpeed,4);
	 ips200_show_int	 (Col_1,Row_10 ,FL_Motor.new_speed,4);
	 ips200_show_int	 (Col_1,Row_11,FLMotorPID.Voltage,4);
	 ips200_show_int	 (Col_1,Row_12,FLMotorPID.err_last,4);		
	 ips200_show_int	 (Col_1,Row_13,FLMotorPID.pwm_out,4);
					
	 ips200_show_string(Col_10,Row_8 ,"FRMotor");
	 ips200_show_int	 (Col_10,Row_9 ,FLSetSpeed,4);
	 ips200_show_int	 (Col_10,Row_10,FR_Motor.new_speed,4);
	 ips200_show_int	 (Col_10,Row_11,FRMotorPID.err_last,4);		
	 ips200_show_int	 (Col_10,Row_12,FRMotorPID.pwm_out,4);
	 ips200_show_int	 (Col_10,Row_13,FRMotorPID.Voltage,4);

	 ips200_show_string(Col_20,Row_8 ,"BMotor");
	 ips200_show_int 	 (Col_20,Row_9 ,FLSetSpeed,4);
	 ips200_show_int 	 (Col_20,Row_10,B_Motor.new_speed,4);
	 ips200_show_int 	 (Col_20,Row_11,BMotorPID.err_last,4);		
	 ips200_show_int 	 (Col_20,Row_12,BMotorPID.pwm_out,4);
	 ips200_show_int 	 (Col_20,Row_13,BMotorPID.Voltage,4);
	 
    
    if(Menu.Set_Mode == Normal_Mode)
    {
        if((Menu.Set_Line == 0) && (Button_Value[2] == 2))//退出
        {
            Button_Value[2] = 0;
            Menu_Mode = Page_Select;//退出到第一页
            Menu.Set_Line = 0;
            ips200_clear();
        }
				 else if((Menu.Set_Line == 1) && (Button_Value[2] == 2))
        {
            Button_Value[2] = 0;
            Menu.Image_Show = !Menu.Image_Show;
            ips200_clear();
        } 
				else if((Menu.Set_Line) && (Button_Value[2] == 2))
        {
            Button_Value[2] = 0;
        } 
        Row_Change();//行切换

    }
    Arrow_Display(Menu.Set_Line);//箭头显示
}


/**@brief   图像显示以及相关参数
-- @param   无
-- @author  陈锦辉
-- @date    2024/7/1
**/
static void Image_Page()
{	
    static uint8 Image_Flag_Show = 0;

		ips200_show_string(Col_1, Row_1,"Image_Flag:");
	  ips200_show_uint	(Col_12,Row_1,Image_Flag_Show,5);//图像调试用
	  ips200_show_string(Col_1, Row_2,"Image_Show:");
		ips200_show_uint	(Col_12,Row_2,Menu.Image_Show,5);//图像调试用
	  ips200_show_string(Col_1, Row_17,"Menu_Mode:");
		ips200_show_uint	(Col_11,Row_17,Menu_Mode,1);		 //图像调试用
	
		if(Image_Flag_Show)
		{
//			 ips200_show_string(Col_1, Row_10 ,"L_Srt:");		    ips200_show_int(Col_6, Row_10,Left_line.Col[0],4); //边线初始点
// 		   ips200_show_string(Col_1, Row_11 ,"R_Srt:");		    ips200_show_int(Col_6, Row_11,Right_line.Col[0],4);//边线初始点
//																																								 
//			 ips200_show_string(Col_17, Row_10 ,"L_pe");			    ips200_show_int(Col_23, Row_10,Left_peak.Col,3);	//顶点
//			 ips200_show_string(Col_17, Row_11 ,"R_pe");			    ips200_show_int(Col_23, Row_11,Right_peak.Col,3); //
//																																								 
//			 ips200_show_string(Col_17, Row_12 ,"L_lo");			  ips200_show_int(Col_23,Row_12,Left_loss.Len,3);//丢线
//		   ips200_show_string(Col_17, Row_13 ,"R_lo");			  ips200_show_int(Col_23,Row_13,Right_loss.Len,3); //
//																																								 
//       ips200_show_string(Col_17,Row_14 ,"Cr_Lx:");				ips200_show_int(Col_23,Row_14,LCrossPeak.Col,4);//十字顶点
//       ips200_show_string(Col_17,Row_15 ,"Cr_Rx:");				ips200_show_int(Col_23,Row_15,RCrossPeak.Col,4);//十字顶点	
//																																								 
//       ips200_show_string(Col_17,Row_16 ,"Ro_2x:");				ips200_show_int(Col_23,Row_16,Round_peak.Col,4);//圆环顶点
//       ips200_show_string(Col_17,Row_17 ,"Ro_2y:");				ips200_show_int(Col_23,Row_17,Round_peak.Row,4);//圆环顶点

			 ips200_show_string(Col_17,Row_13 ,"CAR_X");				ips200_show_int(Col_23,Row_13,Car.Speed_X,4);		//X速度
			 ips200_show_string(Col_17,Row_14 ,"CAR_Y");				ips200_show_int(Col_23,Row_14,Car.Speed_Y,4);		//Y速度
			 ips200_show_string(Col_17,Row_15 ,"CAR_Z");				ips200_show_int(Col_23,Row_15,Car.Speed_Z,4);		//Z速度
			 ips200_show_string(Col_17,Row_16  ,"Curve");				ips200_show_int(Col_23,Row_16,	Box_Curve_State,1);//十字顶点	
			 ips200_show_string(Col_17,Row_17  ,"Start");				ips200_show_int(Col_23,Row_17,	Start,1);//十字顶点	
			 ips200_show_string(Col_17,Row_18  ,"St__F");				ips200_show_int(Col_23,Row_18,	Start_Flag,1);//十字顶点	

//				ips200_show_int(Col_23,Row_16,	Box_Road_Err,1);//十字顶点Box_Road_Err
//				ips200_show_int(Col_7,Row_11,Start_Angle,3); //角度
//				ips200_show_int(Col_7,Row_12,Rec_Now_Angle,3); //角度

//				ips200_show_int(150,16*4,X_err,4);          //箱子信息
//				ips200_show_int(150,16*5,Y_err,4); 
					ips200_show_int(150,16*4,Box.X,4);          //箱子信息
					ips200_show_int(150,16*5,Box.Y,4); 
		
			 ips200_show_string(Col_1,Row_12 ,"L_ang");					ips200_show_float(Col_7,Row_12,Left_Side_Angle,3,2); //角度
			 ips200_show_string(Col_1,Row_13 ,"LM_ang");				ips200_show_float(Col_7,Row_13,Left_Road_Angle,3,2); 			
			 ips200_show_string(Col_1,Row_14 ,"R_ang");					ips200_show_float(Col_7,Row_14,Right_Side_Angle,3,2); 
			 ips200_show_string(Col_1,Row_15 ,"RM_ang");				ips200_show_float(Col_7,Row_15,Right_Road_Angle,3,2);
			 ips200_show_string(Col_1,Row_16 ,"S_Cur:");					ips200_show_int(Col_7,Row_16,Start_Curve,1);//十字顶点	
			 
			 
			 

		   ips200_show_float(Col_1,Row_9   ,Angle_Erro,3,2);

//			ips200_show_uint(Col_23,Row_17,Black_Pnum,5);
//			ips200_show_uint(Col_23,Row_18,White_Pnum,5);

		   ips200_show_uint(Col_23,Row_17   ,Black_Pnum,3);
		   ips200_show_uint(Col_23,Row_18   ,White_Pnum,3);

		}
		// 在OLED显示屏上显示图像，参数为图像数据的指针以及显示相关的参数
		if(Menu.Image_Show)IPS200_Show_MyImage(Col_1, Row_3, (uint8 *)MyImage, MyImage_W, MyImage_H);
		ips200_show_string(Col_0,Row_19,"ANG");					ips200_show_int(Col_4,Row_19,Gyro_Angle.YawAngle,4);//边线初始点	
		ips200_show_string(Col_17,Row_19 ,"box");	
		if(CLASSIFY_DATA.small-101>=0)ips200_show_chinese(Col_23, Row_19,16,(const uint8 *)hanzi[CLASSIFY_DATA.small-101], 3, black_point); // 显示汉字
		else if(CLASSIFY_DATA.small<=100 && CLASSIFY_DATA.small>=0) ips200_show_uint(Col_23,Row_19,CLASSIFY_DATA.small,6);//边线初始点	
		ips200_show_uint(Col_23,Row_18,CLASSIFY_DATA.IS_CLASSIFY,3);//边线初始点	

		
    Exit_Dis;


    if(Menu.Set_Mode == Normal_Mode)
    {
        if((Menu.Set_Line == 0) && (Button_Value[2] == 2))//退出
        {
            Button_Value[2] = 0;
            Menu_Mode = Page_Select;//退出到第一页
            Menu.Set_Line = 0;
            Menu.Image_Show = 1;//false;
            ips200_clear();
        }
        else if((Menu.Set_Line == 1) && (Button_Value[2] == 2))
        {
            Button_Value[2] = 0;
            Image_Flag_Show = !Image_Flag_Show;
            ips200_clear();
        } 
        else if((Menu.Set_Line == 2) && (Button_Value[2] == 2))
        {
            Button_Value[2] = 0;
            Menu.Image_Show = !Menu.Image_Show;
            ips200_clear();
        } 
				else if((Menu.Set_Line) && (Button_Value[2] == 2))
        {
            Button_Value[2] = 0;
        }
        Row_Change();//行切换
    }

    Arrow_Display(Menu.Set_Line);//箭头显示
}

static void Page4_Mode()
{
	IPSshow();
	Exit_Dis;
	if(Menu.Set_Mode == Normal_Mode)
    {
        if((Menu.Set_Line == 0) && (Button_Value[2] == 2))//退出
        {
            Button_Value[2] = 0;
            Menu_Mode = Page_Select;//退出到第一页
            Menu.Set_Line = 0;
            ips200_clear();
        }
				else if((Menu.Set_Line) && (Button_Value[2] == 2))
        {
            Button_Value[2] = 0;
        } 
        Row_Change();//行切换
    }
	
	Arrow_Display(Menu.Set_Line);//箭头显示
}


//}
/**
 ******************************************************************************
 *  @defgroup 外部调用
 *  @brief
 *
**/


/**@brief   每页显示的内容
-- @param   无
-- @author  陈锦辉
-- @date    2023/11/13
**/
void Menu_Display()
{
	IPS_ON_OFF();
	if(Menu.IPS_Show)
	{
		 switch(Menu_Mode)
    {
        case Page_Select:
            Page_Select_Mode();
        break;
        case Box_Page:
            Box_Mode();
        break;
        case Motor_Page:
            Page2_Mode();
        break;
        case Image_page:
            Image_Page();
        break;
				case Page4:
					Page4_Mode();							
				break;
    }
	}
	if(Zebra_flag == 1)
	{	
		X_Y_Z_Speed_Set(400,0,0);
		if(Bufcnt1(Zebra_flag,1000))
		{
				Zebra_flag = 0;
				Start = 0;
        Stop_flag = 1;
				X_Y_Z_Speed_Set(0,0,0);
				Menu_Mode = Box_Page;
				Menu.IPS_Show = 1;
				ips200_clear();
		}		
	}
//	 ips200_show_string(Col_1,Row_16 ,"Curve");					ips200_show_uint(Col_7,Row_16,	Start_Curve,1);//十字顶点
//	 ips200_show_string(Col_1,Row_16 ,"Curve");					ips200_show_uint(Col_23,Row_17,	Box_Curve_State,1);//十字顶点
//		ips200_show_float(Col_7,Row_16,X_Speed,3,2);//十字顶点
//		ips200_show_string(150,16*2 ,"Err:");					ips200_show_int(200,16*2,Image_Erro,4); //误差值 

	
}

