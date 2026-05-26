#ifndef _UART_h_
#define _UART_h_
#include "zf_common_headfile.h"

#define DEBUG_UART 0  //测试串口收发   1开启debug  0关闭debug (因为不想接USB-TTL线)
//串口3尚未用
#define UART_FINE_TUNING (UART_3)
#define UART_FINE_TUNING_CREW (LPUART3_IRQn)

//串口4模型串口
#define UART_RECOGNIZE (UART_4)
#define UART_RECOGNIZE_CREW (LPUART4_IRQn)

//串口1色块串口
#define UART_FINDBOX  (UART_1)
#define UART_FINDBOX_CREW (LPUART1_IRQn)


#define UART_BAUDRATE (DEBUG_UART_BAUDRATE)


typedef enum MainType
{
    None = -1,
    PERIPHERAL,     //电子外设				 //左
    TOOLS,       		//交通常用工具 	 //右 
    Handwritte_num  //手写体的数字	   //奇左偶右
}MainType_t;


typedef enum SmallType
{
    non = -2,
    A = 0,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    
}SmallType_t;


typedef enum Push
{
    No,
    Left,
    Right,
}pust;

// 箱子识别结构体
typedef struct Push_Box
{
    bool IS_CLASSIFY;
    pust 	push;
		SmallType_t small;
    MainType_t main;
}BoxClassfiytypeDef;

typedef struct CLASSIFY
{
    bool IS_CLASSIFY;
    SmallType_t small;
    MainType_t main;
}CLASSIFYtypeDef;

typedef struct BOXREC
{
		uint8_t num;
    SmallType_t small;
    MainType_t main;
}BoxRectypeDef;


typedef struct Uart
{
    uart_index_enum UART_INDEX;        // 注冊串口编号
    uint8_t uart_get_data[1024]; // 串口接收缓存区
    uint8_t fifo_get_data[1024]; // fifo 输出读出缓冲区
		uint32_t index;
    uint8_t get_data;         // 接收数据变量
    uint32 fifo_data_count; // fifo 数据个数
	fifo_struct uart_data_fifo;
}UART;

extern CLASSIFYtypeDef   CLASSIFY_DATA;
extern BoxClassfiytypeDef   Box_DATA[24];
extern const SmallType_t PLACE_TABLE[16];


extern UART _UART_FINDBOX;
extern UART _UART_FINE_TUNING;
extern UART _UART_RECOGNIZE;

extern uint8  Box_Index;

void UART_Init(void);
void UART_init(UART *uart, IRQn_Type UART_PRIORITY, uart_index_enum UART_INDEX);
double UART_ReadBuffer(UART *uart);
void UART_SendByte(UART* uart, uint8_t data);
void Uart_FindBox_Receive(void);
void Uart_Recognize_Receive(void);
void Uart_Fine_Tuning_Receive(void);

#endif