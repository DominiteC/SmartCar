/*********************************************************************************************************************
* MCX Vision Opensourec Library 即（MCX Vision 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2024 SEEKFREE 逐飞科技
* 
* 本文件是 MCX Vision 开源库的一部分
* 
* MCX Vision 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
* 
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
* 
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
* 
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
* 
* 文件名称          main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          MDK 5.38a
* 适用平台          MCX Vision
* 店铺链接          https://seekfree.taobao.com/
* 
* 修改记录
* 日期              作者                备注
* 2024-04-21        ZSY            first version
********************************************************************************************************************/
#include "zf_model_process.h"
#if defined(__cplusplus)
extern "C" // mian文件是C++文件，如果需要包含C语言的头文件，就需要使用extern "C"
{
#endif /* __cplusplus */ 
#include "zf_common_headfile.h"
#include "color_tracer.h"

// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// 核心板下载完代码需要手动复位！！！
// 核心板下载完代码需要手动复位！！！
// 核心板下载完代码需要手动复位！！！.
    
// 如果程序运行后，模块上的Core灯闪烁，说明程序进入了HardFault，就需要检测是否有数组越界，外设没有初始化，外设时钟没有设置等情况
    
// *************************** 例程测试说明 ***************************
// 1.使用DAP下载器连接模块,将IPS2.0屏幕插到模块上，注意引脚方向
// 
// 2.MCX Vision下载本例程，
// 
// 3.复位核心板（需要手动复位） 
// 
// 4.屏幕会显示实时图像并框出色块目标，通过按键可以根据屏幕中心颜色切换识别色彩

// **************************** 代码区域 ****************************
    
// KEY引脚定义
gpio_struct gpio_key_1 =        {GPIO4, 2u};
//#define MAX_OD_BOX_CNT              (12  )

int main(void)
{
    // 时钟和调试串口-串口4初始化
    zf_board_init();
    // 用户串口-串口5初始化
    user_uart_init();
    // 延时300ms
    system_delay_ms(300);
    // 使用C++编译无法使用printf，可以使用zf_debug_printf和zf_user_printf替代
    zf_debug_printf("debug_uart_init_finish\r\n");  // 使用调试串口-串口4发送数据
    zf_user_printf("user_uart_init_finish\r\n");    // 使用用户串口-串口5发送数据

    // 初始化按键
    gpio_init(gpio_key_1, GPI, 0, PULL_UP);
    // 屏幕初始化
    ips200_init();
    // 摄像头初始化
    scc8660_init();
	
//	odresult_t s_odrets[MAX_OD_BOX_CNT];
	int s_odretcnt = 0;

    while (1)
    {
			if(scc8660_finish)
			{
				scc8660_finish = 0;
//				zf_model_run();
							
				ips200_show_scc8660((uint16_t*)g_camera_buffer);
				
				
				if(!gpio_get_level(gpio_key_1))
				{
						// 通过图像中心数据设置色块识别阈值
						set_color_target_condi((*((uint16*)g_camera_buffer + SCC8660_H/2 * SCC8660_W + SCC8660_W/2)), &target_color_condi);

				}
				
				if(color_trace(&target_color_condi, &target_pos_out))
				{
//					zf_debug_printf("Found boxes count\r\n");
//						zf_debug_printf(" x: %d, y: %d, w: %d, h: %d\r\n",
//					target_pos_out.x, target_pos_out.y, target_pos_out.w, target_pos_out.h);
//							zf_debug_printf(" %d, %d, %d, %d, %d, %d\r\n",
//					target_color_condi.h_max,target_color_condi.h_min,target_color_condi.s_max,target_color_condi.s_min,target_color_condi.l_max,target_color_condi.l_min);
				
							
							uint8_t UartBuffer[7] = {0x01, target_pos_out.x,  target_pos_out.y,target_pos_out.w, target_pos_out.h, 0xfe, 0x7e};
//							uint8_t UartBuffer[7] = {0x01, target_color_condi.h_max,  target_color_condi.h_min,target_color_condi.s_max, target_color_condi.s_min, 0xfe, 0x7e};						 
							
							user_uart_write_buffer(UartBuffer, sizeof(UartBuffer));
//							system_delay_ms(10);
							
//						ips200_draw_line((target_pos_out.x - target_pos_out.w/2), (target_pos_out.y + target_pos_out.h/2), (target_pos_out.x + target_pos_out.w/2), (target_pos_out.y + target_pos_out.h/2), 0xffff);
//						ips200_draw_line((target_pos_out.x - target_pos_out.w/2), (target_pos_out.y - target_pos_out.h/2), (target_pos_out.x + target_pos_out.w/2), (target_pos_out.y - target_pos_out.h/2), 0xffff);
//						ips200_draw_line((target_pos_out.x - target_pos_out.w/2), (target_pos_out.y - target_pos_out.h/2), (target_pos_out.x - target_pos_out.w/2), (target_pos_out.y + target_pos_out.h/2), 0xffff);
//						ips200_draw_line((target_pos_out.x + target_pos_out.w/2), (target_pos_out.y - target_pos_out.h/2), (target_pos_out.x + target_pos_out.w/2), (target_pos_out.y + target_pos_out.h/2), 0xffff);
				}
				
				
				

			}
    }
}

// **************************** 代码区域 ****************************
#if defined(__cplusplus)
}
#endif /* __cplusplus */

///**
// * @brief 串口接收中断，配合状态机转换
// * @param data 
// * @return none
// */
//void UART5_IRQHandler(uint8 data) {
//    switch (data)
//    {
//    case UART_STARTFINETUNING_BESIDEROAD: 
//        currMode = TUNING_BESIDEROAD;
//        break;
//    case UART_STARTFINETUNING_INELEMENTS: // 在接收到开始微调命令后， 把目标板是否存在标志位设置为false
//        IS_ALIVE_FLAG = false;
//        currMode = TUNING_INELEMETS;
//        break;
//    case UART_STARTFINETUNING_PLACE:
//        currMode = TUNING_PLACE;
//        break;
//    case UART_UNLOAD_FLAG:
//        IS_ALIVE_FLAG = false;
//        currMode = TUNING_UNLOAD;
//        break;
//    case UART_FINETUNING_BIGPLACE:
//        IS_ALIVE_FLAG = false;
//        currMode = TUNING_BIGPLACE;
//        break;
//    case UART_WAITING:
//        currMode = NONE;
//        break;
//    case UART_START_RUN:
//        setMode = RUN;
//        break;
//    case UART_GET_SMALLPLACE:
//        IS_ALIVE_FLAG = false;
//        currMode = FIND_SMALL_PLACE;
//        break;
//    case UART_FIND_BIGPLACE:
//        currMode = FIND_BIG_PLACE;
//        break;
//    default:
//        break;
//    }
//}