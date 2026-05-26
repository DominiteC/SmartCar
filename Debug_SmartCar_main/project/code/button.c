/**
  ******************************************************************************
  * @file    Button.c
  * @author  陈锦辉
  * @brief   按键驱动
  * @date    11/5/2023
    @verbatim
    轻触按键  拨码按键
    @endverbatim
  * @{
**/

/* Includes ------------------------------------------------------------------*/
#include "Button.h"

/* Define\Declare ------------------------------------------------------------*/
int8 Button_Value[4] = {0,0,0,0};//键值
int8 Button[4] = {C13,C15,C14,C12};
//int8 Button[4] = {C13,C14,C12,0	};
int8 Button_Temp[4] = {0,0,0,0};
int8 Switch_Button_Value[2] = {0,0};
uint16 Key_Time[4] = {0,0,0,0};
#define Key_Sleep_Time 4
//按键管脚定义
#define Switch_Button_0 C26
#define Switch_Button_1 C27

/**
 ******************************************************************************
 *  @defgroup 外部调用
 *  @brief
 *
**/

/**@brief   所有按键初始化
-- @param   无
-- @author  陈锦辉
-- @date    2023/11/5
**/
void All_Button_Init()
{
    gpio_init(C15,GPI,0,GPI_PULL_UP);
    gpio_init(C14,GPI,0,GPI_PULL_UP);
	  gpio_init(C13,GPI,0,GPI_PULL_UP);
    gpio_init(C12,GPI,0,GPI_PULL_UP);
//    gpio_init(Switch_Button_0,GPI,0,GPI_PULL_UP);
//    gpio_init(Switch_Button_1,GPI,0,GPI_PULL_UP);

	
    // gpio_init(Button[3],GPI,0,GPI_PULL_UP);
    // gpio_init(Button[4],GPI,0,GPI_PULL_UP);
    // gpio_init(Switch_Button_0,GPI,0,GPI_PULL_UP);
    // gpio_init(Switch_Button_1,GPI,0,GPI_PULL_UP);
}

/**@brief    所有按键扫描
-- @param    无
-- @author   陈锦辉
-- @verbatim 包括轻触按键和拨码按键
-- @verbatim 需要具体获取哪个按键的状态就判断对应的键值，例如按键0就判断Button_Value[0],判断完记得清零
-- @date     2023/11/5
**/
void All_Button_Scan()
{
    for(int Button_Num = 0;Button_Num <= 3;Button_Num++)
    {
        Get_Button_Value(Button_Num);
    }

    // for(int Switch_Button_Num = 0;Switch_Button_Num <= 1;Switch_Button_Num++)
    // {
    //     Get_Switch_Button_Value(Switch_Button_Num);
    // }
}

/**@brief   所有按键消抖
-- @param   无
-- @author  陈锦辉
-- @date    2024/1/21
**/
void Key_Delay()
{
    for(uint8 Num = 0;Num<=3;Num++)
    {
        if(Key_Time[Num] != 0)
        {
            Key_Time[Num]++;
        }
    }
}

/**
 ******************************************************************************
 *  @defgroup 内部调用
 *  @brief
 *
**/

/**@brief    获取轻触按键键值
-- @param    无
-- @author   陈锦辉
-- @verbatim 采用状态机的方法
-- @date     2023/11/5
**/
void Get_Button_Value(int8 KeyNum)
{
	switch(Button_Temp[KeyNum])
    {
        case 0://空闲状态
            if(gpio_get_level(Button[KeyNum]) == 0)
            {
                Button_Temp[KeyNum] = 1;//消抖
                Key_Time[KeyNum] = 1;//开始计时
            }
        break;
        case 1://消抖状态
            if((Key_Time[KeyNum] > Key_Sleep_Time) && (gpio_get_level(Button[KeyNum]) == 0))//大于消抖时间且仍为低电平
            {
                Button_Temp[KeyNum] = 2;//进入等待释放或长按计时
            }
        break;
        case 2://判断长按还是短按
            if(gpio_get_level(Button[KeyNum]) == 1)//如果按键抬起
            {
                Button_Temp[KeyNum] = 0;//进入等待释放或长按计时
                Button_Value[KeyNum] = 2;//判断为短按           
            }

            if((Key_Time[KeyNum] > Key_Sleep_Time*100) && (gpio_get_level(Button[KeyNum]) == 0))//大于长按时间
            {
                Button_Temp[KeyNum] = 3;//进入等待释放
                Button_Value[KeyNum] = 3;     
            }
        break;
        case 3://等待释放
            if(gpio_get_level(Button[KeyNum]) == 1)
            {
                Key_Time[KeyNum] = 0;
                Button_Temp[KeyNum] = 0;//进入等待释放或长按计时
                Button_Value[KeyNum] = 0;               
            }
        break;
    }
}



/**@brief    获取拨码按键键值
-- @param    无
-- @author   陈锦辉
-- @date     2023/11/7
**/
void Get_Switch_Button_Value(int8 KeyNum)
{
    switch(KeyNum)
    {
        case 0:
            if(gpio_get_level(Switch_Button_0) == 0)
            {
                Switch_Button_Value[0] = 1;
            }
            else if (gpio_get_level(Switch_Button_0) == 1)
            {
                Switch_Button_Value[0] = 0;
            }
        break;
        case 1:
            if(gpio_get_level(Switch_Button_1) == 0)
            {
                Switch_Button_Value[1] = 1;
            }
            else if (gpio_get_level(Switch_Button_1) == 1)
            {
                Switch_Button_Value[1] = 0;
            }
        break;
    }
}

//if( KEY_SHORT_PRESS == key_get_state(KEY_1) ||
//            KEY_SHORT_PRESS == key_get_state(KEY_2) ||
//            KEY_SHORT_PRESS == key_get_state(KEY_3) ||
//            KEY_SHORT_PRESS == key_get_state(KEY_4))                            // 任意按键短按
//        {
//            // 短按的按键在松开时 状态才会被 key_scanner 置位为 KEY_SHORT_PRESS
//          printf("hajimi");
//            // 可以单独清除按键状态
//            key_clear_state(KEY_1);
//            key_clear_state(KEY_2);
//            key_clear_state(KEY_3);
//            key_clear_state(KEY_4);
//        }
//				  else if(KEY_LONG_PRESS == key_get_state(KEY_1) ||
//                KEY_LONG_PRESS == key_get_state(KEY_2) ||
//                KEY_LONG_PRESS == key_get_state(KEY_3) ||
//                KEY_LONG_PRESS == key_get_state(KEY_4))                         // 任意按键长按
//        {
//            // 长按的按键在按下期间会被 key_scanner 不断置位为 KEY_LONG_PRESS
//            // 所以即使清除了本次的状态 在下次扫描时依旧会判定为 KEY_LONG_PRESS
//            count_beep = 40;
//            // 也可以清除所有按键状态
//            key_clear_all_state();
//        }


///**@brief   显示箭头
//-- @param   int Row 需要显示的行号
//-- @author  陈锦辉
//-- @date    2023/11/13
//**/
//static void Arrow_Display(int Col)
//{
//    for(int Col_Num = 0;Col_Num <= 14;Col_Num++)
//    {
//        if(Col == Col_Num)
//        {
//            ips200_show_string(Row_0,Col_Num*16,">");
//        }
//        else
//        {
//					  ips200_show_string(Row_0,Col_Num*16," ");

////            if(!Menu.Image_Show)
////            {
////                ips200_show_string(Row_0,Col_Num*16," ");
////            }
////            else if(Col_Num >=6 && Menu.Image_Show)
////            {
////                ips200_show_string(Row_0,Col_Num*16," ");
////            }
//        }

//    }
//}

///**@brief   行切换
//-- @param   无
//-- @author  陈锦辉
//-- @date    2024/2/16
//**/
//static void Col_Change()
//{
//    if((Button_Value[2] == 2) && (Menu.Set_Line < 14))//顺时针转
//    {
//        Button_Value[2] = 0;
//        Menu.Set_Line++;
//    }
//    else if((Button_Value[0] == 2) && (Menu.Set_Line > 0))//逆时针转
//    {
//        Button_Value[0] = 0;
//        Menu.Set_Line--;
//    }
//		else if((Button_Value[2] == 2) && (Menu.Set_Line == 14))
//		{
//				Button_Value[2] = 0;
//        Menu.Set_Line=0;
//		}
//		else if((Button_Value[0] == 2) && (Menu.Set_Line == 0))
//		{
//				Button_Value[2] = 0;
//        Menu.Set_Line=14;
//		}
//}
