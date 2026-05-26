#include "Wireless.h"

uint8_t WirelessData_len;                       //无线串口获取的消息长度
uint8_t Data_buffer[32];                //无线串口获取的数据缓存区

extern int16 Angle;


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     无线串口模块初始化
// 参数说明     void
// 返回参数     void
// 使用示例     Wireless_Init();
// 备注信息     调用UART2 波特率230400 模块2.0V版本以上可自适应波特率
//-------------------------------------------------------------------------------------------------------------------
void Wireless_Init(void)
{
		wireless_uart_init();
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     无线串口数据读取
// 参数说明     void
// 返回参数     获取到数据: 1 没获取到数据: 0
// 使用示例     Wireless_ReadMessage();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8_t Wireless_ReadMessage(void)
{
    // 查看是否有消息,将消息存入data_buffer,并计算消息长度
    WirelessData_len = (uint8)wireless_uart_read_buffer(Data_buffer, 32);
		int i;
    //获取到数据
    if(WirelessData_len != 0)                                                                      // 收到了消息 读取函数会返回实际读取到的数据个数
    {
				
			wireless_uart_send_buffer(Data_buffer, WirelessData_len);                            //将受到的信息发到电脑助手上
       
        //调试时使用  
//			  if(strcmp("Up\r\n",(const char *)Data_buffer)==0)State=Ahead;                      //前进
//				if(strcmp("Left\r\n",(const char *)Data_buffer)==0)State=Left_RUN;                 //左平移
//				if(strcmp("Right\r\n",(const char *)Data_buffer)==0)State=Right_RUN;               //右平移
//				if(strcmp("S\r\n",(const char *)Data_buffer)==0)State=Stop;                        //停车
//				if(strcmp("Down\r\n",(const char *)Data_buffer)==0)State=Back;                     //后退
//				if(strcmp("Q\r\n",(const char *)Data_buffer)==0)State=Leftaround; //向左改变的角度
//				if(strcmp("E\r\n",(const char *)Data_buffer)==0)State=Rightaround; //向右改变的角度
																																										 //
//				if(strcmp("S\r\n",(const char *)Data_buffer)==0)stop_flag=1;                         //停车
//				if(strcmp("Up\r\n",(const char *)Data_buffer)==0)stop_flag=0;Angle=0;                            //前进
//				if(strcmp("Q\r\n",(const char *)Data_buffer)==0){stop_flag=0;Angle+=10;if(Angle>180)Angle=-180;} //向左改变的角度
//				if(strcmp("E\r\n",(const char *)Data_buffer)==0){stop_flag=0;Angle-=10;if(Angle<-180)Angle=180;} //向右改变的角度
        memset(Data_buffer, 0, 32);   																											 // 清除数据缓存        
        return 1;
    }

    return 0;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     无线串口数据处理
// 参数说明     void
// 返回参数     void
// 使用示例     Wireless_handler();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Wireless_handler(void)
{
//    uint8_t i, j;

    //没有获取到蓝牙数据
    if(Wireless_ReadMessage() == 0)
    {
        return;
    }


}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     无线串口数据处理
// 参数说明     void
// 返回参数     void
// 使用示例     Wireless_handler();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Wire_handler(void)
{
			Data_buffer[0]=Cross_State;
	    WirelessData_len = (uint8)wireless_uart_read_buffer(Data_buffer, 32);
			wireless_uart_send_buffer(Data_buffer,WirelessData_len);
        memset(Data_buffer, 0, 32);   																											 // 清除数据缓存        

}