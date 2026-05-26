#include "Control.h"

//------------------------变量--------------------------------
uint8 X_flag=0;
uint8 Y_flag=0;
uint8 Turn_flag=0;
uint8 finish_flag=0;
int8 X_err;
int8 Y_err;
int8 Box_Road_Err;

float Angle_Erro = 0;

uint16 Time_Cnt1 = 0;
uint16 Time_Cnt2 = 0;

int 	Start_Angle = 0 ;
int 	Rec_Now_Angle = 0;

int8 	Start_Curve = -1;
uint8 Start_Curve_Flag=0;

int8 Box_Curve_State = -1;


Car_Handle Car = 
{
    .Speed_X = 0,
    .Speed_Y = 0,
    .Speed_Z = 0,
    
};

Turn_Handle Turn = 
{
    .Angle_Last = 0,
    .Current_Angle = 0,
    .Offset = 0,
    .Finish = false,
};

Turn_Handle Move = 
{
		.Angle_Last = 0,
    .Current_Angle = 0,
    .Offset = 0,
    .Finish = false,
};

Turn_Handle Aim = {0,0,0,0};
Navigation_Handle Navigation = {0,0,0,0,0,0,0,0,0,0,0};
uint8 Navigation_state = 0;
//------------------------变量--------------------------------


/**@brief   判断时间
-- @param   bool Cond 计时条件
-- @param   uint16 Cnt 计时时间
-- @author  陈锦辉
-- @date    2024/5/23
**/
bool Bufcnt1(bool Cond,uint16 Cnt)
{
		if(Cond)//满足条件
		{
				if(Time_Cnt1 == 0)
				{
						Time_Cnt1 = 1;//开始计时
				}
		}
		else
		{
			Time_Cnt1 = 0;
		}
		
    if(Time_Cnt1 >= Cnt)//时间到达设定
    {
        Time_Cnt1 = 0;
        return true;
    }
    else
    {
        return false;
    }
}

/**@brief   判断时间
-- @param   bool Cond 计时条件
-- @param   uint16 Cnt 计时时间
-- @author  陈锦辉
-- @date    2024/5/23
**/
bool Bufcnt2(bool Cond,uint16 Cnt)
{
    if(Cond)//满足条件
    {
        if(Time_Cnt2 == 0)
        {
            Time_Cnt2 = 1;//开始计时
        }
    }
		else
		{
			Time_Cnt2 = 0;
		}
		
    if(Time_Cnt2 >= Cnt)
    {
        Time_Cnt2 = 0;
        return true;
    }
    else
    {
        return false;
    }
}


////-------------------------------------------------------------------------------------------------------------------
//// 函数简介     整车移动
//// 参数说明     
//// 返回参数     
//// 使用示例     
//// 备注信息     大致方向移动，分7个状态
////-------------------------------------------------------------------------------------------------------------------

void Run_State(work_state1 a)
{
		if(a==Stop)
		{
				Car.Speed_X = 0;	Car.Speed_Y = 0; 	Car.Speed_Z = 0;	
		}
		if(a==Back)
		{
				Car.Speed_X = 0;	Car.Speed_Y = -150; 	Car.Speed_Z = 0;		
		}
		if(a==Ahead)
		{
				Car.Speed_X = 0;	Car.Speed_Y = 150; 	Car.Speed_Z = 0;		
		}
		if(a==Left_RUN)
		{
			Car.Speed_X = -80;	Car.Speed_Y = 0; 	Car.Speed_Z = 0;		
		}
			if(a==Right_RUN)
		{
				Car.Speed_X = 80;	Car.Speed_Y = 0; 	Car.Speed_Z = 0;	
		}
		if(a==Leftaround)
		{
			Car.Speed_X = -80;	Car.Speed_Y = 20; 	Car.Speed_Z = 50;	
		}
 		if(a==Rightaround)
		{
			Car.Speed_X = 80;	Car.Speed_Y = 20; 	Car.Speed_Z = -50;	
		}
		
}


///**@brief   巡线
//-- @param   无
//-- @author  陈锦辉
//-- @date    2025/5/30
//**/
void Car_run(float Speed)
{
    //停车标志位立起
    if(Stop_flag == 1)
    {
        SetSpeed = 0;
				Car.Speed_X = 0;
				Car.Speed_Y = 0;
				Car.Speed_Z = 0;
			
				
		}
		else 
		{
			    SetSpeed = -MotorSetSpeed;
					Car.Speed_X = 0;
					Car.Speed_Y = -Speed;
					Car.Speed_Z = 8.3 * Image_Erro;		
		}	
}

//1.25,,,1.33  //100


//-----------------------------------------------内部调用------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    tan值转角度
// 参数说明     void
// 返回参数     void
// 使用示例     tan_to_Angle( tan);
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
float tan_to_Angle(float tan)
{
	float radians = atan(tan);
	float angle = radians * 180.0 / PI;
	return angle;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    检查箱子信息
// 参数说明     void
// 返回参数     void
// 使用示例     
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
bool box_Rec(void)
{
	static uint8 k = 0;
	static uint8_t lastData;
	int a;
	
	if(CLASSIFY_DATA.IS_CLASSIFY == true)
	{
		if(lastData == CLASSIFY_DATA.small)
		{
				k++;
		}
		else
		{
				k=0;
		}
		CLASSIFY_DATA.IS_CLASSIFY = false;
	}
	
	lastData = CLASSIFY_DATA.small;
	if(k)
	{
		k = 0;
			Box_DATA[Box_Index].IS_CLASSIFY = true;
			Box_DATA[Box_Index].small = CLASSIFY_DATA.small;		
			Box_DATA[Box_Index].main = CLASSIFY_DATA.main;
			switch (Box_DATA[Box_Index].main)
			{
				case  None:
					
				Box_DATA[Box_Index].push = Left;
				
				break;
				
				case  PERIPHERAL:
					
				Box_DATA[Box_Index].push = Left;
				
				break;
				
				case  TOOLS:
				Box_DATA[Box_Index].push = Right;
				
				break;
				
				case  Handwritte_num:
					
				a=Box_DATA[Box_Index].small % 2;				
				if(a)Box_DATA[Box_Index].push = Left;
				else Box_DATA[Box_Index].push = Right;
				
				break;				
			}
				
				return 1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     Turn_box_Action()内部调用函数
// 参数说明     void
// 返回参数     void
// 使用示例     内部调用
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------

bool Move_Target_Angle(float Target_Angle)
{   
    Move.Current_Angle = Gyro_YawAngle_Get();//获取当前角度

    if(Move.Angle_Last == 0)
    {
        Move.Angle_Last = Move.Current_Angle;
    }

    Move.Offset += Move.Current_Angle - Move.Angle_Last;
    Move.Angle_Last = Move.Current_Angle;
    		
		if(Target_Angle - Move.Offset >= 3.0f)
		{
				X_Y_Z_Speed_Set(270,-90,185);						
		}
		else if(Target_Angle - Move.Offset <= -3.0f) 
		{
				X_Y_Z_Speed_Set(270,90,-185);					
		}
		else
		{
				Car.Speed_X = 0;
				Car.Speed_Y = 0;
				Car.Speed_Z = 0;
		}			
		if(Bufcnt2(fabs(Target_Angle - Move.Offset) <=3.0f,500))
		{		
        Move.Offset = 0;
        Move.Angle_Last = 0;
				return 1;
		}
		return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     接近箱子
// 参数说明     void
// 返回参数     void
// 使用示例     内部调用
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
bool Navigation_Process_Box(void)
{
    static uint16 Wait_Time = 0;//等待的时间
		X_err = 0.3*X_err + 0.7*(Box.X-86);
		Y_err = 0.3*Y_err + 0.7*(66-Box.Y);
    switch(Navigation_state)
    {
			
        case Start_State:
            X_Y_Z_Speed_Set(0,0,0);//先停一会
//            if(fabs(X_Speed) <= 0.1 && fabs(Y_Speed) <= 0.1)
//            {
                Navigation_state = Move_State;
                Enable_Navigation();
//            }
        break;
						
        case Move_State:
            //获取图像识别的坐标
							Car.Speed_Z = 5*Angle_Control(Navigation.Start_Angle);
							if(abs(X_err)>19 || abs(Y_err)>7 || X_flag == 0 || Y_flag == 0 )
							{	
								if(X_err>19) {X_flag=0 ;Car.Speed_X = 	20+1.5*X_err;}//+1.5*X_err;}	}
								if(Y_err>7 ) {Y_flag=0 ;Car.Speed_Y =   13+5*Y_err;}//30+5*Y_err;}
								if(X_err<-19){X_flag=0 ;Car.Speed_X =  -20+1.5*X_err;}//-3+X_err;}
								if(Y_err<-7 ){Y_flag=0 ;Car.Speed_Y =  -13+5*Y_err;}//-30+5*Y_err;}	
							}
							if(abs(X_err)<=19){Car.Speed_X = 0;X_flag=1;}
							if(abs(Y_err)<=7){Car.Speed_Y = 0;Y_flag=1;}
							if(Box.S<1100)	 {Y_flag=0;Car.Speed_Y = -200;}

							if(Bufcnt1(X_flag==1&&Y_flag==1, 1000))
							{							
									Navigation_state = Move_Finish;												
							}	
							
        break;
				
        case Move_Finish:
            Navigation.Finish_Flag = true;
            Car.Speed_X = 0;
            Car.Speed_Y = 0;
            Car.Speed_Z = 0;//Angle_Control(Navigation.End_Angle);
						FLMotorPID.pwm_out = 0;
						FRMotorPID.pwm_out = 0;
						BMotorPID .pwm_out = 0;
						Reset_Navigation();
						return 1;//一次惯性导航完成
        break;
    }
		return 0;

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     根据箱子信息转到相应地方
// 参数说明     void
// 返回参数     void
// 使用示例     内部调用
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------

bool Turn_box_Action(void)
{
	
	switch(Box_DATA[Box_Index].push)
	{
		case No:
				if(Start_Curve==No_Curve)			return Move_Target_Angle(-85);
				if(Start_Curve==Left_Curve)		return Move_Target_Angle(-95);
				if(Start_Curve==Right_Curve)	return Move_Target_Angle(-80);

			
		break;
		
		case Left://推箱往左
				if(Start_Curve==No_Curve)			return Move_Target_Angle(-85);
				if(Start_Curve==Left_Curve)		return Move_Target_Angle(-95);
				if(Start_Curve==Right_Curve)	return Move_Target_Angle(-80);
			 	
		break;

		case Right://推箱往右
				if(Start_Curve==No_Curve)			return Move_Target_Angle(85);
				if(Start_Curve==Left_Curve)		return Move_Target_Angle(80);
				if(Start_Curve==Right_Curve)	return Move_Target_Angle(95);
			 
	  break;
			
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     根据一开始的箱子偏差做出相应推箱子出赛道的时间
// 参数说明     void
// 返回参数     void
// 使用示例     内部调用
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------

bool Pull_box_Action(void)
{
		X_Y_Z_Speed_Set(400,0,0);	//推		
		if(Box_DATA[Box_Index].push == Left)
		{
				if(Bufcnt1(true,1300+Box_Road_Err))
				{
					X_Y_Z_Speed_Set(0,0,0);
					return 1;
				}						
		}
		else if(Box_DATA[Box_Index].push == Right)
		{
				if(Bufcnt1(true,1300-Box_Road_Err))
				{
					X_Y_Z_Speed_Set(0,0,0);
					return 1;
				}							
				
		}		
		
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     根据一开始的箱子偏差做出相应回到赛道的时间
// 参数说明     void
// 返回参数     void
// 使用示例     内部调用
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
bool Back_Road(void)
{
		X_Y_Z_Speed_Set(400,180,0);	//推		
		if(Box_DATA[Box_Index].push == Left)
		{
				if(Bufcnt1(true,700+Box_Road_Err))
				{
					X_Y_Z_Speed_Set(0,0,0);
					return 1;
				}						
		}
		else if(Box_DATA[Box_Index].push == Right)
		{
				if(Bufcnt1(true,700-Box_Road_Err))
				{
					X_Y_Z_Speed_Set(0,0,0);
					return 1;
				}							
				
		}		
		
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介    	推完箱子后回到赛道
// 参数说明     void
// 返回参数     void
// 使用示例     内部调用
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
bool Back_aim_Action(void)
{	
	
		switch(Box_DATA[Box_Index].push)
		{

			case No:
				X_Y_Z_Speed_Set(0,0,150);
				
			break;
			
			case Left://推箱往左
				X_Y_Z_Speed_Set(0,0,150);
				
					
			break;

			case Right://推箱往右	 	 			
				X_Y_Z_Speed_Set(0,0,-150);
				 
			break;		
		}
		if(Bufcnt1(true,700) &&	Cross_Col_Per >0.4)
		{
				Car.Speed_X = 0;	Car.Speed_Y = 0; 	Car.Speed_Z = 0;
				Box_State = Finish_box;
				return 1;
		}
		return 0;

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    	推完箱子后回到相应角度
// 参数说明     void
// 返回参数     void
// 使用示例     内部调用
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
bool Back_Start_Angle(void)
{	
	if(fabs(Angle_Control(Start_Angle))>=2)
	{ 
		if(fabs(Angle_Control(Start_Angle)>0))
		{			
			Car.Speed_X = 0;
			Car.Speed_Y = 0;
			Car.Speed_Z = 5*Angle_Control(Start_Angle);		
		}
		else if(fabs(Angle_Control(Start_Angle)<0))
		{
			Car.Speed_X = 0;
			Car.Speed_Y = 0;
			Car.Speed_Z = 5*Angle_Control(Start_Angle);		
		}
	}
	if(Bufcnt2(fabs(Angle_Control(Start_Angle))<=2,500))
	{		
			Car.Speed_X = 0;
			Car.Speed_Y = 0;
			Car.Speed_Z = 0;
			return 1;

	}
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    	回到赛道后转相应的角度使其正常行驶
// 参数说明     void
// 返回参数     void
// 使用示例     内部调用
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
bool Back_Start_Angle1(void)
{	
	switch(Box_DATA[Box_Index].push)
	{
		case No:
			if(Start_Curve == No_Curve)	
			{
				return Turn_Angle(85);
			}
			else if(Start_Curve == Left_Curve)	
			{
				return Turn_Angle(70);
			}
			else if(Start_Curve == Right_Curve)	
			{
				return Turn_Angle(112);
			}
			
		break;
		
		case Left://推箱往左	
			
			if(Start_Curve == No_Curve)	
			{
				return Turn_Angle(85);
			}
			else if(Start_Curve == Left_Curve)	
			{
				return Turn_Angle(70);
			}
			else if(Start_Curve == Right_Curve)	
			{
				return Turn_Angle(112);
			}
			 	
		break;

		case Right://推箱往右
	 	 			
			if(Start_Curve == No_Curve)	
			{
				return Turn_Angle(-85);				
			}
			else if(Start_Curve == Left_Curve)	
			{
				return Turn_Angle(-112);				
			}
			else if(Start_Curve == Right_Curve)	
			{
				return Turn_Angle(-70);				
			}		
		
		  break;			
	}
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介    	回到赛道后转相应的角度使其正常行驶
// 参数说明     void
// 返回参数     void
// 使用示例     内部调用
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
bool Curve_Rec(void)
{	
	switch(Start_Curve)
	{
		case No_Curve:
			
				return 1;
						
		break;
		
		case Left_Curve:	
			
			  return Turn_Angle(-33);
						 	
		break;

		case Right_Curve:
	 	 			
				return Turn_Angle(33);		
	 
		break;			
	}
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介    	让车保持相应角度平移
// 参数说明     void
// 返回参数     void
// 使用示例     内部调用
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
bool Back_Road_Move(void)
{	
		switch(Box_DATA[Box_Index].push)
	{

		case No:
			Car.Speed_X = 300;	Car.Speed_Y = 0;
			if(Start_Curve == No_Curve)	
			{
					Car.Speed_Z = 10*Angle_Control(Start_Angle);					
			}
			else if(Start_Curve == Left_Curve)	
			{
				  Car.Speed_Z = 10*Angle_Control(Start_Angle);					
			}
			else if(Start_Curve == Right_Curve)	
			{
					Car.Speed_Z = 10*Angle_Control(Start_Angle);					
			}

//			if(Bufcnt1(true,200) && Left_Start_flag || Right_Start_flag && Cross_Col_Per > 0.5)
			return 0;
		break;
		
		case Left://推箱往左		
			Car.Speed_X = 300;	Car.Speed_Y = 0;
			if(Start_Curve == No_Curve)	
			{
				 	Car.Speed_Z = 10*Angle_Control(Start_Angle);					
			}
			else if(Start_Curve == Left_Curve)	
			{
					Car.Speed_Z = 10*Angle_Control(Start_Angle);					
			}
			else if(Start_Curve == Right_Curve)	
			{
					Car.Speed_Z = 10*Angle_Control(Start_Angle);					
			}
//			if(Bufcnt1(true,200) && Left_Start_flag || Right_Start_flag && Cross_Col_Per > 0.5)
			
			 	
		break;

		case Right://推箱往右
	 	 	Car.Speed_X = -300;	Car.Speed_Y = 0;
			if(Start_Curve == No_Curve)	
			{
				 	Car.Speed_Z = 10*Angle_Control(Start_Angle);
			}
			else if(Start_Curve == Left_Curve)	
			{
				 	Car.Speed_Z = 10*Angle_Control(Start_Angle);					
			}
			else if(Start_Curve == Right_Curve)	
			{
					Car.Speed_Z = 10*Angle_Control(Start_Angle);					
			}

//			if(Bufcnt1(true,300) && Left_Start_flag || Right_Start_flag && Cross_Col_Per > 0.5)
			 
	  break;
	}
	if(Bufcnt1(true,300)	&& (Left_Start_flag || Right_Start_flag))
	{
			Car.Speed_X = 0;	Car.Speed_Y = 0; 	Car.Speed_Z = 0;
			return 1;
	}

	return 0;

}


//-----------------------------------------------内部调用------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    箱子弯道检测
// 参数说明     void
// 返回参数     void
// 使用示例     Box_Curve_Check();
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
void Box_Curve_Check(void)
{

		float temp_Angle;
	
		//双边可检测
			if(abs(abs(Len_Right)-abs(Len_Left))<30)
			{
				//左弯
				if(Right_Side_Angle >=-48.0 && Right_Side_Angle < 0 && Right_Side_Angle!=none)
				{		
						Box_Curve_State = Left_Curve;
						return;
				}							
				//双边可检测
				if(Left_Side_Angle<=48.0 && Left_Side_Angle>0 && Left_Side_Angle!=none )
				{				
						Box_Curve_State = Right_Curve;
						return;
				}
				if(Left_Side_Angle != none && Left_Side_Angle >48)
				{
						Box_Curve_State = No_Curve;
				}				
				else if(Right_Side_Angle != none && Right_Side_Angle <-48 )
				{
						Box_Curve_State = No_Curve;
				}
				if(Left_Side_Angle == none && Right_Side_Angle == none)
				{
						Box_Curve_State = No_Curve;
						return;
				}

			}
			else 
			{
				if(Len_Left < Len_Right)//右边线远长于左边线
				{
					if(Right_Side_Angle >=-48.0 && Right_Side_Angle < 0 && Right_Side_Angle!=none)
					{		
							Box_Curve_State = Left_Curve;
							return;
					}							
					else if(Right_Side_Angle != none && Right_Side_Angle <-48 )
					{
							Box_Curve_State = No_Curve;
							return;
					}
				}				
				else //左边线远长于右边线
				{

					if(Left_Side_Angle<=48.0 && Left_Side_Angle>0 && Left_Side_Angle!=none )
					{				
							Box_Curve_State = Right_Curve;
							return;
					}
					if(Left_Side_Angle != none && Left_Side_Angle >48)
					{
							Box_Curve_State = No_Curve;
							return;
					}				
				}						
			}
}




////-------------------------------------------------------------------------------------------------------------------
//// 函数简介    箱子弯道检测
//// 参数说明     void
//// 返回参数     void
//// 使用示例     Box_Curve_Check();
//// 备注信息     
////-------------------------------------------------------------------------------------------------------------------
//void Box_Curve_Check(void)
//{

//		float temp_Angle;
//	
//		//双边可检测
//		//左弯
//			if(Len_Right>Len_Left)
//			{
//				//双边可检测
//				if(Right_Side_Angle >=-44.0 && Right_Side_Angle < 0 && Right_Side_Angle!=none && Left_Side_Angle!=none)
//				{		
//						Box_Curve_State = Left_Curve;
//				}
//				//仅单边
//				//左弯
//				if(Right_Side_Angle >=-44.0 && Right_Side_Angle < 0 && Right_Side_Angle!=none && Left_Side_Angle==none)
//				{
//						Box_Curve_State = Left_Curve;	
//						return;
//				}
//				else if(Left_Side_Angle != none && Left_Side_Angle >44 && Right_Side_Angle == none)
//				{
//						Box_Curve_State = No_Curve;
//				}
//				else if(Right_Side_Angle != none && Right_Side_Angle <-44 && Left_Side_Angle == none)
//				{
//						Box_Curve_State = No_Curve;
//				}
//				else if(Right_Side_Angle != none && Right_Side_Angle <-44 && Left_Side_Angle != none)
//				{
//						Box_Curve_State = No_Curve;
//				}

//			}
//			else if(Len_Left>=Len_Right)
//			{
//					//双边可检测
//				  if(Left_Side_Angle<=44.0 && Left_Side_Angle>0 && Left_Side_Angle!=none && Right_Side_Angle!=none)
//					{				
//							Box_Curve_State = Right_Curve;
//					}
//					//仅单边
//					else if(Left_Side_Angle<=44.0 && Left_Side_Angle>0 && Left_Side_Angle!=none && Right_Side_Angle==none)
//					{
//							Box_Curve_State = Right_Curve;	
//							return;
//					}		
//					else if(Left_Side_Angle != none && Left_Side_Angle >44 && Right_Side_Angle == none)
//					{
//							Box_Curve_State = No_Curve;
//					}
//					else if(Right_Side_Angle != none && Right_Side_Angle <-44 && Left_Side_Angle == none)
//					{
//							Box_Curve_State = No_Curve;
//					}

//				
//			}
//			if(Left_Side_Angle == none && Right_Side_Angle == none)
//			{
//					Box_Curve_State = No_Curve;
//					return;
//			}
//			return ;
//}

//void Curve_Err()
//{
//	float temp_Angle;

//	if(Start_Curve_Flag)
//	{
//		switch(Box_Curve_State)
//		{
//				case No_Curve:
////							if(Left_Side_Angle != none && Right_Side_Angle != none)
////							{
////									Angle_Erro = (69.6-Left_Road_Angle) + (-66.6-Right_Road_Angle);
////							}
////							else if(Left_Side_Angle == none && Right_Side_Angle == none)
////							{
//								Angle_Erro = 0;	
////							}
////							//直道仅左线
////							else if(Left_Side_Angle!=none && Right_Side_Angle==none)
////							{
////									Angle_Erro = (66.6-Left_Road_Angle) ;
////							}
////							else if(Right_Side_Angle!=none && Left_Side_Angle==none)
////							{
////									Angle_Erro = (-70-Right_Road_Angle) ;
////							}
//				break;
//				
//				case Left_Curve:
//					
//								Angle_Erro = (-70.5-Right_Road_Angle) ;	
//				
//				break;

//				case Right_Curve:
//							//右弯
//								Angle_Erro = (52.92-Left_Road_Angle);

//				break;
//		}
//		
//	}

//}

void Curve_Err()
{
	float temp_Angle;

	if(Start_Curve_Flag)
	{
		switch(Box_Curve_State)
		{
				case No_Curve:
					Angle_Erro = 0;
				break;
				
				case Left_Curve:
					//左弯
					Angle_Erro = (White_Pnum-Black_Pnum);//应逆时针转
				break;

				case Right_Curve:
					//右弯
					Angle_Erro = (Black_Pnum-White_Pnum);//应顺时针转
				break;
		}		
	}
//	if(last_Black_num>Black_Pnum){Angle_Erro = 0;}
//	else if(abs(Rec_Now_Angle - Start_Angle)>60)
//	{
//		
//	}
	last_White_num = White_Pnum;
	last_Black_num = Black_Pnum;

	
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介    箱子矫正
// 参数说明     void
// 返回参数     void
// 使用示例     Box_Curve_aim();
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------

//bool Box_Curve_aim(void)
//{
//	switch(Start_Curve)
//	{
//		case Left_Curve://逆	
//		{			
//			if(fabs(Angle_Erro)>6)
//			{
//				X_Y_Z_Speed_Set(0,0,Angle_Erro - 50);
//				return 0;
//			}
//		}
//		break;
//		
//		case Right_Curve://顺
//		{	
//			if(fabs(Angle_Erro)>6)
//			{
//				X_Y_Z_Speed_Set(0,0,Angle_Erro + 50);
//				return 0;				
//			}
//		}
//		break;

//		case No_Curve:
//		{	
//			if(fabs(Angle_Erro)>4.5)
//			{
//				X_Y_Z_Speed_Set(0,0,5 * Angle_Erro);
//				return 0;
//			}
//		}
//		break;
//	}
//	if(Start_Curve == 0 && Bufcnt1(fabs(Angle_Erro)<=4.5,800))
//	{
//		X_Y_Z_Speed_Set(0,0,0);
//		return 1;
//	}
//	else if((Start_Curve ==1 || Start_Curve ==2) && Bufcnt1(fabs(Angle_Erro)<=6,800))
//	{
//		X_Y_Z_Speed_Set(0,0,0);
//		return 1;
//	}

//	return 0;
//}


bool Box_Curve_aim(void)
{
	switch(Start_Curve)
	{
		case Left_Curve://逆	
		{			
					//左弯
					if(fabs(Angle_Erro)>2.5)	
					{
					X_Y_Z_Speed_Set(0,0,5*Angle_Erro-50);
					return 0;
					}
		}
		break;
		
		case Right_Curve://顺
		{	
			//右弯
					if(fabs(Angle_Erro)>2.5)
					{						
						X_Y_Z_Speed_Set(0,0,5*Angle_Erro+50);
							return 0;
					}
		}
		break;

		case No_Curve:
		{	
				if(fabs(Angle_Erro)>4.5)
				{
					X_Y_Z_Speed_Set(0,0,5 * Angle_Erro);
					return 0;
				}	
			}		
		break;
	}			
		if(Start_Curve == 0 && Bufcnt1(fabs(Angle_Erro)<=4.5,800))
		{
			X_Y_Z_Speed_Set(0,0,0);
			Enable_Navigation();
			return 1;
		}
		else if((Start_Curve ==1 || Start_Curve ==2) && Bufcnt1(fabs(Angle_Erro)<=2.5,800))
		{
			X_Y_Z_Speed_Set(0,0,0);
			Enable_Navigation();
			return 1;
		}
//		else if((Start_Curve ==1 || Start_Curve ==2) && (fabs(Angle_Erro)<=0.5))
//		{
//			X_Y_Z_Speed_Set(0,0,0);
//			return 1;
//		}


	return 0;
}


//bool Box_Curve_aim(void)
//{
//	switch(Start_Curve)
//	{
//		case Left_Curve://逆	
//		{			
//			if(fabs(Angle_Erro)>6)
//			{
//				X_Y_Z_Speed_Set(0,0,Angle_Erro - 50);
//				return 0;
//			}
//		}
//		break;
//		
//		case Right_Curve://顺
//		{	
//			if(fabs(Angle_Erro)>6)
//			{
//				X_Y_Z_Speed_Set(0,0,Angle_Erro + 50);
//				return 0;				
//			}
//		}
//		break;

//		case No_Curve:
//		{	
//			if(fabs(Angle_Erro)>4.5)
//			{
//				X_Y_Z_Speed_Set(0,0,5 * Angle_Erro);
//				return 0;
//			}
//		}
//		break;
//	}
//	if(Start_Curve == 0 && Bufcnt1(fabs(Angle_Erro)<=4.5,800))
//	{
//		X_Y_Z_Speed_Set(0,0,0);
//		return 1;
//	}
//	else if((Start_Curve ==1 || Start_Curve ==2) && Bufcnt1(fabs(Angle_Erro)<=6,800))
//	{
//		X_Y_Z_Speed_Set(0,0,0);
//		return 1;
//	}

//	return 0;
//}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介    	让车转相应角度（有偏差）
// 参数说明     void
// 返回参数     void
// 使用示例     Turn_Angle(90);正顺时针，负逆时针
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
bool Turn_Angle(float Target_Angle)
{   
    Turn.Current_Angle = Gyro_YawAngle_Get();//获取当前角度
    float Offset_Erro = 0;
    float Yaw_Erro = 0;

    if(Turn.Angle_Last == 0)
    {
        Turn.Angle_Last = Turn.Current_Angle;
    }

    Turn.Offset += Turn.Current_Angle - Turn.Angle_Last;
    Turn.Angle_Last = Turn.Current_Angle;
    Offset_Erro = (Target_Angle - Turn.Offset);
    
    if(Target_Angle - Turn.Offset >= 2.5f)
    {
        Car.Speed_X = 0;
        Car.Speed_Y = 0;
        Car.Speed_Z = 250+Offset_Erro;
    }
    else if(Target_Angle - Turn.Offset <= -2.5f)
    {
        Car.Speed_X = 0;
        Car.Speed_Y = 0;
        Car.Speed_Z = -250+Offset_Erro;
    }
    else
    {
        Car.Speed_X = 0;
        Car.Speed_Y = 0;
        Car.Speed_Z = 0;
    }
	
		if(Bufcnt2(fabs(Target_Angle - Turn.Offset) <=2.5f,600))
		{
        Turn.Offset = 0;
        Turn.Angle_Last = 0;
				return 1;
		}
		return 0;
}




//-------------------------------------------------------------------------------------------------------------------
// 函数简介    箱子状态运动
// 参数说明     void
// 返回参数     void
// 使用示例     Box_Run();
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------

void Box_Run(void)
{
	 static uint16 Wait_Time = 0;//等待的时间

	 switch(Box_State)
	 {
		 case No_box:
//				Line_Angle();	
//				Box_Curve_Check();	
//	  		X_Y_Z_Speed_Set(150,180,0);//调试用		
		 
		 break;
		 		
		 case Rec_Angle:
			 
		 Line_Angle();
		 Box_Curve_Check();		 
		 if(Start_Curve == -1 && Box_Curve_State != -1 && Bufcnt1(Start_Curve == -1,1000)) 
		 {
			 Start_Curve_Flag = 1;
			 Start_Curve = Box_Curve_State;
		 }
		 
		 if(Start_Curve_Flag)
		 {
//				Curve_Err();
				if(Curve_Rec())
					{
							Box_Road_Err	=	Box.X-85;
							Box_State = Find_box;
					}
				//				if(Box_Curve_aim())	 {Box_Road_Err	=	Box.X-80;Box_State = Find_box;}
		 }
		 break;
		 
		case Find_box:
//			 			if(box_Rec())	Box_State = Finish_box; 		 		 

				if(Navigation_Process_Box())Box_State = Turn_box;
		
		break;
				
		 case	Turn_box:		 
			 
			if(Turn_Angle(170))	Box_State = Box_scan; 
		 
		 break;
		 
		 case	Box_scan:		 
			 
			if(box_Rec())	Box_State = Back_Turn; 		 		 
//			if(box_Rec())	Box_State = Finish_box; 		 		 
		 
		 break;
 
		 case Back_Turn:				
			
			 if(Turn_Angle(-170))Box_State = Turn_aim;
		 
		 break;
		 
			case Turn_aim:
				
			 if (Turn_box_Action())Box_State = Pull_box;		
			
			break;

//		 case Pull_box:
//			 
//					if(Pull_box_Action()){Box_State = Back_aim;Box_Road_Err=0;}
//		 
//		 break;
//				
//		 case Back_aim:
////				Back_Start_Angle();
//				if(Back_Start_Angle1())
//				{
//					Start_Angle=Gyro_Angle.YawAngle;
//					Box_State = Back_road;
//				}
//				
////			Back_aim_Action();
//	 break;

//		 
//		 case Back_road:
//			 
//					if(Back_Road_Move())Box_State = Finish_box;
//;

////					Car.Speed_X = 0;	Car.Speed_Y = -300; 	Car.Speed_Z = 0;
////						X_Y_Z_Speed_Set(300,180,0);
////		 
////					if(Bufcnt1(true,700) && Stop_flag==0)
////					{
////							Car.Speed_X = 0;	Car.Speed_Y = 0; 	Car.Speed_Z = 0;
////							finish_flag=1;
////							Box_State = Finish_box;//Back_aim;
////					}
//		
//     break;

	 case Pull_box:
		 
				if(Pull_box_Action()){Box_State = Back_road;}
	 
	 break;

	 case Back_road:
//				Back_Start_Angle();
				if(Back_Road()){Box_State = Back_aim;Box_Road_Err=0;}
								
//			Back_aim_Action();
	 break;

		 
		 case Back_aim:
			 
					if(Back_Start_Angle1())Box_State = Finish_box;
		
     break;			
		 
		 case Finish_box:
			 
        //清理标志位
		 
        Work_State = Origin_State;
        Box_State = No_box;
				Box_Index++;
				Start_Curve_Flag = 0;
				Start_Curve = -1;
				Box_Curve_State = -1;
				Box_Index = Box_Index%24;		 
				Box.H = 0;
				Box.W = 0;			
			
  
		 break;
		 
		
	 }
	

}


/**@brief   设置车三个方向的速度
-- @param   float Speed_X X轴速度，既横向速度
-- @param   float Speed_Y Y轴速度，既前向速度
-- @param   float Speed_Z Z轴速度，既旋转速度，正的为顺时针旋转
-- @author  陈锦辉
-- @date    2025/5/25
**/
void Box_Car_Speed(int16 Speed_X,int16 Speed_Y,int16 Speed_Z)
{
	
				FLSetSpeed = -(Speed_Y + Speed_X + Speed_Z);
				FRSetSpeed = (Speed_Y - Speed_X - Speed_Z);
				BSetSpeed = 2*(Speed_X - 0.5*Speed_Z);	
}


/**@brief   设置车三个方向的速度
-- @param   float Speed_X X轴速度，既横向速度
-- @param   float Speed_Y Y轴速度，既前向速度
-- @param   float Speed_Z Z轴速度，既旋转速度，正的为顺时针旋转
-- @author  陈锦辉
-- @date    2025/5/25
**/
void X_Y_Z_Speed_Set(int16 Speed,int16 Angle,int16 Yaw)
{
			float radians = Angle * (PAI / 180);

       Car.Speed_X=Speed*(sin(radians));
	     Car.Speed_Y=Speed*(cos(radians));
	     Car.Speed_Z=Yaw;

}

/**
 ******************************************************************************
 *  @defgroup 内部调用
 *  @brief
 *
**/

/**@brief   惯性导航使能
-- @param   无
-- @author  陈锦辉
-- @date    2023/12/12
**/
static void Enable_Navigation()
{
    if(Navigation.Start_Flag == false)
    {
        Navigation.Start_Angle = Gyro_YawAngle_Get();//当前角度 
        Navigation.Start_Flag = true;//开启惯性导航
        Navigation.Finish_Flag = false;
			
    }
}

/**
 ******************************************************************************
 *  @defgroup 外部调用
 *  @brief
 *
**/

/**@brief   惯性导航重置
-- @param   无
-- @author  陈锦辉
-- @date    2024/4/3
**/
void Reset_Navigation()
{
    if(Navigation.Start_Flag == true)
    {
        Navigation.Start_Angle = 0;//当前角度
        Navigation.Cur_Position_X = 0;
        Navigation.Cur_Position_Y = 0;
        Navigation.Start_Flag = false;//关闭惯性导航
        Navigation_state = Start_State;//状态清零
        Navigation.X_Finish = false;
        Navigation.Y_Finish = false;
    }
}

/**@brief   角度控制
-- @param   需要维持的角度
-- @author  庄文标
-- @return  PID值
-- @date    2024/3/31
**/
float Angle_Control(float Start_Angle)
{
    float Yaw_Err = 0.0f;
    Yaw_Err = Start_Angle - Gyro_YawAngle_Get();
    if(fabs(Yaw_Err)>2)    return Yaw_Err;
		else 									 return 0;

}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取图像左边线
// 参数说明     无
// 返回参数     void
// 使用示例     GetLeftSide();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Box_GetLeft(void)
{
    uint8_t L_lenth = 0, find, pin;
    int row, col;
    int colmin, colmax;

    //找不到左边线起始点
    if(Left_line.Row[0] == none)
    {
        return;
    }
    Left_Start_flag = 1;
    pin = 1;

    for(row = Left_line.Row[0] - pin; pin < MyImage_H ; pin++)
    {
        find = 0;
        row = Left_line.Row[0] - pin;

        //图像基本扫描完成,停止扫描边线
        if(row <= 1) {break;}

        //范围选定
        colmin = Left_line.Col[pin - 1] - 5;
        colmax = Left_line.Col[pin - 1] + 25;

        if(colmin <= 3){colmin = 3;};
        if(colmax >= MyImage_W - 3){colmax = MyImage_W - 3;};

       for(col =colmin ; col <= colmax; col++)
        {
					if(MyImage[row][col-2] == black_point && MyImage[row][col-1] == black_point)
					{
            if(MyImage[row][col] == white_point)
            {              
                    Left_line.Row[pin] = row;
                    Left_line.Col[pin] = col - 1;
                    find = 1;
                    L_lenth++;
							      #if Image_color //给图像上色
                    MyImage[row][col - 1] = red_point;
                    MyImage[row][col - 2] = red_point;
										#endif
                    break;
                
            }
					}	
        }

        if(find == 0)
        {
            break;
        }
    }

    //左边线长度太短,左边线起始点可能有误
    if(L_lenth <= 40)
    {
        Left_Start_flag = 0;
        Left_line.Row[0] = none;
        Left_line.Col[0] = none;
    }

}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取图像右边线
// 参数说明     无
// 返回参数     void
// 使用示例     GetRightSide();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Box_GetRight(void)
{
    uint8_t R_lenth = 0, pin, find;
    int row, col;
    int colmin, colmax;

    //找不到右边线起始点
    if(Right_line.Row[0] == none)
    {
        return;
    }
    Right_Start_flag = 1;
    pin = 1;

    for(row = Right_line.Row[0] - pin; pin < MyImage_H ; pin++)
    {
        find = 0;
        row = Right_line.Row[0] - pin;

        //图像基本扫描完成,停止扫描边线
        if(row <= 1) {break;}

        //范围选定
        colmin = Right_line.Col[pin - 1] - 25;
        colmax = Right_line.Col[pin - 1] + 5;


        if(colmin <= 3){colmin = 3;};
        if(colmax >= MyImage_W - 3){colmax = MyImage_W - 3;};

        for(col = colmax; col >=colmin ; col--)
        {
            if(MyImage[row][col+2] == black_point && MyImage[row][col+1] == black_point)
						{
                if(MyImage[row][col] == white_point )
                {
                    Right_line.Row[pin] = row;
                    Right_line.Col[pin] = col + 1;
                    find = 1;
                    R_lenth++;
							      #if Image_color //给图像上色
                    MyImage[row][col + 1] = red_point;
                    MyImage[row][col + 2] = red_point;
										#endif
                    break;
                }
						}
        }

        if(find == 0)
        {
            break;
        }
    }

    //右边线长度太短,右边线起始点可能有误
    if(R_lenth <= 40)
    {
        Right_Start_flag = 0;
        Right_line.Row[0] = none;
        Right_line.Col[0] = none;
    }

}


////-------------------------------------------------------------------------------------------------------------------
//// 函数简介     获取图像左边线
//// 参数说明     无
//// 返回参数     void
//// 使用示例     GetLeftSide();
//// 备注信息
////-------------------------------------------------------------------------------------------------------------------
//void Box_GetLeft(void)
//{
//		uint8_t Len = 0;
//    uint8_t pin;
//    int row, col;
//	  int col_new = 0;
//    Left_Start_flag = 1;
//    pin = 0;

//    for(row =  20 + pin; pin < MyImage_H-2 ; pin++)
//    {        
//        row = 20 + pin;
//        //图像基本扫描完成,停止扫描边线
//			if(row >= MyImage_H-2) {break;}
//			
//      for(col = 3 ; col <= MyImage_W-3; col++)
//			{
//				if(MyImage[row][col-2] == black_point && MyImage[row][col-1] == black_point)
//				{
//						if(MyImage[row][col] == white_point)
//						{
//								if(col_new == 0)
//								{
//										Left_line.Row[Len] = row;
//										Left_line.Col[Len] = col - 1;									
//										col_new = Left_line.Col[Len];
////										#if Image_color //给图像上色
////										MyImage[row][col - 1] = red_point;
////										MyImage[row][col - 2] = red_point;
////										#endif
//											Len++;																							
//										break;
//								}
//								else
//								{
//									if(col_new-col>(-3))
//									{
//										Left_line.Row[Len] = row;
//										Left_line.Col[Len] = col - 1;									
//										col_new = Left_line.Col[Len];
//										#if Image_color //给图像上色
//										MyImage[row][col - 1] = red_point;
//										MyImage[row][col - 2] = red_point;
//										#endif
//										Len++;																							
//										break;
//									}
//									else 
//										break;

//								}
//						}
//				}	
//			}
//				
//    }
//		Len_Left = Len;	

//}


////-------------------------------------------------------------------------------------------------------------------
//// 函数简介     获取图像右边线
//// 参数说明     无
//// 返回参数     void
//// 使用示例     GetRightSide();
//// 备注信息
////-------------------------------------------------------------------------------------------------------------------
//void Box_GetRight(void)
//{
//  	uint8_t Len=0;
//		uint8_t pin;
//    int row, col;
//	  int col_new = 0;
//    Right_Start_flag = 1;
//    pin = 0;

//    for(row =  20 + pin; pin < MyImage_H-2 ; pin++)
//    {        
//        row = 20 + pin;
//        //图像基本扫描完成,停止扫描边线
//			if(row >= MyImage_H-2) {break;}

//       for(col = MyImage_W-3  ; col >= 3; col--)
//        {
//            if(MyImage[row][col+2] == black_point && MyImage[row][col+1] == black_point)
//						{
//                if(MyImage[row][col] == white_point )
//                {
//									if(col_new == 0)
//									{
//										Right_line.Row[Len] = row;
//										Right_line.Col[Len] = col - 1;									
//										col_new = Right_line.Col[Len];
//										Len++;																							
//										break;
//									}
//									else
//									{
//											if(col_new-col<3 )
//											{									
//												Right_line.Row[Len] = row;
//												Right_line.Col[Len] = col - 1;									
//												col_new = Right_line.Col[Len];
//												printf("%d,%d\r\n",Right_line.Col[Len],Right_line.Row[Len]);												
//						
//												#if Image_color //给图像上色
//												MyImage[row][col + 1] = red_point;
//												MyImage[row][col + 2] = red_point;
//												#endif
//												Len++;
//												break;
//											}
//											else
//												break;
//									}														
//                }
//						}
//        }
//    }
//		Len_Right = Len;

//}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     左边线顶点
// 参数说明     无
// 返回参数     void
// 使用示例     LeftSide_Peak();
// 备注信息     严格上来讲是寻找左边线列最大的点
//-------------------------------------------------------------------------------------------------------------------
void Box_Left_Peak(void)
{

    int temp, pin, col_min = MyImage_W;

    //如果不存在左边线起始点
    if(Left_line.Row[0] == none)
    {
        return;
    }

		 for(pin = 0; pin < MyImage_H + 20; pin++)
    {
				//若该点无数据
        if(Left_line.Row[pin] == none)
        {
            break;
        }

        //打擂台,寻找右边线列最小处点
        if(col_min >= Left_line.Col[pin] )
        {
            col_min = Left_line.Col[pin];
            temp = pin;
        }
    }
		
    Left_peak.Row = Left_line.Row[temp];
    Left_peak.Col = Left_line.Col[temp];
    Left_peak.pin = temp;
		
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     右边线顶点
// 参数说明     无
// 返回参数     void
// 使用示例     RightLine_Peak();
// 备注信息     严格上来讲是寻找右边线列最小的点
//-------------------------------------------------------------------------------------------------------------------
void Box_Right_Peak(void)
{
    int temp, pin, col_max = 0;

    //如果不存在右边线起始点
    if(Right_line.Row[0] == none)
    {
        return;
    }

		for(pin = 0; pin < MyImage_H + 20; pin++)
    {
			//若该点无数据
        if(Right_line.Row[pin] == none)
        {
            break;
        }
        //打擂台,寻找左边线列最大处点
        if(col_max <= Right_line.Col[pin] )
        {
            col_max = Right_line.Col[pin];
            temp = pin;
        }
    }

    Right_peak.Row = Right_line.Row[temp];
    Right_peak.Col = Right_line.Col[temp];
    Right_peak.pin = temp;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取边线顶点
// 参数说明     无
// 返回参数     void
// 使用示例     SideLine_Peak();
// 备注信息     严格上来讲是寻找左边线列最大的点,右边线列最小的点
//-------------------------------------------------------------------------------------------------------------------
void Box_Line_Peak(void)
{
    Box_Left_Peak();
    Box_Right_Peak();
}



void Get_LeftRight()
{
	Box_GetLeft();
	Box_GetRight();
}





// 计算二阶多项式拟合
PolyFitResult polyfit2(int *x, int *y, int n) {
    double sum_x = 0, sum_x2 = 0, sum_x3 = 0, sum_x4 = 0;
    double sum_y = 0, sum_xy = 0, sum_x2y = 0;
		int temp = n;
    // 计算各项累加和
    for (size_t i = 0; i < n ; i++) {
			if(y[i] > 25 && y[i] < 80)
			{
				temp --;
				continue;
			}
        double xi = x[i];
        double xi2 = xi * xi;
        double xi3 = xi2 * xi;
        double xi4 = xi3 * xi;

        sum_x += xi;
        sum_x2 += xi2;
        sum_x3 += xi3;
        sum_x4 += xi4;

        sum_y += y[i];
        sum_xy += xi * y[i];
        sum_x2y += xi2 * y[i];
    }

    // 构造正规方程的矩阵
    // | n      sum_x   sum_x2 | | c |   | sum_y   |
    // | sum_x  sum_x2  sum_x3 | | b | = | sum_xy  |
    // | sum_x2 sum_x3  sum_x4 | | a |   | sum_x2y |
    double A[3][3] = {
        { (double)temp, sum_x, sum_x2 },
        { sum_x, sum_x2, sum_x3 },
        { sum_x2, sum_x3, sum_x4 }
    };
    double B[3] = { sum_y, sum_xy, sum_x2y };

    // 使用高斯消元法求解线性方程组
    for (int k = 0; k < 3; k++) {
        // 部分主元消去
        int max_row = k;
        for (int i = k + 1; i < 3; i++) {
            if (fabs(A[i][k]) > fabs(A[max_row][k])) {
                max_row = i;
            }
        }

        // 交换行
        if (max_row != k) {
            for (int j = 0; j < 3; j++) {
                float tmp = A[k][j];
                A[k][j] = A[max_row][j];
                A[max_row][j] = tmp;
            }
            double tmp = B[k];
            B[k] = B[max_row];
            B[max_row] = tmp;
        }

        // 消元
        for (int i = k + 1; i < 3; i++) {
            double factor = A[i][k] / A[k][k];
            for (int j = k; j < 3; j++) {
                A[i][j] -= factor * A[k][j];
            }
            B[i] -= factor * B[k];
        }
    }

    // 回代求解
    PolyFitResult result;
    result.a = B[2] / A[2][2];
    result.b = (B[1] - A[1][2] * result.a) / A[1][1];
    result.c = (B[0] - A[0][1] * result.b - A[0][2] * result.a) / A[0][0];

    return result;
}

// 计算二次多项式 a + bx + cx2 在给定点x处的值
void polyval2(PolyFitResult *coefficients, int* x)
	{
		double y_double;
		int y;
		for(uint8_t i = 0; i < Len_Right; i++)
		{
			y_double = coefficients->a + coefficients->b*x[i]*1.0 + coefficients->c*x[i]*x[i];
			y = (int)(y_double);
//			printf("%d, %d\r\n", x[i], y);
			MyImage[y][x[i]]=red_point;
		}
    
}