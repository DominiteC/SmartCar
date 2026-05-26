#include "UserMain.h"
#include "zf_common_headfile.h"

#define IPS200_TYPE     (IPS200_TYPE_SPI)                                 // 双排排针 并口两寸屏 这里宏定义填写 IPS200_TYPE_PARALLEL8
																																								// 单排排针 SPI 两寸屏 这里宏定义填写 IPS200_TYPE_SPI

void IMU_Init()
{
    while (1)
    {
        if (imu660ra_init())
            system_delay_ms(1000);
        else
            break;
        system_delay_ms( 2000);
    }
    system_delay_ms(100);
    Gyro_Offset_Init();
}

void Mt9v03x_Init()
{
    while (1)
    {
        if (mt9v03x_init())
            system_delay_ms(1000);
            // ips200_show_string(0, 16, "mt9v03x reinit.");
        else
            break;
        system_delay_ms(1000); // 闪灯表示异常
    }
}


void User_Init()
{
		ips200_set_dir(IPS200_PORTAIT);
    ips200_set_font(IPS200_8X16_FONT); 
    ips200_set_color(RGB565_BLACK, RGB565_WHITE);
    ips200_init(IPS200_TYPE);

//	    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_DEBUG_UART);

//	    seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, MyImage[0], MyImage_W, MyImage_H);

//		 Flash_Init();
	  key_init(10);																	//按键初始化
//		Wireless_Init();															//无线传输初始化
		IMU_Init();																		//陀螺仪初始化
		Mt9v03x_Init();																//摄像头初始化 
		UART_Init();																	//串口初始化 													
    All_Button_Init();

	
		PID_Init();																		//pid初始化
		MyImage_Init();																//备份图像数组初始化
		Data_Init();                                  //数据初始化`
		Motor_Init();																	//电机初始化

		pit_ms_init(PIT_CH0, 5); 											//pid和编码器用到的
//		pit_ms_init(PIT_CH1, 5); 											//无线收发用到的
//		pit_ms_init(PIT_CH2, 10);											//还没用到
		pit_ms_init(PIT_CH3, 1);											//还没用到
		timer_init(GPT_TIM_1,TIMER_MS);
//		system_delay_ms(1000);


}

PolyFitResult coffee;


void User_Loop()
{

	 if(mt9v03x_finish_flag)
        {

							mt9v03x_finish_flag = 0;				//清除标志位
							Data_Init();										//清空数据
							Image_Compress();								//裁剪				
							Image_Transfer(Bin_OTSU_mode);  //大津法二值化				
							Image_Filter();								  //滤波	
							if(Box_State==Rec_Angle){Get_LeftRight();Box_Line_Peak();}
							
//-------------------------------拟合-------------------------------
//							Get_LeftRight();
//							Box_Line_Peak();					
//							coffee = polyfit2(&Right_line.Col[0],&Right_line.Row[0],Len_Right);
//							printf("%f,%f,%f\r\n",coffee.a,coffee.b,coffee.c);
//							polyval2(&coffee,&Right_line.Col[0]);
//-------------------------------拟合-------------------------------
							
							
//							SideLine_Peak();
//							Line_Angle();	

							
							if(Box_State==No_box)
							{												
										GetLRSide();								//获取左右边线		
										SideLine_Peak();						//获取左右边线顶点						
										LossLine_Check();						//丢线检查										
							}
							if(Box_State == No_Duel)
							{
								Loss_line_Protect();				//出界保护
							}
							if(Stop_flag == 0)
							{
									All_Check();
									All_handler();			
							}
							
        }
//				 seekfree_assistant_camera_send();

//		printf("%d,%d,%d\r\n",FL_Motor.encoder,FR_Motor.encoder,B_Motor.encoder);				
				 Menu_Display();	
				
				
//---------------------------------------计时---------------------------------------------------------
//						timer_start(GPT_TIM_1);
//					timer_stop(GPT_TIM_1);
//					  printf("time = %d\r\n",timer_get(GPT_TIM_1));
//         timer_clear(GPT_TIM_1);
//---------------------------------------计时---------------------------------------------------------

}

