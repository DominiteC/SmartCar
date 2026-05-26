#include"IPSshow.h"

int Image_Erro;
int Image_Erro_;


void IPSshow(void)
{
////y: 0 ~ 120------------------------------------------------------------------------------------------------
//    //本区域内一般作为图像显示使用
//	
				// 在OLED显示屏上显示图像，参数为图像数据的指针以及显示相关的参数
		if(Menu.Image_Show)IPS200_Show_MyImage(Col_1, Row_1, (uint8 *)MyImage, MyImage_W, MyImage_H);

//			ips200_show_string(150,16*2 ,"Err:");					ips200_show_int(200,16*2,Image_Erro,4); //误差值 
			ips200_show_string(150,16*3 ,"tim1:");					ips200_show_int(200,16*3,Time_Cnt1,4); //误差值 
//			ips200_show_string(150,16*4 ,"tim2:");					ips200_show_int(200,16*4,Time_Cnt2,4); //误差值
			ips200_show_int(200,16*4,Box.S,4); //误差值 	
//			ips200_show_int(150,16*4,X_err,4);          //箱子信息
//			ips200_show_int(150,16*5,Y_err,4);      

//显示完整灰度图像
//ips200_displayimage03x(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);

//    //画中心十字

//    ips200_draw_line(MyImage_W/2, 0, MyImage_W/2, MyImage_H, RGB565_RED);
//    ips200_draw_line(0, MyImage_H / 2, MyImage_W, MyImage_H / 2, RGB565_RED);

	
//y: 0 ~ 120------------------------------------------------------------------------------------------------
	
//y: 192 ~ 320------------------------------------------------------------------------------------------------
//----------------------------电机转动数据-------------------------------------

//			 ips200_show_string(Row_1,Col_7 ,"FLMotor");
//			 ips200_show_int(Row_1,Col_8 ,FLMotorPID.Voltage,4);
//			 ips200_show_int(Row_1,Col_9 ,FLMotorPID.err_last,4);		
//			 ips200_show_int(Row_1,Col_10,FLMotorPID.pwm_out,4);

//							
//			 ips200_show_string(Row_10,Col_7,"FRMotor");
//			 ips200_show_int(Row_10,Col_8 ,FRMotorPID.Voltage,4);
//			 ips200_show_int(Row_10,Col_9 ,FRMotorPID.err_last,4);
//			 ips200_show_int(Row_10,Col_10,FRMotorPID.pwm_out,4);

//			 ips200_show_string(Row_20,Col_7 ,"BMotor");
//			 ips200_show_int(Row_20,Col_8 ,BMotorPID.Voltage,4);
//			 ips200_show_int(Row_20,Col_9 ,BMotorPID.err_last,4);
//			 ips200_show_int(Row_20,Col_10,BMotorPID.pwm_out,4);
 
	 		
//-----------------------------电机转动数据-------------------------------------

//-----------------------------运行状态检查-------------------------------------

			 
//x: 左边------------------------------------------------------------------------------------------------

//-------------------------------Col_11到Col_16-----------------------------------------------------------------

//			 ips200_show_string(Row_1,Col_11 ,"L_pe");			ips200_show_int(Row_5,Col_11,Left_peak.Col,3);	//顶点
//			 ips200_show_string(Row_1,Col_12 ,"R_pe");			ips200_show_int(Row_5,Col_12,Right_peak.Col,3); //
// 			 ips200_show_string(Row_1,Col_12 ,"L_Srt:");		ips200_show_int(Row_6,Col_11,Left_line.Col[0],4);//边线初始点
// 		   ips200_show_string(Row_1,Col_13 ,"R_Srt:");		ips200_show_int(Row_6,Col_12,Right_line.Col[0],4);//边线初始点
 

//			 ips200_show_string(Row_1,Col_11 ,"L_lo");			ips200_show_int(Row_6,Col_11,Left_loss.Col,3);//丢线
//		   ips200_show_string(Row_1,Col_12 ,"R_lo");			ips200_show_int(Row_6,Col_12,Left_loss.Row,3); //

//		   ips200_show_string(0,16*12 ,"Ten_per");			ips200_show_float(8*8,16*12,Cross_Row_Per,1,2);//贯通行

			
//			 ips200_show_string(Row_1,Col_13 ,"L_Per");			ips200_show_float(Row_6,Col_13,Left_Row_Per,1,2);//贯通行
//			 ips200_show_string(Row_1,Col_14 ,"R_Per");			ips200_show_float(Row_6,Col_14,Right_Row_Per,1,2);     
//			 ips200_show_string(Row_1,Col_11 ,"Col_per");		ips200_show_float(Row_8,Col_11,Cross_Col_Per,1,2);  

//			 ips200_show_string(Row_1,Col_12 ,"L_ang");			ips200_show_float(Row_6,Col_12,Left_Side_Angle,3,2); //角度
//			 ips200_show_string(Row_1,Col_13 ,"LM_mang");		ips200_show_float(Row_7,Col_13,Left_Road_Angle,3,2);      
//			 ips200_show_string(Row_1,Col_14 ,"R_ang");			ips200_show_float(Row_6,Col_14,Right_Side_Angle,3,2); 
//			 ips200_show_string(Row_1,Col_15 ,"RM_ang");			ips200_show_float(Row_7,Col_15,Right_Road_Angle,3,2);

//			ips200_show_string(Row_1,Col_15 ,"X:");			  ips200_show_int(Row_6,Col_15,Box.X,3);//箱子数据
//			ips200_show_string(Row_1,Col_16 ,"Y:");				ips200_show_int(Row_6,Col_16,Box.Y,3);     

			
//			 ips200_show_string(Row_1,Col_16 ,"Curve");			ips200_show_uint(Row_6,Col_16,Box_Curve_State,1);//道路检测  
//-------------------------------Col_11到Col_16-----------------------------------------------------------------
			 
//		

//			ips200_show_string(Row_1,Col_17 ,"ang");				ips200_show_int(Row_6,Col_17,Turn.Offset,3);//旋转角度     
	
//			ips200_show_string(0,16*15 ,"L_S_F:");						ips200_show_int(8*7,16*15,Left_Start_flag,1);//起点标志位
//			ips200_show_string(0,16*16 ,"R_S_F:");						ips200_show_int(8*7,16*16,Right_Start_flag,1);     


//x: 左边------------------------------------------------------------------------------------------------

//x: 右边------------------------------------------------------------------------------------------------

//-------------------------------Col_11到Col_16-----------------------------------------------------------------

//			ips200_show_string(8*17,16*14 ,"Ro_1x:");			ips200_show_int(8*23,16*14,Round_peak_Col,4);//圆环第一阶段角点
//			ips200_show_string(8*17,16*15 ,"Ro_1y:");			ips200_show_int(8*23,16*15,Round_peak_Row,4);//圆环第一阶段角点
//		  ips200_show_int(8*17,16*14,Round_peak_Row,4);//起点标志位
//		  ips200_show_int(8*17,16*15,Round_peak_Col,4);//起点标志位

			
//			ips200_show_string(Row_17,Col_12,"O_S");					ips200_show_int(Row_23,Col_12,Origin_State,4);//箱子前的状态		
//			ips200_show_string(Row_17,Col_13,"W_S");					ips200_show_int(Row_23,Col_13,Work_State,4);	//当前状态
			ips200_show_string(Col_17,Row_11,"ANG");					ips200_show_int(Col_23,Row_11,Gyro_Angle.YawAngle,4);//边线初始点	
			ips200_show_string(Col_17,Row_14 ,"CAR_X");				ips200_show_int(Col_23,Row_14,Car.Speed_X,4);		//X速度
			ips200_show_string(Col_17,Row_15 ,"CAR_Y");				ips200_show_int(Col_23,Row_15,Car.Speed_Y,4);		//Y速度
			ips200_show_string(Col_17,Row_16 ,"CAR_Z");				ips200_show_int(Col_23,Row_16,Car.Speed_Z,4);		//Z速度
//-------------------------------Col_11到Col_16-----------------------------------------------------------------

//-------------------------------Col_17到Col_19-----------------------------------------------------------------

//			if(CLASSIFY_DATA.small-101>=0)	 ips200_show_string(Row_23,Col_19,PLACE_TABLE_STR[CLASSIFY_DATA.small-101]);
//			if(Box_DATA[Box_Index].small-101>=0)ips200_show_string(Row_23,Col_19,PLACE_TABLE_STR[Box_DATA[Box_Index].small-101]);
		
//		ips200_show_uint(Row_23,Col_17,Start_Angle,3);
//		ips200_show_uint(Row_23,Col_18,Start_Curve,1);
//		ips200_show_uint(Row_18,Col_18,Box_Road_Err,3);
//		ips200_show_string(Row_17,Col_17 ,"X_F");					ips200_show_int(Row_23,Col_17,X_flag,4);		//箱子运动状态
//		ips200_show_string(Row_17,Col_18 ,"Y_F");					ips200_show_int(Row_23,Col_18,Y_flag,4);		
//		ips200_show_string(Row_17,Col_19 ,"B_S");					ips200_show_int(Row_23,Col_19,Box_State,4); 


//			ips200_show_string(Row_17,Col_17 ,"Cr_Lx:");				ips200_show_int(Row_23,Col_17,LCrossPeak.Col,4);//十字顶点
//			ips200_show_string(Row_17,Col_18 ,"Cr_Rx:");				ips200_show_int(Row_23,Col_18,RCrossPeak.Col,4);//十字顶点
																																									 
//			ips200_show_string(Row_17,Col_18 ,"Ro_2x:");				ips200_show_int(Row_23,Col_18,Round_peak.Col,4);		 //圆环顶点
//			ips200_show_string(Row_17,Col_19 ,"Ro_2y:");				ips200_show_int(Row_23,Col_19,Round_peak.Row,4);		 //圆环顶点			
//			ips200_show_string(Row_17,Col_18 ,"Rou_F:");				ips200_show_int(Row_23,Col_18,Roundabout_DirFlag,1);//圆环区别位
//			ips200_show_string(Row_17,Col_19 ,"Rou_S:");				ips200_show_int(Row_23,Col_19,Roundabout_State,1);	 //圆环标志位
//-------------------------------Col_17到Col_19-----------------------------------------------------------------


//x: 右边------------------------------------------------------------------------------------------------
				
//-----------------------------------------运行状态检查---------------------------------------------------

//y: 192 ~ 320------------------------------------------------------------------------------------------------

}
	
