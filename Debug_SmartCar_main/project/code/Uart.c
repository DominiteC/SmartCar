#include "Uart.h"

UART _UART_FINDBOX;
UART _UART_FINE_TUNING;
UART _UART_RECOGNIZE;
	
CLASSIFYtypeDef   		CLASSIFY_DATA;
BoxClassfiytypeDef    Box_DATA[24];

uint8  Box_Index = 0;

const MainType_t  	MAIN_TABLE[4]       = {None, PERIPHERAL, TOOLS, Handwritte_num};
const char*     		HUGE_PLACE_STR[3]   = {"1", "2", "3"};
const SmallType_t   PLACE_TABLE[16]     = {non, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O};


/**@brief    初始化串口(内部调用)
-- @param    传入结构体地址, 传入中断组
-- @return   None
-- @auther   陈锦辉
-- @date     2023/12/23
**/

void UART_Init(void)
{
    UART_init(&_UART_FINDBOX, UART_FINDBOX_CREW, UART_FINDBOX);
	  UART_init(&_UART_RECOGNIZE, UART_RECOGNIZE_CREW, UART_RECOGNIZE);
//		UART_init(&_UART_FINE_TUNING, UART_FINE_TUNING_CREW, UART_FINE_TUNING);

}


void UART_init(UART *uart, IRQn_Type UART_PRIORITY, uart_index_enum UART_INDEX)
{
    uart->UART_INDEX = UART_INDEX;
	uart->index = 0;
	fifo_init(&uart->uart_data_fifo, FIFO_DATA_8BIT, uart->uart_get_data, 1024); // 初始化 fifo 挂载缓冲区
    switch (UART_INDEX)
    {
			case UART_FINDBOX:
        uart_init(UART_INDEX, UART_BAUDRATE, UART1_TX_B12, UART1_RX_B13); // 初始化串口引脚
				interrupt_set_priority(UART_PRIORITY, 1); // 设置对应 UART_INDEX 的中断优先级为 0
				uart_rx_interrupt(UART_INDEX, ZF_ENABLE); // 开启 UART_INDEX 的接收中断
      break;
			

    case UART_FINE_TUNING:
        uart_init(UART_INDEX, UART_BAUDRATE, UART3_TX_C8, UART3_RX_C9);
		interrupt_set_priority(UART_PRIORITY, 2); // 设置对应 UART_INDEX 的中断优先级为 0
		uart_rx_interrupt(UART_INDEX, ZF_ENABLE); // 开启 UART_INDEX 的接收中断        
		break;
			
			
    case UART_RECOGNIZE:
        uart_init(UART_INDEX, UART_BAUDRATE, UART4_TX_C16, UART4_RX_C17);
        interrupt_set_priority(UART_PRIORITY, 0); // 设置对应 UART_INDEX 的中断优先级为 0
				uart_rx_interrupt(UART_INDEX, ZF_ENABLE); // 开启 UART_INDEX 的接收中断   
    break;
		
    default:
        break;
    }
	
    uart_write_byte(UART_INDEX, '\r');        // 输出回车
    uart_write_byte(UART_INDEX, '\n');        // 输出换行
}

/**
 * @brief: 串口发送一个字节 
 * @param: 串口索引, 发送的字节
 * @param: #define UART_CLASSIFY_PIC (0x06) // 开始识别分类的图像
   @param: #define UART_CLASSIFY_SMALLPLACE (0x07) // 开始识别小类放置区域
   @param: #define UART_CLASSIFY_BIGPLACE  (0x08)  // 开始识别大类放置区域
 * @return： None
 */
void UART_SendByte(UART* uart, uint8_t data)
{
    uart_write_byte(uart->UART_INDEX, data);
}

/**@brief    读取缓存当中的数据
-- @param    传入想要获取数据的结构体地址
-- @return   返回double类型数据
-- @auther   陈锦辉
-- @date     2023/12/23
**/
double UART_ReadBuffer(UART *uart)
{
    uart->fifo_data_count = fifo_used(&uart->uart_data_fifo);
    if (uart->fifo_data_count != 0)
    {
        fifo_read_buffer(&uart->uart_data_fifo, uart->fifo_get_data, &uart->fifo_data_count, FIFO_READ_AND_CLEAN);
        #if DEBUG_UART
            uart_write_buffer(uart->UART_INDEX, uart->fifo_get_data, uart->fifo_data_count);
        #endif
        // 因为从串口读取的数据为十六进制，所以可以直接强转double(带符号不影响)
        return (double)uart->fifo_get_data[0];
    }
    else
    {
        return NULL;
    }
}




///**@brief   找目标板openart串口
//-- @param   无
//-- @author  陈锦辉
//-- @date    2023/12/23
//**/
void Uart_FindBox_Receive(void)
{
  static uint8_t tailLastData;

	uart_query_byte(UART_1, &_UART_FINDBOX.get_data);

  _UART_FINDBOX.fifo_get_data[_UART_FINDBOX.index] = _UART_FINDBOX.get_data;
  _UART_FINDBOX.index++;

  if (_UART_FINDBOX.get_data == 0x7e && tailLastData == 0xfe)
  {
    if (_UART_FINDBOX.index == 7)
    {     			
			if (_UART_FINDBOX.fifo_get_data[0] == 0x01)
      {
//			  printf("Found box\r\n");
				Box.X = _UART_FINDBOX.fifo_get_data[1];
				Box.Y = _UART_FINDBOX.fifo_get_data[2];
				Box.W = _UART_FINDBOX.fifo_get_data[3]; 
				Box.H = _UART_FINDBOX.fifo_get_data[4];
				Box.S=  Box.W * Box.H;
//				printf("X:%d\r\nY:%d\r\nS:%d\r\n",Box.X,Box.Y,S);
		
      } 
      memset(_UART_FINDBOX.fifo_get_data, 0, sizeof(_UART_FINDBOX.fifo_get_data));
      _UART_FINDBOX.index = 0;
    }
    else
    {
      memset(_UART_FINDBOX.fifo_get_data, 0, sizeof(_UART_FINDBOX.fifo_get_data));
      _UART_FINDBOX.index = 0;
    }
  }
  tailLastData = _UART_FINDBOX.get_data;
}

//模型识别串口
void Uart_Recognize_Receive(void)
{
  static uint8_t lastData;
  uart_query_byte(UART_4, &_UART_RECOGNIZE.get_data);
  _UART_RECOGNIZE.fifo_get_data[_UART_RECOGNIZE.index] = _UART_RECOGNIZE.get_data;
  _UART_RECOGNIZE.index++;

  // fifo_write_buffer(&_UART_RECOGNIZE.uart_data_fifo, &_UART_RECOGNIZE.get_data, 1);
  // printf("index: %d\n", _UART_RECOGNIZE.index);
  // 微调状态
	//  // 识别图片并接收识别种类状态
  if (_UART_RECOGNIZE.get_data == 0xef && lastData == 0xfe)
  {

    if (_UART_RECOGNIZE.index == 5)
    {
			if (_UART_RECOGNIZE.fifo_get_data[0] == 0xff && _UART_RECOGNIZE.fifo_get_data[1] == 0xee)
      {
												
        CLASSIFY_DATA.IS_CLASSIFY = true;
        CLASSIFY_DATA.small = _UART_RECOGNIZE.fifo_get_data[2];
        /*
         *  工具 TOOLS: 扳手A101, 电烙铁B102，手电钻C103，卷尺D104, 螺丝刀E105, 钳子F106，示波器G107，万用表H108
         *  电子外设 PERIPHERAL: 打印机I109, 键盘J110, 手机K111, 鼠标L112, 头戴式耳机M113, 显示器N114, 音响O115 
         */
        if (CLASSIFY_DATA.small >=101 && CLASSIFY_DATA.small <=107)
        {
          CLASSIFY_DATA.main = TOOLS;
        }
        else if ( CLASSIFY_DATA.small >=108 && CLASSIFY_DATA.small <=115)
        {
          CLASSIFY_DATA.main = PERIPHERAL;
        }
        else if(CLASSIFY_DATA.small >= 1 && CLASSIFY_DATA.small <= 100)
        {
          CLASSIFY_DATA.main = Handwritte_num;
        }
				else
				{
					CLASSIFY_DATA.main = None;
				}
				
			}		
				memset(_UART_RECOGNIZE.fifo_get_data, 0, sizeof(_UART_RECOGNIZE.fifo_get_data));
				_UART_RECOGNIZE.index = 0;

    }
    else
    {
      memset(_UART_RECOGNIZE.fifo_get_data, 0, sizeof(_UART_RECOGNIZE.fifo_get_data));
      _UART_RECOGNIZE.index = 0;
			
			 CLASSIFY_DATA.IS_CLASSIFY = false;
       CLASSIFY_DATA.small = non;
       CLASSIFY_DATA.main = None;
    }
  }
	  lastData = _UART_RECOGNIZE.get_data;
}


/**@brief   微调openart串口
-- @param   无
-- @auther  戴骐阳
-- @date    2023/12/23
**/
void Uart_Fine_Tuning_Receive(void)
{
    static uint8_t tailLastData;
  uint16_t S;

	uart_query_byte(UART_3, &_UART_FINE_TUNING.get_data);

  _UART_FINE_TUNING.fifo_get_data[_UART_FINE_TUNING.index] = _UART_FINE_TUNING.get_data;
  _UART_FINE_TUNING.index++;

  if (_UART_FINE_TUNING.get_data == 0x7e && tailLastData == 0xfe)
  {
    if (_UART_FINE_TUNING.index == 7)
    {     			
			if (_UART_FINE_TUNING.fifo_get_data[0] == 0x01)
      {
//			  printf("Found box\r\n");
				Box.X = _UART_FINE_TUNING.fifo_get_data[1];
				Box.Y = _UART_FINE_TUNING.fifo_get_data[2];
				Box.W = _UART_FINE_TUNING.fifo_get_data[3]; 
				Box.H = _UART_FINE_TUNING.fifo_get_data[4];
				Box.S=  Box.W * Box.H;
//				printf("X:%d\r\nY:%d\r\nS:%d\r\n",Box.X,Box.Y,S);
//				printf("X:%d\r\nY:%d\r\nW:%d\r\nH:%d\r\nS:%d\r\n",Box.X,Box.Y,Box.W,Box.H,S);

      } 
      memset(_UART_FINE_TUNING.fifo_get_data, 0, sizeof(_UART_FINE_TUNING.fifo_get_data));
      _UART_FINE_TUNING.index = 0;
    }
    else
    {
      memset(_UART_FINE_TUNING.fifo_get_data, 0, sizeof(_UART_FINE_TUNING.fifo_get_data));
      _UART_FINE_TUNING.index = 0;
    }
  }
  tailLastData = _UART_FINE_TUNING.get_data;

}