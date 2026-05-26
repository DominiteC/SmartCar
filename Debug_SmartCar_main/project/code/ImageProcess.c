#include "ImageProcess.h"

uint8 MyImage[MyImage_H][MyImage_W]={};                                  //裁剪得到的图像数据(仍然是灰度)
	
int Servo_Weight[9] =
{
        1, 2, 3, 4, 5, 4, 3, 2, 1,          // 31 ~ 40
};
	
//严禁扭摄像头
//根据不同行作相应处理，画出中线
#define ref_  1.7   	
#define reduce  0.012

uint8_t Start_Flag;

float ref = ref_;

//-----------------种子生长法用---------------------
uint16 Dir_R[3*148] = { 0 };//用来存储右边生长方向
uint16 Dir_L[3*148] = { 0 };//用来存储左边生长方向

uint16 Len_Left  = 0;//统计左边找到点的个数
uint16 Len_Right = 0;//统计右边找到点的个数

float white_Per;//列
float black_Per;//列

uint8_t last_White_num = 0;
uint8_t last_Black_num = 0;

//-----------------种子生长法用---------------------

int threshold = 80;                                                    //所求阈值

uint8_t Work_State = No_Duel;                           //工作状态(是否走直弯道) 默认 No_Duel
uint8_t Cross_State = No_Cross;                         //十字路口标志位        默认 No_Cross
uint8_t Cross_DirFlag = No_Into;                        //十字进入方向标志位    默认 No_Into
uint8_t Roundabout_State = No_Round;                    //环岛标志位           默认 No_Round
uint8_t Roundabout_DirFlag = NonDir;                    //环岛方向标志位        默认 NonDir
uint8_t Box_State = No_box;                      				//箱子标志位           默认 No_barrier
uint8_t Origin_State = 0;
	
	
//定义膨胀和腐蚀的阈值区间
#define Threshold_Max   255*5//此参数可根据自己的需求调节
#define Threshold_Min   255*2//此参数可根据自己的需求调节

uint16 Black_Pnum;//统计黑点的个数
uint16 White_Pnum;//统计白点的个数	

struct SIDELINE Left_line;                                                  //赛道左边线
struct SIDELINE Right_line;                                                 //赛道右边线
	
struct PEAK Left_peak;                                              		   	//左边线顶点
struct PEAK Right_peak;                                                			//右边线顶点
	
//边线跳变点
struct Jump Left_jump;
struct Jump Right_jump;
	
//丢失线顶点(也就是丢失边线后,继续向上寻找，直至再次寻找到赛道边缘的点)
struct LOSS Left_loss;
struct LOSS Right_loss;
	
//边线起始点标志位 0:丢失起始点  1:存在起始点   (默认丢失)
uint8_t Left_Start_flag = 0;                                            //左边线起始点标记
uint8_t Right_Start_flag = 0;                                           //右边线起始点标记

uint8_t Leftline_Jump_flag = 0;																					//左线跳变标志位
uint8_t Rightline_Jump_flag = 0 ;																					//右线跳变标志位


int Center_Line_Col[5*MyImage_H];                                         //赛道中线列数据
int Center_Line_Row[5*MyImage_H];                                         //赛道中线行数据

struct CROSSPEAK LCrossPeak;                                            //十字路口顶点数据
struct CROSSPEAK RCrossPeak;                                            //十字路口顶点数据

struct BOX_PLACE Box;            																				//箱子数据

struct PEAK Round_peak;

int Round_peak_Row;                                                     //环岛顶点数据
int Round_peak_Col;                                                     //环岛顶点数据

//边线跳变标志位  0:边线未跳变  1:边线跳变   (默认不跳变)
uint8_t LeftlineJump_flag = 0;
uint8_t RightlineJump_flag = 0;

//图像中的赛道绝对宽度,为Control.c中的Race_Width数组服务;
uint8_t GetRaceWidth[MyImage_H];

//特殊速度设置标志位 0:标志位无效 1:标志位有效
uint8_t SpecialSpeed_flag = 0;
int SpecialSpeed;

//停车标志位,该标志位不会自动清除 0:标志位无效 1:标志位有效  (默认停车)
uint8_t Stop_flag = 0;

//斑马线标志位(停车标志位)     0:斑马线未发现    1:斑马线发现
uint8_t Zebra_flag = 0;

//贯通列 贯通行 停车行 左半贯通行 右半贯通行
float Cross_Col_Per;
float Cross_Row_Per;
float Stop_Row_Per;
float Left_Row_Per;
float Right_Row_Per;
float Right_Round_Per;

float Left_Side_Tan;		
float Left_Side_Angle;	
float Right_Side_Tan;	
float Right_Side_Angle;

float Left_Road_Tan;		
float Left_Road_Angle;	
float Right_Road_Tan;		
float Right_Road_Angle;	

//左右边线方差
float L_S;
float R_S;



//==============================================图像二值化处理部分==========================================================

////-------------------------------------------------------------------------------------------------------------------
//// 函数简介     图像直接裁剪
//// 参数说明     无
//// 返回参数     void
//// 使用示例     Image_SuitCut();
//// 备注信息     将图像裁剪成MyImage_W与MyImage_H指定大小
////-------------------------------------------------------------------------------------------------------------------
//void Image_SuitCut(void)
//{
//#if (MT9V03X_W == MyImage_W && MT9V03X_H == MyImage_H)
//    memcpy(MyImage,mt9v03x_image,sizeof(mt9v03x_image));
//#else
//    uint8_t Col, Row, Col_offset, Row_offset;
//    Col_offset = (MT9V03X_W - MyImage_W) / 2;       //计算列偏移量
//    Row_offset = (MT9V03X_H - MyImage_H) / 2;       //计算行偏移量
//    for(Row = 0; Row < MyImage_H; Row++)
//    {
//        for(Col = 0; Col < MyImage_W; Col++)
//        {
//            MyImage[Row][Col] = mt9v03x_image[Row + Row_offset][Col + Col_offset];
//        }
//    }
//#endif
//}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     原始灰度图像压缩处理
// 参数说明     无
// 返回参数     void
// 使用示例     Image_Compress();
// 备注信息     将原始尺寸的灰度图像压缩成你所需要的大小,列压缩只支持对半切,切勿使用其他比例
//-------------------------------------------------------------------------------------------------------------------
void Image_Compress(void)
{
  int i, j, row, line;
  const float div_h = MT9V03X_H / MyImage_H, div_w = MT9V03X_W / MyImage_W;         //根据原始的图像尺寸和你所需要的图像尺寸确定好压缩比例。

  for (i = 0; i < MyImage_H; i++)                            //遍历图像的每一行，从第零行到第59行。
  {
    row = i * div_h + 0.5;
    for (j = 0; j < MyImage_W; j++)                          //遍历图像的每一列，从第零列到第79列。
    {
      line = j * div_w + 0.5;
      MyImage[i][j] = mt9v03x_image[row][line];       //mt9v03x_image数组里面是原始灰度图像，Image_Use数组存储的是我之后要拿去处理的图像，但依然是灰度图像哦！只是压缩了一下而已。
    }
  }
}



void MyImage_Init(void)
{
				for(uint8 i=0;i<MyImage_W;i++)
				{
					 for(uint8 j=0;j<MyImage_H;j++)
					{
						MyImage[i][j]=0;
					}
				}

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     固定阈值二值化图像
// 参数说明     无
// 返回参数     void
// 使用示例     Bin_Threshold_Transfer();
// 备注信息     阈值Bin_Threshold在ImageProcess.c中宏定义
//-------------------------------------------------------------------------------------------------------------------
void Bin_Threshold_Transfer(void)
{
    int Col, Row;
    for(Row = 0; Row < MyImage_H; Row++)
    {
        for(Col = 0; Col < MyImage_W; Col++)
        {
            if(MyImage[Row][Col] < Bin_Threshold)       //图像亮度低于固定阈值
            {
                MyImage[Row][Col] = black_point;                  //将图像设置为黑色
            }
            else
            {
                MyImage[Row][Col] = white_point;                  //将图像设置为白色
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     大津法二值化图像
// 参数说明     无
// 返回参数     void
// 使用示例     Bin_OTSU_mode_Transfer();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Bin_OTSU_mode_Transfer(void)
{
#define GrayScale 256
		
	  int White_num = 0;
    int Black_num = 0;
    int row, col, i;
    int MinValue = 255;																	//打擂台所作数
    int MaxValue = 0;																		//打擂台所作数
    int sum = MyImage_H * MyImage_W;                    //图片像素总个数
    uint32_t gray_sum = 0;                              //每个像素灰度值的总和
    int GrayCount[256] = {0};                           //对应灰度i下的像素个数，共256个灰度值
    float GrayPercent[256] = {0};                       //对应灰度i下像素个数占总像素的百分比

		int HistGram[GrayScale];
		
    float w0 = 0, w1 = 0;                               //前景像素全图占比;后景像素全图占比
    float sigma_max = 0;                                //当前灰度下所得类间方差;最大类间方差
    float sigma_temp = 0;                               //当前灰度下所得类间方差
//    int threshold = 0;                                  //所求阈值
    static int last_threshold = 0;                      //上一次阈值
    //用于计算下一行数值的中间量
    float u0_temp = 0, u1_temp = 0;
    float u0 = 0, u1 = 0, u_delta = 0;

		
    //计算每个灰度级i的像素个数
    for(row = 0; row < MyImage_H; row++)
    {
        for(col = 0; col < MyImage_W; col++)
        {
            GrayCount[MyImage[row][col]]++;
            gray_sum += MyImage[row][col];

            //打擂台寻找像素图中的最大灰度
            if(MyImage[row][col] > MaxValue)
            {
                MaxValue = MyImage[row][col];
            }

            //打擂台寻找像素图中的最小灰度
            if(MyImage[row][col] < MinValue)
            {
                MinValue = MyImage[row][col];
            }

        }
    }

    //计算每个灰度级i下像素个数占全图的百分比
    for(i = 0; i < 256; i++)
    {
        GrayPercent[i] = GrayCount[i] * 1.0 / sum;
    }

    for(i = MinValue; i < MaxValue; i++)
    {
        //公式计算
        w0 += GrayPercent[i];
        u0_temp += i * GrayPercent[i];
        w1 = 1 - w0;
        u1_temp = gray_sum / sum - u0_temp;

        //计算公式所需参数,计算类间方差
        u0 = u0_temp / w0;
        u1 = u1_temp / w1;
        u_delta = u0 - u1;
        sigma_temp = w0 * w1 * u_delta * u_delta;

        //找寻最大类间方差,直至结尾
        if(sigma_temp > sigma_max)
        {
            sigma_max = sigma_temp;
            threshold = i;
        }

        if(sigma_temp < sigma_max)
        {
            break;
        }
    }

//    //阈值平滑过度
//    if(last_threshold - threshold > 25)
//    {
//        threshold = last_threshold - 25;
//    }
//    else if(last_threshold - threshold < -25)
//    {
//        threshold = last_threshold + 25;
//    }

//    threshold = 1.035*threshold;          //大津法阈值偏移
	    threshold = threshold+1;          //大津法阈值偏移

    last_threshold = threshold;
//		ips200_show_int(Row_23,Col_18,threshold,3);
		
    for(row = 0; row < MyImage_H; row++)
    {
        for(col = 0; col < MyImage_W; col++)
        {
            if(MyImage[row][col] < last_threshold)                     //图像亮度低于固定阈值
            {
                MyImage[row][col] = black_point;                  //将图像设置为黑色
								Black_num++;
            }
            else
            {
                MyImage[row][col] = white_point;                  //将图像设置为白色
								White_num++;
            }
        }
    }
	
		
		white_Per  = 1.0 * White_num / (White_num + Black_num);
		black_Per  = 1.0 - white_Per;
//		Black_Pnum = 100 * black_Per;
//		White_Pnum = 100 * white_Per;
		
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     自适应阈值法
// 参数说明     无
// 返回参数     void
// 使用示例     adaptiveThreshold();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------

void adaptiveThreshold(uint8_t clip_value)
{
	int block=9;
  int half_block = block / 2;
  for(int y=half_block; y<MyImage_H-half_block; y++)
	{
    for(int x=half_block; x<MyImage_W-half_block; x++)
		{
      // 计算局部阈值
      int thres = 0;
      for(int dy=-half_block; dy<=half_block; dy++)
			{
        for(int dx=-half_block; dx<=half_block; dx++)
				{
          thres += MyImage[y+dy][x+dx];
        }
      }
      thres = thres / (block * block) - clip_value;
      // 进行二值化
      MyImage[y][x] = MyImage[y][x]>thres ? white_point : black_point;
    }
  }
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     八领域差比和法
// 参数说明     无
// 返回参数     void
// 使用示例     image_CBH();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------

void image_CBH(int16 Threshold_cbh)
{
	int16 row,column;
	int8 image_cbh;
	int8 cbh_ary[3][MyImage_W*2];

//八个方向直接差比和
 for(row=MyImage_H-2;row>=1;row--)
	{
      	for(column=1;column<=MyImage_W-2;column++)
			{
				cbh_ary[2][column*2]   = 100*(MyImage[row][column] - MyImage[row+1][column])/(MyImage[row][column] + MyImage[row+1][column]);
				cbh_ary[2][column*2-1] = 100*(MyImage[row][column] - MyImage[row+1][column-1])/(MyImage[row][column] + MyImage[row+1][column-1]);
				cbh_ary[2][column*2+1] = 100*(MyImage[row][column] - MyImage[row+1][column+1])/(MyImage[row][column] + MyImage[row+1][column+1]);
				cbh_ary[1][column*2+1] = 100*(MyImage[row][column] - MyImage[row][column+1])/(MyImage[row][column] + MyImage[row][column+1]);

				cbh_ary[1][column*2-1] = 100*(MyImage[row][column] - MyImage[row][column-1])/(MyImage[row][column] + MyImage[row][column-1]);
				cbh_ary[0][column*2+1] = 100*(MyImage[row][column] - MyImage[row-1][column+1])/(MyImage[row][column] + MyImage[row-1][column+1]);
				cbh_ary[0][column*2-1] = 100*(MyImage[row][column] - MyImage[row-1][column-1])/(MyImage[row][column] + MyImage[row-1][column-1]);
				cbh_ary[0][column*2] = 100*(MyImage[row][column] - MyImage[row-1][column])/(MyImage[row][column] + MyImage[row-1][column]);

				image_cbh=(cbh_ary[0][column*2]+cbh_ary[1][column*2-1]+cbh_ary[1][column*2+1]
									+cbh_ary[0][column*2-1]+cbh_ary[0][column*2+1]+cbh_ary[2][column*2-1]
									+cbh_ary[2][column*2+1]+cbh_ary[2][column*2])/8;
				
				if(image_cbh>Threshold_cbh)
				{
					MyImage[row][column]=black_point;
				}else
				{
					MyImage[row][column]=white_point;
				}
			}
			
		}
		
		for(int i=0;i<MyImage_H;i++)
		{
			MyImage[i][0]=black_point;
			MyImage[i][MyImage_W-1]=black_point;
		}
		
		for(int j=0;j<MyImage_W;j++)
		{
			MyImage[0][j]=white_point;
			MyImage[MyImage_H-1][j]=white_point;
		}
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     Sobel二值化
// 参数说明     无
// 返回参数     void
// 使用示例     sobelAutoThreshold();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------

void sobelAutoThreshold()
{
/** 卷积核大小 */
    short KERNEL_SIZE = 3;
    short xStart = KERNEL_SIZE ;
    short xEnd = MyImage_W - KERNEL_SIZE ;
    short yStart = KERNEL_SIZE ;
    short yEnd = MyImage_H - KERNEL_SIZE ;
    short row, col, k;
    short temp[4];
    for (row = yStart; row <= yEnd; row+=2)
    {
        for (col = xStart; col <= xEnd; col+=2)
        {
            /* 计算不同方向梯度幅值  */
            temp[0] = - (short) MyImage[row - 1] [col - 1] + (short) MyImage[row - 1][col + 1]	    // {-1, 0, 1},
											- (short) MyImage[row]     [col - 1] + (short) MyImage[row]    [col + 1]			// {-1, 0, 1},
											- (short) MyImage[row + 1][col - 1] + (short) MyImage[row + 1][col + 1];    	// {-1, 0, 1};

            temp[1] = - (short) MyImage[row - 1][col - 1] + (short) MyImage[row + 1][col - 1]     	// {-1, -1, -1},
											- (short) MyImage[row - 1][col]     + (short) MyImage[row + 1][col]       		// { 0,  0,  0},
											- (short) MyImage[row - 1][col + 1] + (short) MyImage[row + 1][col + 1];			// { 1,  1,  1};
	
            temp[2] = - (short) MyImage[row - 1][col]     + (short) MyImage[row]    [col - 1]				//  {0, -1, -1},
											- (short) MyImage[row]    [col + 1] + (short) MyImage[row + 1][col]       		//  {1,  0, -1},
											- (short) MyImage[row - 1][col + 1] + (short) MyImage[row + 1][col - 1];    	//  {1,  1,  0};
            
            temp[3] = - (short) MyImage[row - 1][col] +     (short) MyImage[row]    [col + 1]       // {-1, -1,  0},
											- (short) MyImage[row]    [col - 1] + (short) MyImage[row + 1][col]       		// {-1,  0,  1},
											- (short) MyImage[row - 1][col - 1] + (short) MyImage[row + 1][col + 1];    	//  {0,  1,  1};

            temp[0] = abs(temp[0]);
            temp[1] = abs(temp[1]);
            temp[2] = abs(temp[2]);
            temp[3] = abs(temp[3]);

            /* 找出梯度幅值最大值  */
            for (k = 1; k < 3; k++)
            {
                if (temp[0] < temp[k])
                {
                    temp[0] = temp[k];
                }
            }
						


            /* 使用像素点邻域内像素点之和的一定比例    作为阈值  */
            temp[3] =
											(short) MyImage[row - 1][col - 1] + (short) MyImage[row - 1][col] + (short) MyImage[row - 1][col + 1]
                    + (short) MyImage[row]    [col - 1] + (short) MyImage[row]    [col] + (short) MyImage[row]    [col + 1]
                    + (short) MyImage[row + 1][col - 1] + (short) MyImage[row + 1][col] + (short) MyImage[row + 1][col + 1];

            if (temp[0] > (temp[3] / 12.0f))
            {
                MyImage[row][col] = white_point;
            }
            else
            {
                MyImage[row][col] = black_point;
            }
        }
    }

}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     图像处理
// 参数说明     transfer_mode   transfer_mode_enum在ImageProcess.h中宏定义
// 返回参数     void
// 使用示例     Image_Transfer(Bin_OTSU_mode);
// 备注信息     可以选择不同的图像处理方式
//-------------------------------------------------------------------------------------------------------------------
void Image_Transfer(transfer_mode_enum transfer_mode)
{
    int row;
		int col;
    switch(transfer_mode)
    {
        case Bin_Threshold_mode:
        {
            Bin_Threshold_Transfer();
        }break;

        case Bin_OTSU_mode:
        {
            Bin_OTSU_mode_Transfer();
        }break;
				case Bin_adaptive_mode:
				{
					adaptiveThreshold(40);
				}
				case sobel_mode:
				{
					sobelAutoThreshold ();
				}
        default:
        {
            //图像默认为灰度图像,无需处理.
        }break;
    }

    //将图像上下左右边缘修改成蓝色,这样可以减少边线搜索时的误判
    for(row = 0; row < MyImage_H; row++)
    {
        MyImage[row][0] = blue_point;
   			MyImage[row][1] = blue_point;
        MyImage[row][MyImage_W - 1] = blue_point;
        MyImage[row][MyImage_W - 2] = blue_point;
    }
		for(col = 0; col < MyImage_W; col++)
		{
			MyImage[0][col] = blue_point;
			MyImage[1][col] = blue_point;
			MyImage[MyImage_H - 1][col] = blue_point;
			MyImage[MyImage_H - 2][col] = blue_point;
		}
		
//		    //将图像上下左右边缘修改成蓝色,这样可以减少边线搜索时的误判
//    for(row = 0; row < MyImage_H; row++)
//    {
//        MyImage[row][0] = black_point;
//   			MyImage[row][1] = black_point;
//        MyImage[row][MyImage_W - 1] = black_point;
//        MyImage[row][MyImage_W - 2] = black_point;
//    }
//		for(col = 0; col < MyImage_W; col++)
//		{
//			MyImage[0][col] = black_point;
//			MyImage[1][col] = black_point;
//			MyImage[MyImage_H - 1][col] = black_point;
//			MyImage[MyImage_H - 2][col] = black_point;
//		}


}


////-------------------------------------------------------------------------------------------------------------------
//// 函数简介     二值化图像滤波
//// 参数说明     无
//// 返回参数     void
//// 使用示例     Image_BinFilter();
//// 备注信息     三黑点三白点感染滤波(?)
////-------------------------------------------------------------------------------------------------------------------
//void Image_BinFilter(void)
//{
//    int Col, Row;
//    for(Row = 1; Row < MyImage_H - 1; Row++)
//    {
//        for(Col = 1; Col < MyImage_W - 1; Col++)
//        {
//            if(MyImage[Row][Col] == black_point && (                    //如果图像中存在一个黑点
//              (MyImage[Row - 1][Col] + MyImage[Row + 1][Col] +
//               MyImage[Row][Col - 1] + MyImage[Row][Col + 1]) > 255*3))     //黑点四周存在三个白点
//            {
//               MyImage[Row][Col] = white_point;                         //把黑点变成白点
//            }

//            if(MyImage[Row][Col] == white_point && (                    //如果图像中存在一个白点
//              (MyImage[Row - 1][Col] + MyImage[Row + 1][Col] +
//               MyImage[Row][Col - 1] + MyImage[Row][Col + 1]) < 1))     //白点四周存在三个黑点
//            {
//               MyImage[Row][Col] = black_point;                         //把白点变成黑点
//            }

//        }
//    }

//}

/**
//函数简介   形态学滤波
// 参数说明     无
// 返回参数     void
// 使用示例     Image_Filter();
**/
void Image_Filter()//形态学滤波，简单来说就是膨胀和腐蚀的思想
{
    uint16 i, j;
    uint32 num = 0;


    for (i = 2; i < MyImage_H - 2; i++)
    {
        for (j = 2; j < (MyImage_W - 2); j++)
        {
            //统计八个方向的像素值
            num =
                MyImage[i - 1][j - 1] + MyImage[i - 1][j] + MyImage[i - 1][j + 1]
                + MyImage[i][j - 1] + MyImage[i][j + 1]
                + MyImage[i + 1][j - 1] + MyImage[i + 1][j] + MyImage[i + 1][j + 1];


            if (num >= Threshold_Max && MyImage[i][j] == 0)
            {

                MyImage[i][j] = 255;//白  可以搞成宏定义，方便更改

            }
            if (num <= Threshold_Min && MyImage[i][j] == 255)
            {

                MyImage[i][j] = 0;//黑

            }

        }
    }

}


//==============================================图像二值化处理部分==========================================================



//================================================处理元素数据获取============================================================
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取图像中赛道绝对宽度
// 参数说明     无
// 返回参数     void
// 使用示例     GetRace_Width();
// 备注信息     获取的数值将存储在GetRaceWidth中,通过USART0串口打印
//-------------------------------------------------------------------------------------------------------------------
void GetRace_Width(void)
{
    uint8_t i;
    uint8_t j;

    for(i = 0; i < MyImage_H; i++)
    {
        if(Right_line.Col[i] != none && Left_line.Col[i] != none)
        {
            GetRaceWidth[i] = Right_line.Col[i] - Left_line.Col[i];
            printf("%d\n", GetRaceWidth[i]);
        }

        j++;

        if(j >= 10)
        {
            j = 0;
            printf("\r\n");
        }

    }

//    uart_write_buffer(UART_0, GetRaceWidth, MyImage_H);

}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     贯通列
// 参数说明     无
// 返回参数     void
// 使用示例     Cross_Col();
// 备注信息     用于冲出赛道保护
//-------------------------------------------------------------------------------------------------------------------
float Cross_Col(void)
{
    float White_percent;
    int i, row;
    int White_num = 0;
    int Black_num = 0;
    uint16_t point = 5 * MyImage_H;

    //自行选择判断的列
    int col[5] = {Race_Center - 3, Race_Center - 2, Race_Center, Race_Center + 2, Race_Center + 3};

    for(i = 0; i < 5; i++)
    {
        for(row = MyImage_H - 2; row >= 5; row--)
        {
            if(MyImage[row][col[i]] == white_point)
            {
                White_num++;
            }
        }
    }
    White_percent = 1.0 * White_num / point;

    //限幅,贯通列需要用于小车速度权重计算
    if(White_percent > 1){White_percent = 1;}

    return White_percent;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     贯通列
// 参数说明     无
// 返回参数     void
// 使用示例     Cross_Col();
// 备注信息     用于环岛
//-------------------------------------------------------------------------------------------------------------------
float Round_Row(void)
{
    float White_percent;
    int i,j;
		int RowCenter = 40;
		int ColC = 125;
    int White_num = 0;
    int Black_num = 0;
    uint16_t point = 5 * 9;

    //自行选择判断的行
    int row[9] = {RowCenter - 5,RowCenter - 4,RowCenter - 3, RowCenter - 2, RowCenter, RowCenter + 2, RowCenter + 3,RowCenter +4,RowCenter + 5};
		int col[5] = {ColC-3,ColC-2,ColC,ColC+2,ColC+3};
    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 5; j++)
        {
            if(MyImage[row[i]][col[j]] == white_point)
            {
                White_num++;
            }
            else
            {
                Black_num++;
                if(Black_num > 40)
                {
                    break;
                }
            }
        }
    }

    White_percent = 1.0 * White_num / point;

    return White_percent;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     贯通行
// 参数说明     RowCenter   需要判断的贯通行中心行
// 返回参数     void
// 使用示例     Cross_Row();
// 备注信息     用于十字路口判断
//-------------------------------------------------------------------------------------------------------------------
float Cross_Row(uint8_t RowCenter)
{
    float White_percent;
    int i, col;
    int White_num = 0;
    int Black_num = 0;
    uint16_t point = 5 * MyImage_W;

    //自行选择判断的行
    int row[5] = {RowCenter - 3, RowCenter - 2, RowCenter, RowCenter + 2, RowCenter + 3};

    for(i = 0; i < 5; i++)
    {
        for(col = 0; col < MyImage_W - 1; col++)
        {
            if(MyImage[row[i]][col] == white_point)
            {
                White_num++;
            }
            else
            {
                Black_num++;
                if(Black_num > 40)
                {
                    break;
                }
            }
        }
    }

    White_percent = 1.0 * White_num / point;

    return White_percent;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     贯通左半行
// 参数说明     无
// 返回参数     void
// 使用示例     LCross_Row();
// 备注信息     用于十字路口出口判断及环岛判断
//-------------------------------------------------------------------------------------------------------------------
float LCross_Row(void)
{
    float White_percent;
    int i, col;
    int White_num = 0;
    int Black_num = 0;
    uint16_t point = 5 * (Race_Center - 2);

    //自行选择判断的行
    int row[5] = {45 - 3, 45 , 45 + 3, 45 + 6, 45 + 9};

    for(i = 0; i < 5; i++)
    {
        for(col = Race_Center; col > 2; col--)
        {
            if(MyImage[row[i]][col] == white_point)
                White_num++;
            else
            {
                Black_num++;
                
            }
        }
    }
    White_percent = 1.0 * White_num / point;

    return White_percent;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     贯通右半行
// 参数说明     无
// 返回参数     void
// 使用示例     RCross_Row();
// 备注信息     用于十字路口出口判断及环岛判断
//-------------------------------------------------------------------------------------------------------------------
float RCross_Row(void)
{
    float White_percent;
    int i, col;
    int White_num = 0;
    int Black_num = 0;
    uint16_t point = 5 * (Race_Center - 1);

    //自行选择判断的行
    int row[5] = {45 - 3, 45 , 45 + 3, 45 + 6, 45 + 9};

    for(i = 0; i < 5; i++)
    {
        for(col = Race_Center; col < MyImage_W - 2; col++)
        {
            if(MyImage[row[i]][col] == white_point)
                White_num++;
            else
            {
                Black_num++;
                
            }
        }
    }
    White_percent = 1.0 * White_num / point;

    return White_percent;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取tan值
// 参数说明     无
// 返回参数     void
// 使用示例     Line_tan();
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
void Line_tan(void)
{
		float tan;
	
		if(Left_peak.Col!=none && Left_peak.Row!=none && Left_line.Col[0]<=25 && Left_line.Row[0]!=none )
		{
			Left_Side_Tan = -(float)(Left_peak.Row-87)/(float)(Left_peak.Col-8);
			Left_Road_Tan = -(float)(Left_line.Row[(Left_peak.pin)/2]-87)/(float)(Left_line.Col[(Left_peak.pin)/2]-8);

		}
		else if(Left_peak.Col!=none && Left_peak.Row!=none && Left_line.Col[0]>25 && Left_line.Row[0]!=none )
		{
			Left_Side_Tan = -(float)(Left_peak.Row-Left_line.Row[0])/(float)(Left_peak.Col-Left_line.Col[0]);
			Left_Road_Tan = -(float)(Left_line.Row[(Left_peak.pin)/2]-Left_line.Row[0])/(float)(Left_line.Col[(Left_peak.pin)/2]-Left_line.Col[0]);

		}

		else Left_Side_Tan = none;
		
		
		
		if(Right_peak.Col!=none && Right_peak.Row!=none && Right_line.Col[0]>=115 && Right_line.Row[0]!=none )
		{
			Right_Side_Tan = -(float)(Right_peak.Row-87)/(float)(Right_peak.Col-132);
			Right_Road_Tan = -(float)(Right_line.Row[(Right_peak.pin)/2]-87)/(float)(Right_line.Col[(Right_peak.pin)/2]-132);
		}
		else if(Right_peak.Col!=none && Right_peak.Row!=none && Right_line.Col[0]<115 && Right_line.Row[0]!=none )
		{
			Right_Side_Tan = -(float)(Right_peak.Row-Right_line.Row[0])/(float)(Right_peak.Col-Right_line.Col[0]);
			Right_Road_Tan = -(float)(Right_line.Row[(Right_peak.pin)/2]-Right_line.Row[0])/(float)(Right_line.Col[(Right_peak.pin)/2]-Right_line.Col[0]);
		}

		else Right_Side_Tan = none;

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取tan值
// 参数说明     无
// 返回参数     void
// 使用示例     Line_tan();
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
void Line_tan1(void)
{
		float tan;
	
		if(Left_peak.Col<=25 && Left_peak.Row!=none && Left_line.Col[0]!=none && Left_line.Row[0]!=none )
		{
			Left_Side_Tan = -(float)(Left_line.Row[0]-87)/(float)(Left_line.Col[0]-8);
			Left_Road_Tan = -(float)(Left_line.Row[(Left_peak.pin)/2]-87)/(float)(Left_line.Col[(Left_peak.pin)/2]-8);

		}
		else if(Left_peak.Col>25 && Left_peak.Row!=none && Left_line.Col[0] !=none && Left_line.Row[0]!=none )
		{
			Left_Side_Tan = -(float)(Left_line.Row[0]-Left_peak.Row)/(float)(Left_line.Col[0]-Left_peak.Col);
			Left_Road_Tan = -(float)(Left_line.Row[(Left_peak.pin)/2]-Left_peak.Row)/(float)(Left_line.Col[(Left_peak.pin)/2]-Left_peak.Col);

		}
		else 
		{
			Left_Side_Tan = none;
			Left_Road_Tan = 0;
		}

//		if(Left_peak.Col!=none && Left_peak.Row!=none && Left_line.Col[0] !=none && Left_line.Row[0]!=none )
//		{
//			Left_Side_Tan = -(float)(Left_line.Row[0]-Left_peak.Row)/(float)(Left_line.Col[0]-Left_peak.Col);
//			Left_Road_Tan = -(float)(Left_line.Row[(Left_peak.pin)/2]-Left_peak.Row)/(float)(Left_line.Col[(Left_peak.pin)/2]-Left_peak.Col);
//		}

		
		
		if(Right_peak.Col>=115 && Right_peak.Row!=none && Right_line.Col[0]!=none && Right_line.Row[0]!=none )
		{
			Right_Side_Tan = -(float)(Right_line.Row[0]-87)/(float)(Right_line.Col[0]-132);
			Right_Road_Tan = -(float)(Right_line.Row[(Right_peak.pin)/2]-87)/(float)(Right_line.Col[(Right_peak.pin)/2]-132);
		}
		else if(Right_peak.Col<115 && Right_peak.Row!=none && Right_line.Col[0]!=none && Right_line.Row[0]!=none )
		{
			Right_Side_Tan = -(float)(Right_line.Row[0]-Right_peak.Row)/(float)(Right_line.Col[0]-Right_peak.Col);
			Right_Road_Tan = -(float)(Right_line.Row[(Right_peak.pin)/2]-Right_peak.Row)/(float)(Right_line.Col[(Right_peak.pin)/2]-Right_peak.Col);
		}
		else
		{
			Right_Side_Tan = none;
			Right_Road_Tan = 0;
		}

//		if(Right_peak.Col!=none && Right_peak.Row!=none && Right_line.Col[0] !=none && Right_line.Row[0]!=none )
//		{
//				Right_Side_Tan = -(float)(Right_line.Row[0]-Right_peak.Row)/(float)(Right_line.Col[0]-Right_peak.Col);
//				Right_Road_Tan = -(float)(Right_line.Row[(Right_peak.pin)/2]-Right_peak.Row)/(float)(Right_line.Col[(Right_peak.pin)/2]-Right_peak.Col);
//		}
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取角度
// 参数说明     无
// 返回参数     void
// 使用示例     Line_Angle();
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
void Line_Angle()
{
//	float left_tan;
//	float right_tan;
//	
//	left_tan = Left_Side_Tan;
		Line_tan1();
		if(Left_Side_Tan != none)
			{
				Left_Side_Angle = tan_to_Angle(Left_Side_Tan);
				Left_Road_Angle = tan_to_Angle(Left_Road_Tan);		
			}
			
		else
		{
			Left_Side_Angle = none;
			Left_Road_Angle = none;
		}
		
		if(Right_Side_Tan != none)
		{
			Right_Side_Angle = tan_to_Angle(Right_Side_Tan);
			Right_Road_Angle = tan_to_Angle(Right_Road_Tan);;
		}	
		
		else
		{
			Right_Side_Angle = none;
			Right_Road_Angle = none;
		}
}

//================================================处理元素数据获取============================================================

//================================================边线处理部分============================================================




//-------------------------------------------------------------------------------------------------------------------
// 函数简介     数据初始化
// 参数说明     无
// 返回参数     void
// 使用示例     Data_Init();
// 备注信息     对数据进行清除,防止影响下一次写入
//-------------------------------------------------------------------------------------------------------------------
void Data_Init(void)
{
    uint8_t pin;
	
    //左右边线顶点清理
    for(pin = 0; pin < MyImage_H; pin++)
    {
        Left_line.Row[pin] = none;
        Left_line.Col[pin] = none;
        Right_line.Row[pin] = none;
        Right_line.Col[pin] = none;
    }
		
	

    for(pin = 0; pin < MyImage_H; pin++)
    {
        Center_Line_Col[pin] = 73;
        Center_Line_Row[pin] = none;
    }

		Len_Left  = 0;
		Len_Right = 0;
		
		if(Box_State==No_box)
		{
			//箱子数据清理
			Box.X = 0;
			Box.Y = 0;
			X_err = 0;
			Y_err = 0;
		}
    //边线顶点清理
    Left_peak.Col = none;
    Left_peak.Row = none;
    Left_peak.pin = 0;
    Right_peak.Col = none;
    Right_peak.Row = none;
    Right_peak.pin = 0;

    //十字路口顶点清理
    LCrossPeak.Col = none;
    LCrossPeak.Row = none;
    RCrossPeak.Col = none;
    RCrossPeak.Row = none;

    //环岛顶点清理
		Round_peak.Col = none;
		Round_peak.Row = none;
		Round_peak.pin = 0;
    Round_peak_Row = none;
    Round_peak_Col = none;

    //标志位清理
	  Stop_flag = 0;
    Left_Start_flag = 0;
    Right_Start_flag = 0;
    LeftlineJump_flag = 0;
    RightlineJump_flag = 0;

    //贯通列计算 贯通行计算 停车行计算 左半贯通行计算 右半贯通行计算
    Cross_Col_Per = Cross_Col();
    Cross_Row_Per = Cross_Row(50);
    Left_Row_Per = 	LCross_Row();
    Right_Row_Per = RCross_Row();
		
		Left_Side_Tan    = none;		
		Left_Side_Angle  = none;	
		Right_Side_Tan   = none;	
		Right_Side_Angle = none;
		
}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     寻找左边线起点
// 参数说明     void
// 返回参数     void
// 使用示例     LeftStartFind();
// 备注信息     寻找边线辅助用
//-------------------------------------------------------------------------------------------------------------------
void LeftStartFind(void)
{
    int row,col;

    //寻找左边线起点(此处条件为找到第一个符合的点就停止)
    for(row = MyImage_H - 2; row > (MyImage_H / 2) && Left_line.Row[0] == none; row--)
    {
					
      for(col = Race_Center + 15; col > 2; col--)
      {
          //先找到两个白点
        if(MyImage[row][col] == white_point && MyImage[row][col - 1] == white_point)
        {
            //再找到两个黑点
          if((MyImage[row][col - 2] == black_point || MyImage[row][col - 2] == red_point)&& (MyImage[row][col - 3] == black_point||MyImage[row][col - 3] == black_point))
          {
             //发现左边线起始点,进行记录
             Left_line.Row[0]= row;
             Left_line.Col[0]= col - 1;
             Left_Start_flag = 1;
							#if Image_color
								//给图像上色
							 MyImage[row][col - 1] = red_point;
							 MyImage[row][col - 2] = red_point;
							#endif							
						
             break;
          }
        }
      }
    }

}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     寻找右边线起点
// 参数说明     void
// 返回参数     void
// 使用示例     RightStartFind();
// 备注信息     寻找边线辅助用
//-------------------------------------------------------------------------------------------------------------------
void RightStartFind(void)
{
  int row,col;
			
  //寻找右边线起点(此处条件为找到第一个符合的点就停止)
  for(row = MyImage_H - 2; row > (MyImage_H / 2 ) && Right_line.Row[0] == none; row--)
  {
    for(col = Race_Center - 15; col < MyImage_W - 2; col++)
    {
        //先找到两个白点
      if(MyImage[row][col] == white_point && MyImage[row][col + 1] == white_point)
      {
          //再找到两个黑点
        if((MyImage[row][col + 2] == black_point || MyImage[row][col + 2] == red_point) && (MyImage[row][col + 3] == black_point || MyImage[row][col + 3] == red_point))
        {
            //发现右边线起始点,进行记录
           Right_line.Row[0] = row;
           Right_line.Col[0] = col + 1;
           Right_Start_flag = 1;
           #if Image_color 
					//给图像上色
           MyImage[row][col + 1] = red_point;
           MyImage[row][col + 2] = red_point;
				   #endif

					
        }
      }
    }
  }

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取图像左边线
// 参数说明     无
// 返回参数     void
// 使用示例     GetLeftSide();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void GetLeftSide(void)
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

    for(row = Left_line.Row[0] - pin; pin < MyImage_H-2 ; pin++)
    {
        find = 0;
        row = Left_line.Row[0] - pin;

        //图像基本扫描完成,停止扫描边线
        if(row <= 1) {break;}

        //范围选定
        colmin = Left_line.Col[pin - 1] - 3;
        colmax = Left_line.Col[pin - 1] + 8;

        if(colmin <= 3){colmin = 3;};
        if(colmax >= MyImage_W - 3){colmax = MyImage_W - 3;};

       for(col = colmax; col >= colmin; col--)
        {
            if(MyImage[row][col] == white_point && MyImage[row][col - 1] == white_point)
            {
                if(MyImage[row][col - 2] == black_point && MyImage[row][col - 3] == black_point)
                {
                    Left_line.Row[pin] = row;
                    Left_line.Col[pin] = col - 2;
                    find = 1;
                    L_lenth++;
                    #if Image_color  //给图像上色
                    MyImage[row][col - 2] = red_point;
                    MyImage[row][col - 1] = red_point;
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
	
			 if(L_lenth <= 20)
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
void GetRightSide(void)
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

    for(row = Right_line.Row[0] - pin; pin < MyImage_H-2 ; pin++)
    {
        find = 0;
        row = Right_line.Row[0] - pin;

        //图像基本扫描完成,停止扫描边线
        if(row <= 1) {break;}

        //范围选定
        colmin = Right_line.Col[pin - 1] - 8;
        colmax = Right_line.Col[pin - 1] + 3;


        if(colmin <= 3){colmin = 3;};
        if(colmax >= MyImage_W - 3){colmax = MyImage_W - 3;};

        for(col = colmin; col <= colmax; col++)
        {
            if(MyImage[row][col] == white_point && MyImage[row][col + 1] == white_point)
            {
                if((MyImage[row][col + 2] == black_point || MyImage[row][col + 2] ==red_point) && (MyImage[row][col + 3] == black_point || MyImage[row][col + 3] == red_point))
                {
                    Right_line.Row[pin] = row;
                    Right_line.Col[pin] = col + 2;
                    find = 1;
                    R_lenth++;
                    #if Image_color //给图像上色
                    MyImage[row][col + 2] = red_point;
                    MyImage[row][col + 1] = red_point;
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
				
				if(R_lenth <= 20)
				{
						Right_Start_flag = 0;
						Right_line.Row[0] = none;
						Right_line.Col[0] = none;
				}

		
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     二次扫线(左边线)
// 参数说明     Peak_Pin    扫线开始的边线顶点序号
// 返回参数     void
// 使用示例     FindLeftLineTwice();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void FindLeftLineTwice(int Peak_Pin)
{
    uint8_t pin, find;
    int row, col;
    int colmin, colmax;

    pin = 0;
		Peak_Pin=0;
    for(row = Left_line.Row[Peak_Pin] - pin - 1;row < MyImage_H - 2; pin++)
    {
        find = 0;
        row = Left_line.Row[Peak_Pin] - pin - 1;

        //图像基本扫描完成,停止扫描边线
        if(row <= 1) {break;}

        //范围选定
        colmin = Left_line.Col[Peak_Pin + pin] - 10;
        colmax = Left_line.Col[Peak_Pin + pin] + 10;


        if(colmin <= 5){colmin = 5;};
        if(colmax >= MyImage_W - 1){colmax = MyImage_W - 1;};

        for(col = colmax; col >= colmin; col--)
        {
            if(MyImage[row][col] == white_point && MyImage[row][col - 1] == white_point)
            {
                if(MyImage[row][col - 2] == black_point)
                {
                    Left_line.Row[Peak_Pin + pin + 1] = row;
                    Left_line.Col[Peak_Pin + pin + 1] = col - 2;
                    find = 1;
										#if Image_color 
                    //给图像上色
                    MyImage[row][col - 2] = red_point;
                    MyImage[row][col - 1] = red_point;
										#endif
                    break;
                }
            }
        }

        if(find == 0)
        {
            Left_peak.pin = Peak_Pin + pin + 1;
            break;
        }
    }

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     二次扫线(右边线)
// 参数说明     Peak_Pin    扫线开始的边线顶点序号
// 返回参数     void
// 使用示例     FindRightLineTwice();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void FindRightLineTwice(int Peak_Pin)
{
    uint8_t pin, find;
    int row, col;
    int colmin, colmax;

    pin = 0;
		Peak_Pin=0;

    for(row = Right_line.Row[Peak_Pin] - pin - 1;row < MyImage_H - 2; pin++)
    {
        find = 0;
        row = Right_line.Row[Peak_Pin] - pin - 1;

        //图像基本扫描完成,停止扫描边线
        if(row <= 1) {break;}

        //范围选定
        colmin = Right_line.Col[Peak_Pin + pin] - 10;
        colmax = Right_line.Col[Peak_Pin + pin] + 10;


        if(colmin <= 5){colmin = 5;};
        if(colmax >= MyImage_W - 1){colmax = MyImage_W - 1;};

        for(col = colmin; col <= colmax; col++)
        {
            if(MyImage[row][col] == white_point && MyImage[row][col + 1] == white_point)
            {
                if(MyImage[row][col + 2] == black_point)
                {
                    Right_line.Row[Peak_Pin + pin + 1] = row;
                    Right_line.Col[Peak_Pin + pin + 1] = col + 2;
                    find = 1;
                    #if Image_color //给图像上色
                    MyImage[row][col + 2] = red_point;
                    MyImage[row][col + 1] = red_point;
										#endif
                    break;
                }
            }
        }

        if(find == 0)
        {
            Right_peak.pin = Peak_Pin + pin + 1;
            break;
        }
    }

}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取图像左右边线
// 参数说明     无
// 返回参数     void
// 使用示例     GetLRSide();
// 备注信息7
//-------------------------------------------------------------------------------------------------------------------
void GetLRSide(void)
{

    //寻找起点
    LeftStartFind();
    RightStartFind();
//    //寻找边线
    GetLeftSide();
    GetRightSide();
	
	
//    //左右边线方差计算
//    L_S = S_Calculate(0);
//    R_S = S_Calculate(1);

}




//-------------------------------------------------------------------------------------------------------------------
// 函数简介     左边线顶点
// 参数说明     无
// 返回参数     void
// 使用示例     LeftSide_Peak();
// 备注信息     严格上来讲是寻找左边线列最大的点
//-------------------------------------------------------------------------------------------------------------------
void LeftSide_Peak(void)
{

    int temp, pin, col_max = 0;

    //如果不存在左边线起始点
    if(Left_line.Row[0] == none)
    {
        return;
    }

    for(pin = 0; pin < MyImage_H - 2; pin++)
    {
			
        //若该点无数据
        if(Left_line.Row[pin] == none)
        {
            break;
        }

        //打擂台,寻找左边线列最大处点
        if(col_max <= Left_line.Col[pin])
        {
            col_max = Left_line.Col[pin];
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
void RightLine_Peak(void)
{
    int temp, pin, col_min = MyImage_W;

    //如果不存在右边线起始点
    if(Right_line.Row[0] == none)
    {
        return;
    }

    for(pin = 0; pin < MyImage_H - 2; pin++)
    {
        //若该点无数据
        if(Right_line.Row[pin] == none)
        {
            break;
        }

        //打擂台,寻找右边线列最小处点
        if(col_min >= Right_line.Col[pin])
        {
            col_min = Right_line.Col[pin];
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
void SideLine_Peak(void)
{
    LeftSide_Peak();
    RightLine_Peak();
	
	
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     跳变点检测
// 参数说明     无
// 返回参数     void
// 使用示例     JumpPoint_Check();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void JumpPoint_Check(void)
{
    int pin;
    int left_pin, right_pin;

    //遍历寻找跳变点
    for(pin = 0; Left_line.Row[pin] != none && pin < 70; pin++)
    {
        //发现左边线存在向图像外侧弯折的边线
        if((Left_line.Col[pin] - Left_line.Col[pin + 3]) > 3)
        {
            //发现边线跳变
            Left_jump.Row = Left_line.Row[pin];
            Left_jump.Col = Left_line.Col[pin];
            left_pin = pin;
            LeftlineJump_flag = 1;
            break;
        }
				else if((Right_line.Col[pin] - Right_line.Col[pin + 3]) < -3)
				{
					//发现边线跳变
            Right_jump.Row = Right_line.Row[pin];
            Right_jump.Col = Right_line.Col[pin];
            right_pin = pin;
            RightlineJump_flag = 1;
            break;
				}
    }

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     左边丢线检查
// 参数说明     无
// 返回参数     void
// 使用示例     LeftLossLine_Check();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void LeftLossLine_Check(void)
{
    uint8_t pin;
    int Row, Col;

    //寻找左边断线
    //左顶点不存在
    if(Left_peak.Row == none )
    {
        Row = 80;
        Col = 10;
    }
    //左顶点存在
    else
    {
        Row = Left_peak.Row - 3;
        Col = Left_peak.Col + 12;
    }

    Left_loss.Len = 0;

    for(pin = 0; Row - pin >= 5; pin++)
    {
        //如果这个点是白点
        if(MyImage[Row - pin][Col] == white_point)
        {
            Left_loss.Len += 1;
//            //在画布上着色,调试时使用(可能会导致环岛顶点抓取失败)
//            MyImage[Row - pin][Col] = blue_point;
        }
        //如果是黑点
        else
        {
            Left_loss.Row = Row - pin;//断点后往上寻点，可能是十字左上顶点
            Left_loss.Col = Col;
            break;
        }
    }

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     右边丢线检查
// 参数说明     无
// 返回参数     void
// 使用示例     RightLossLine_Check();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void RightLossLine_Check(void)
{
    uint8_t pin;
    int Row, Col;

    //寻找右边断线
    //右顶点不存在
    if(Right_peak.Row == none)
    {
        Row = 80;
        Col = 135;
    }
    //右顶点存在
    else
    {
        Row = Right_peak.Row - 3;
        Col = Right_peak.Col - 8;
    }

    Right_loss.Len = 0;

    for(pin = 0; Row - pin >= 5; pin++)
    {
        //如果这个点是白点
        if(MyImage[Row - pin][Col] == white_point)
        {
            Right_loss.Len += 1;
//            //在画布上着色,调试时使用(可能会导致环岛顶点抓取失败)
//            MyImage[Row - pin][Col] = blue_point;
        }
        //如果是黑点
        else
        {
            Right_loss.Row = Row - pin;//断点后往上寻点，可能是十字右上顶点
            Right_loss.Col = Col;
            break;
        }
    }

}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     丢线检查
// 参数说明     无
// 返回参数     void
// 使用示例     LossLine_Check();
// 备注信息     用于寻找十字上拐点,并把断线的区域用蓝点画出来
//-------------------------------------------------------------------------------------------------------------------
void LossLine_Check(void)
{
    LeftLossLine_Check();
    RightLossLine_Check();
	
	
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     两点斜率补线法
// 参数说明     Col_Array       拟合点列坐标,例如:x1,x2
// 参数说明     Row_Array       拟合点行坐标,例如:y1,y2
// 参数说明     sideline        拟合的边线   Fix_Left:左边线  Fix_Right:右边线
// 返回参数     无
// 使用示例     Normal_LineFit(Col_Array, Row_Array, Fix_Left);
// 备注信息     拟合后的数值会直接作用在边线上,该函数只接受两点拉线
//-------------------------------------------------------------------------------------------------------------------
void Normal_LineFit(int *Col_Array, int *Row_Array, fix_line sideline)
{
    int i = 0, j = 0;

    /* 直线 Col = k * Row + b */
    float k = 0.0f, b = 0.0f;

    switch(sideline)
    {
      case Fix_Left://左补线
        {
						if(Row_Array[1] == Row_Array[0])return;
            k = (float)((float)Col_Array[1] - (float)Col_Array[0]) / (float)((float)Row_Array[1] - (float)Row_Array[0]);
            b = (float)Col_Array[0] - (float)Row_Array[0] * k;

            //十字补线以及出环岛补线
            if(Cross_State != No_Cross || Roundabout_State == Round_Turn || Roundabout_State == Find_Round || Roundabout_State == Check_Round )
            {
                // 使用计算所求的k, b对边线进行修整
                for (i = Row_Array[0]; i > Row_Array[1]; i--)
                {
                    Left_line.Row[Left_peak.pin + j] = i;
                    Left_line.Col[Left_peak.pin + j] = i * k + b;

                    //画点
                    MyImage[Left_line.Row[Left_peak.pin + j]][Left_line.Col[Left_peak.pin + j]] = black_point;

                    //调试用代码
                    //ips200_show_float(0, 264, k, 3, 1);
                    //ips200_show_float(40, 264, b, 3, 1);

                    j++;
                }
            }

            if(Roundabout_State == Into_Round || Roundabout_State == Out_Round)
            {
                // 使用计算所求的k, b对边线进行修整
                for (i = Row_Array[0]; i >= Row_Array[1]; i--)
                {
                    Left_line.Row[j] = i;
                    Left_line.Col[j] = i * k + b;
                    //画点
                    MyImage[Left_line.Row[j]][Left_line.Col[j]] = black_point;

//                    ips200_show_float(0, 264, k, 3, 1);
//                    ips200_show_float(40, 264, b, 3, 1);

                    j++;
                }

                //拟合结束,重新赋值顶点
                Left_peak.pin = j;
            }

            break;
        }

      case Fix_Right://右补线
        {
						if(Row_Array[1] == Row_Array[0])return;
            k = (float)((float)Col_Array[1] - (float)Col_Array[0]) / (float)((float)Row_Array[1] - (float)Row_Array[0]);
            b = (float)Col_Array[0] - (float)Row_Array[0] * k;

            if(Cross_State != No_Cross || Roundabout_State == Round_Turn || Roundabout_State == Find_Round || Roundabout_State == Check_Round )
            {
                // 使用计算所求的k, b对边线进行修整
                for (i = Row_Array[0]; i > Row_Array[1]; i--)
                {
                    Right_line.Row[Right_peak.pin + j] = i;
                    Right_line.Col[Right_peak.pin + j] = i * k + b;

                    //画点
                    MyImage[Right_line.Row[Right_peak.pin + j]][Right_line.Col[Right_peak.pin + j]] = black_point;

                    //调试用代码
                    //ips200_show_float(0, 264, k, 3, 1);
                    //ips200_show_float(40, 264, b, 3, 1);

                    j++;
                }
            }

            if(Roundabout_State == Into_Round || Roundabout_State == Out_Round)
            {
                // 使用计算所求的k, b对边线进行修整
                for (i = Row_Array[0]; i >= Row_Array[1]; i--)
                {
                    Right_line.Row[j] = i;
                    Right_line.Col[j] = i * k + b;
                    //画点
                    MyImage[Right_line.Row[j]][Right_line.Col[j]] = black_point;
        //            ips200_show_float(0, 264, a, 3, 1);
        //            ips200_show_float(40, 264, b, 3, 1);

                    j++;
                }

                //拟合结束,重新赋值顶点
                Right_peak.pin = j;
            }

            break;
        }

    }
}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     直线赛道判断
// 参数说明     无
// 返回参数     void
// 使用示例     Straight_Race_check();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8_t Straight_Race_check(void)
{

    //左,右边线不丢失且贯通列白点占比很高
    if(Cross_Col() >= 0.9)
    {
        return 1;
    }

    return 0;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     部分数组求和
// 参数说明     Array                       需求和数组
// 参数说明     length                      数组需求和部分
// 返回参数     array_sum                   数组求和结果
// 使用示例     ArrayPart_Sum(x, 10);       对数组x前十个元素进行求和
// 备注信息     最大支持对256长度数组进行求和
//-------------------------------------------------------------------------------------------------------------------
uint32_t ArrayPart_Sum(uint8_t *Array, uint8_t length)
{
    uint8_t i;
    uint32_t array_sum;
    for(i = 0; i < length; i ++)
    {
        array_sum += Array[i];
    }

    return array_sum;
}


//================================================边线处理部分============================================================


//================================================边线元素处理部分============================================================

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     直线处理
// 参数说明     无
// 返回参数     void
// 使用示例     StraightRace_handler();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void StraightRace_handler(void)
{
	  int Row;
    int pin=1;

	
    //两边线起始点都不存在,直接返回
    if(Left_Start_flag == 0 && Right_Start_flag == 0)
    {

        return;
    }
//		//找到双线
    else if(Left_Start_flag == 1 && Right_Start_flag == 1)
		{

							//左边线较长
					if(Left_peak.pin >= Right_peak.pin + 40)
					{

							for(Row = Left_line.Row[0] - pin; pin < MyImage_H-30; pin++)
							{
										Row = Left_line.Row[0] - pin;
										Center_Line_Row[pin] = Row;																						 //y
										Center_Line_Col[pin] = ((Left_line.Row[0] - pin)/ref + Left_line.Col[pin]);
								
										MyImage[Center_Line_Row[pin]][Center_Line_Col[pin]] = black_point;
										ref-=reduce;	
							}
							ref=ref_;
							return;				

					}
					//右边线较长
					else if(Left_peak.pin + 40 <= Right_peak.pin)
					{
							for(Row = Right_line.Row[0] - pin; pin < MyImage_H-30; pin++)
							{
										Row = Right_line.Row[0] - pin;
										Center_Line_Row[pin] = Row;																						 //y
										Center_Line_Col[pin] = ( Right_line.Col[pin]-(Right_line.Row[0] - pin)/ref);
								
										MyImage[Center_Line_Row[pin]][Center_Line_Col[pin]] = black_point;
										ref-=reduce;
							}
							ref=ref_;
							return;
					}
					else
					{
							for(Row = Left_line.Row[0] - pin; pin < MyImage_H-30; pin++)
							{
										Row = Left_line.Row[0] - pin;
										Center_Line_Row[pin] = Row;																						 //y
										Center_Line_Col[pin] = (Right_line.Col[pin] + Left_line.Col[pin])>>1;				
										MyImage[Center_Line_Row[pin]][Center_Line_Col[pin]] = black_point;	
							}			
						return;
					}
		}
		//仅找到左线
		else if(Left_Start_flag == 1 && Right_Start_flag == 0)
		{
        for(Row = Left_line.Row[0] - pin; pin < MyImage_H-30; pin++)
        {
							Row = Left_line.Row[0] - pin;
							Center_Line_Row[pin] = Row;																						 //y
							Center_Line_Col[pin] = ((Left_line.Row[0] - pin)/ref+Left_line.Col[pin]);
					
							MyImage[Center_Line_Row[pin]][Center_Line_Col[pin]] = black_point;
							ref-=reduce;				
				}
				ref=ref_;
				return;
		}
		//仅找到右线
		else if(Left_Start_flag == 0 && Right_Start_flag == 1)
		{
        for(Row = Right_line.Row[0] - pin; pin < MyImage_H-30; pin++)
        {
							Row = Right_line.Row[0] - pin;
							Center_Line_Row[pin] = Row;																						 //y
							Center_Line_Col[pin] = (Right_line.Col[pin] - (Right_line.Row[0] - pin)/ref);				
							MyImage[Center_Line_Row[pin]][Center_Line_Col[pin]] = black_point;
							ref-=reduce;	

								
				}
				ref=ref_;
				return;
		}
		
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     直线处理
// 参数说明     无
// 返回参数     void
// 使用示例     angle_err();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
float angle_err()
{
	int16 Servo_error=0,Weight_sum=0;

		for(int8 pin = 0; pin < MyImage_H; pin++)
		{
				if(Center_Line_Col[pin] != none && Center_Line_Row[pin] < MyImage_H)
				{
						//误差由中线差值乘以权重对应行的权重
						Servo_error += Servo_Weight[Center_Line_Row[pin]] * (Center_Line_Col[pin] - Race_Center);         //此处需要乘以修正值
						Weight_sum += Servo_Weight[Center_Line_Row[pin]];
				}
		}
		Servo_error /= Weight_sum;

		return Servo_error;
}


//================================================十字处理部分============================================================


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     十字路口检查
// 参数说明     无
// 返回参数     进入十字路口: 1   未进入十字路口: 0
// 使用示例     CrossRoad_Check();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8_t CrossRoad_Check(void)
{
    //十字检测
    if(Left_loss.Len >= 20 && Right_loss.Len >= 20 &&																																			 //左右丢线超20
			 Left_peak.Row > (MyImage_H / 2 + 5)	&& Right_peak.Row > (MyImage_H / 2 + 5) &&																			 //左右顶点低于画面一半
			(Left_peak.Col < MyImage_W / 2 || Left_peak.Col == none) &&	(Right_peak.Col > MyImage_W / 2 || Right_peak.Col == none) &&//左顶点在左半或不见与右顶点在右半或不见
			(Left_Row_Per>0.8 &&	Right_Row_Per>0.8	))//左右中部都有大片空白
    {
        
        return 1;
    }
		//斜入十字检测
		else if((Left_loss.Len >= 20 && LeftlineJump_flag==1) || (Right_loss.Len >= 20 && RightlineJump_flag))
    {
        
        return 2;
    }
			

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     十字路口左边线拟合
// 参数说明     void
// 返回参数     无
// 使用示例     Cross_FixLeft_Line();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Cross_FixLeft_Line(void)
{
    int row, col;
    int Col_Array[2], Row_Array[2];

    row = Left_loss.Row - 3;    //-1是为了抓拐点更加稳定

    //扫线,寻找左上拐点
    for(col = Left_loss.Col; col < Race_Center + 20; col++)
    {
        if(MyImage[row][col] == black_point)
        {
            if(MyImage[row][col + 1] == white_point && MyImage[row][col + 2] == white_point )
            {
                //记录可能是十字路口顶点的点,保留row最大的那个
                LCrossPeak.Col = col;
                LCrossPeak.Row = row;
                break;
            }
        }
    }

    if(LCrossPeak.Col == none || LCrossPeak.Row == none || LCrossPeak.Row < 5)
    {
        return;
    }

    if(Left_peak.Col != none && Left_peak.Row > MyImage_H / 2)
    {
        //赋值需要拟合的参数
        Col_Array[0] = Left_peak.Col;
        Row_Array[0] = Left_peak.Row;
        Col_Array[1] = LCrossPeak.Col;
        Row_Array[1] = LCrossPeak.Row;
    }
    else
    {
        //赋值需要拟合的参数
        Col_Array[0] = 20;
        Row_Array[0] = 90;
        Col_Array[1] = LCrossPeak.Col;
        Row_Array[1] = LCrossPeak.Row;

        //修正拟合补线时的起点
        Left_peak.pin = 0;

    }

    //对左边线进行补线
    Normal_LineFit(Col_Array, Row_Array, Fix_Left);

    //补完线把边线起始点重置,提供拟合中线条件
    Left_Start_flag = 1;

    //中线拟合的终点由顶点决定,这里应该重新赋值
    Left_peak.pin += (Row_Array[0] - Row_Array[1]);

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     十字路口右边线拟合
// 参数说明     void
// 返回参数     无
// 使用示例     Cross_FixRight_Line();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Cross_FixRight_Line(void)
{
    int row, col;
    int Col_Array[2], Row_Array[2];

    row = Right_loss.Row - 3;    //-1是为了抓拐点更加稳定

    //扫线,寻找右上拐点
    for(col = Right_loss.Col; col > Race_Center - 15 ; col--)
    {
        if(MyImage[row][col] == black_point)
        {
            if(MyImage[row][col - 1] == white_point && MyImage[row][col - 2] == white_point )
            {
                //记录可能是十字路口顶点的点,保留row最大的那个
                RCrossPeak.Col = col;
                RCrossPeak.Row = row;
                break;
            }
        }
    }


    if(RCrossPeak.Col == none || RCrossPeak.Row == none || RCrossPeak.Row < 10)
    {
        return;
    }

    if(Right_peak.Col != none && Right_peak.Row > MyImage_H / 2)
    {
        //赋值需要拟合的参数
        Col_Array[0] =  Right_peak.Col;
        Row_Array[0] =  Right_peak.Row;
        Col_Array[1] =  RCrossPeak.Col;
        Row_Array[1] =  RCrossPeak.Row;
    }
    else
    {
        //赋值需要拟合的参数
        Col_Array[0] =  125;
        Row_Array[0] =  90;
        Col_Array[1] =  RCrossPeak.Col;
        Row_Array[1] =  RCrossPeak.Row;

        //修正拟合补线时的起点
        Right_peak.pin = 0;
    }


    //对右边线进行补线
    Normal_LineFit(Col_Array, Row_Array, Fix_Right);

    //补完线把边线起始点重置,提供拟合中线条件
    Right_Start_flag = 1;

    //中线拟合的终点由顶点决定,这里应该重新赋值
    Right_peak.pin += (Row_Array[0] - Row_Array[1]);

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     十字路口处理
// 参数说明     void
// 返回参数     无
// 使用示例     CrossRoad_handler();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void CrossRoad_handler(void)
{

    //进出弯处理
    if(Cross_State == Before_Turn)
    {
        //位于十字内,且左,右半贯通行出现大量黑点
        if(Left_Row_Per <= 0.63 || Right_Row_Per <= 0.63)
        {
            //进入十字弯道
            Cross_State = Stop_State;
            return;
        }

				
        //左边线补线和右边线补线
        Cross_FixLeft_Line();
        Cross_FixRight_Line();

        //中线拟合
        StraightRace_handler();
				

    }

    if(Cross_State == In_Cross)
    {
			//重新找到线
        if(Left_line.Row[5] >= 70 && Right_line.Row[5] >= 70 && Left_line.Row[5] != none && Right_line.Row[5] != none)
        {
            Cross_State = Cross_Turn;
        }
				
				Cross_FixLeft_Line();
        Cross_FixRight_Line();
        StraightRace_handler();
				return;
    }
		if(Cross_State == Stop_State)	
		{
			
				if(Start_Flag==0)Start=0;
				if(Start==0)
				{
					if(Bufcnt1(!Start_Flag,3000))
					{
						Start_Flag = 1;
						Start = 1;
						Cross_State = In_Cross;
					}
				}

		}

    //十字弯道转弯段
    if(Cross_State == Cross_Turn)
    {

        if(Cross_Row_Per >= 0.65)
        {
            Cross_State = Out_Cross;
            return;
        }

        //当成直弯道处理
        StraightRace_handler();
    }

    //离开十字
    if(Cross_State == Out_Cross)
    {
        //如果两边线起始点位于较低侧,正入十字检测
        if((Left_line.Row[1] >= 70 || Right_line.Row[1]>= 70) &&
					  (Left_line.Row[1] != none && Right_line.Row[1] != none) &&
					Left_Row_Per <= 0.73 || Right_Row_Per <= 0.73)
        {
            //离开十字路口,工作状态恢复
            Cross_State = No_Cross;
            Cross_DirFlag = No_Into;
            Work_State = No_Duel;
						return;
        }

        //左边线补线和右边线补线
        Cross_FixLeft_Line();
        Cross_FixRight_Line();

        //中线拟合
        StraightRace_handler();

       
    }


}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     十字路口处理
// 参数说明     void
// 返回参数     无
// 使用示例     CrossRoad_handler();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Intersection_handler(void)
{

	
    //进入路口前
    if(Cross_State == Before_Turn)
    {
        //路口内，即将出路口
        if((Left_line.Row[1] >= 75 || Right_line.Row[1]>= 75) &&
					  (Left_line.Row[1] != none && Right_line.Row[1] != none) &&
					Left_Row_Per <= 0.73 || Right_Row_Per <= 0.73)
        {
            //离开路口,工作状态恢复
            Cross_State = No_Cross;
            Cross_DirFlag = No_Into;
            Work_State = No_Duel;
						return;
        }

        //左边线补线和右边线补线
        Cross_FixLeft_Line();
        Cross_FixRight_Line();
 
        //中线拟合
        StraightRace_handler();
				

    }





}

//================================================十字处理部分============================================================


//================================================圆环处理部分============================================================


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     环岛检查
// 参数说明     无
// 返回参数     进入环岛: 1   未进入环岛: 0
// 使用示例     Roundabout_Check();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8_t Roundabout_Check(void)
{
    //左贯通行白点较多,右边线平直,表示进入环岛,且为左环岛
    if(Left_Row_Per >= 0.90 && Right_peak.Row < 10 && Right_peak.Col >= Race_Center && Left_loss.Len >= 20 && Right_loss.Len <= 30)
    {
        //环岛特殊速度
//        ips200_show_uint(0, 248,Roundabout_DirFlag , 1);
        Roundabout_DirFlag = LeftRound;

        return Find_Round;
    }

    //右贯通行白点较多,左边线平直,表示进入环岛,且为右环岛
    if(Right_Row_Per >= 0.90 && Left_peak.Row < 10 && Left_peak.Col <= Race_Center && Right_loss.Len >= 20 && Left_loss.Len <= 30)
    {

//        ips200_show_uint(0, 248,Roundabout_DirFlag , 1);
        Roundabout_DirFlag = RightRound;

        return Find_Round;
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     环岛补线
// 参数说明     无
// 返回参数     void
// 使用示例     Roundabout_FixLine();
// 备注信息     补好的边线会直接作用在边线结构体中
//-------------------------------------------------------------------------------------------------------------------
void Roundabout_FixLine(void)
{
    int row, col, pin;
    int temp = 0;
    int Col_Array[2], Row_Array[2];

    //左环岛补线
    if(Roundabout_DirFlag == LeftRound)
    {
			
			if(Roundabout_State == Find_Round || Roundabout_State == Check_Round )
        {
					 row = Left_loss.Row - 3;    //-1是为了抓拐点更加稳定
					
					 //扫线,寻找左上拐点
					for(col = Left_loss.Col; col < Race_Center + 15; col++)
						{
								if(MyImage[row][col] == black_point)
								{
										if(MyImage[row][col + 1] == white_point && MyImage[row][col + 2] == white_point)
										{
												//记录可能是十字路口顶点的点,保留row最大的那个
												Round_peak_Col = col;
												Round_peak_Row = row;
												break;
										}
								}
						}
						
						if(Round_peak_Col == none || Round_peak_Row == none || Round_peak_Row < 10)
						{
								return;
						}

						if(Left_peak.Col != none && Left_peak.Row > MyImage_H / 2)
						{
								//赋值需要拟合的参数
								Col_Array[0] = Left_peak.Col;
								Row_Array[0] = Left_peak.Row;
								Col_Array[1] = Round_peak_Col;
								Row_Array[1] = Round_peak_Row;
						}
						else
						{
								//赋值需要拟合的参数
								Col_Array[0] = 20;
								Row_Array[0] = 90;
								Col_Array[1] = Round_peak_Col;
								Row_Array[1] = Round_peak_Row;

								//修正拟合补线时的起点
								Left_peak.pin = 0;

						}

						//对左边线进行补线
						Normal_LineFit(Col_Array, Row_Array, Fix_Left);

						//补完线把边线起始点重置,提供拟合中线条件
						Left_Start_flag = 1;

						//中线拟合的终点由顶点决定,这里应该重新赋值
						Left_peak.pin += (Row_Array[0] - Row_Array[1]);
						
        }
			
        if(Roundabout_State == Into_Round)
        {
            if(Left_line.Row[0] < 70)
            {
                //清理原来的左边线
                for(pin = 0; pin < MyImage_H + 10; pin++)
                {
                    //清除画图
                    MyImage[Left_line.Row[pin]][Left_line.Col[pin]] = black_point;
                    MyImage[Left_line.Row[pin]][Left_line.Col[pin] + 1] = black_point;
                    //清除数值
                    Left_line.Row[pin] = none;
                    Left_line.Col[pin] = none;
                }
            }

            //扫线,寻找左上角点
            for(col = Race_Center - 40; col <= Race_Center + 20; col++)
            {
                for(row = 10; row <= 60; row++)
                {
                    //从上往下,先黑后白
                    if(MyImage[row][col] == black_point && MyImage[row + 1][col] == black_point)
                    {
                        if(MyImage[row + 2][col] == white_point && MyImage[row + 3][col] == white_point)
                        {
                            //temp不存在说明是第一次寻找
                            if(row + 1 > temp || temp == 0)
                            {
                                //不断反复,记录环岛行最大的点
                                Round_peak.Col = col;
                                Round_peak.Row = row + 1;
                                temp = Round_peak.Row;
                            }
                        }
                    }
                }
            }

            //找不到顶点就不修整边线
            if(Round_peak.Row == none)
            {
                return;
            }

            //填充数值
            Col_Array[0] = 140;
            Row_Array[0] = 80;
            Col_Array[1] =  Round_peak.Col;
            Row_Array[1] =  Round_peak.Row;

							//清理原来的右边线
            for(pin = 0; pin < MyImage_H + 10; pin++)
            {
                //清除画图,注释掉不影响实际使用
                MyImage[Right_line.Row[pin]][Right_line.Col[pin]] = black_point;
                MyImage[Right_line.Row[pin]][Right_line.Col[pin] - 1] = black_point;
                //清除数值
                Right_line.Row[pin] = none;
                Right_line.Col[pin] = none;
            }

            //右边线补线
            Normal_LineFit(Col_Array, Row_Array, Fix_Right);

            //二次寻线
            FindRightLineTwice(Right_peak.pin - 1);

            return;
        }

        if(Roundabout_State == Round_Turn)
        {
            //填充数值,拉,就摁拉线
            Col_Array[0] = Right_peak.Col;
            Row_Array[0] = Right_peak.Row;
            Col_Array[1] = 35;
            Row_Array[1] = 10;

            if(Right_peak.Col == none)
            {
                return;
            }

            //右边线补线
            Normal_LineFit(Col_Array, Row_Array, Fix_Right);

            //重新计算中线拟合范围
            Right_peak.pin += (Row_Array[0] - Row_Array[1]);

            return;
        }
				
				if(Roundabout_State == Out_Round)
        {
            //填充数值,拉,就摁拉线
            Col_Array[0] = 135;
            Row_Array[0] = 80;
            Col_Array[1] = 55;//45
            Row_Array[1] = 10;

           
            //左边线补线
            Normal_LineFit(Col_Array, Row_Array, Fix_Right);
					
						Right_Start_flag = 1;

//						Right_peak.pin = (Row_Array[0] - Row_Array[1]);

            return;
        }

    }

    //右环岛补线
    if(Roundabout_DirFlag == RightRound)
    {
			
			if(Roundabout_State == Find_Round || Roundabout_State == Check_Round )
        {
					
					 row = Right_loss.Row - 3;    //-1是为了抓拐点更加稳定
					
					 //扫线,寻找右上拐点
						for(col = Right_loss.Col; col > Race_Center - 15; col--)
						{
								if(MyImage[row][col] == black_point)
								{
										if(MyImage[row][col - 1] == white_point && MyImage[row][col - 2] == white_point)
										{
												//记录可能是十字路口顶点的点,保留row最大的那个
												Round_peak_Col = col;
												Round_peak_Row = row;
												break;
										}
								}
						}
						
						
						if(Round_peak_Col == none || Round_peak_Row == none || Round_peak_Row < 10)
						{
								return;
						}

						if(Right_peak.Col != none && Right_peak.Row > MyImage_H / 2)
						{
								//赋值需要拟合的参数
								Col_Array[0] = Right_peak.Col;
								Row_Array[0] = Right_peak.Row;
								Col_Array[1] = Round_peak_Col;
								Row_Array[1] = Round_peak_Row;
						}
						else
						{
								//赋值需要拟合的参数
								Col_Array[0] = 125;
								Row_Array[0] = 90;
								Col_Array[1] = Round_peak_Col;
								Row_Array[1] = Round_peak_Row;

								//修正拟合补线时的起点
								Right_peak.pin = 0;

						}

						//对左边线进行补线
						Normal_LineFit(Col_Array, Row_Array, Fix_Right);

						//补完线把边线起始点重置,提供拟合中线条件
						Right_Start_flag = 1;

						//中线拟合的终点由顶点决定,这里应该重新赋值
						Right_peak.pin += (Row_Array[0] - Row_Array[1]);
						
        }

        if(Roundabout_State == Into_Round)
        {
            if(Right_line.Row[0] < 70)
            {
                //清理原来的右边线
                for(pin = 0; pin < MyImage_H + 10; pin++)
                {
                    //清除画图
                    MyImage[Right_line.Row[pin]][Right_line.Col[pin]] = black_point;
                    MyImage[Right_line.Row[pin]][Right_line.Col[pin] + 1] = black_point;
                    //清除数值
                    Right_line.Row[pin] = none;
                    Right_line.Col[pin] = none;
                }
            }

            //扫线,寻找右上角点
            for(col = Race_Center + 40; col >= Race_Center - 20; col--)
            {
                for(row = 10; row <= 60; row++)
                {
                    //从上往下,先黑后白
                    if(MyImage[row][col] == black_point && MyImage[row + 1][col] == black_point)
                    {
                        if(MyImage[row + 2][col] == white_point && MyImage[row + 3][col] == white_point)
                        {
                            //temp不存在说明是第一次寻找
                            if(row + 1 > temp || temp == 0)
                            {
                                //不断反复,记录环岛行最大的点
                                Round_peak.Col = col;
                                Round_peak.Row = row + 1;
                                temp = Round_peak.Row;
															  #if Image_color
																MyImage[row + 1][col] = red_point;                //清除数值
																#endif
                            }
                        }
                    }
                }
            }
           //找不到顶点就不修整边线
            if(Round_peak.Row == none)
            {
                return;
            }

            //填充数值
            Col_Array[0] = 10;
            Row_Array[0] = 88;
            Col_Array[1] = Round_peak.Col;
            Row_Array[1] = Round_peak.Row;

            //清理原来的左边线
            for(pin = 0; pin < MyImage_H + 10; pin++)
            {
                //清除画图,注释掉不影响实际使用
                MyImage[Left_line.Row[pin]][Left_line.Col[pin]] = black_point;
                MyImage[Left_line.Row[pin]][Left_line.Col[pin] + 1] = black_point;
                //清除数值
                Left_line.Row[pin] = none;
                Left_line.Col[pin] = none;
            }
						

            //左边线补线
            Normal_LineFit(Col_Array, Row_Array, Fix_Left);

            //二次寻线
            FindLeftLineTwice(Left_peak.pin - 1);

            return;
        }

        if(Roundabout_State == Round_Turn)
        {
            //填充数值,拉,就摁拉线
            Col_Array[0] = Left_peak.Col;
            Row_Array[0] = Left_peak.Row;
            Col_Array[1] = 125;
            Row_Array[1] = 10;

            if(Left_peak.Col == none)
            {
                return;
            }

            //左边线补线
            Normal_LineFit(Col_Array, Row_Array, Fix_Left);

            //重新计算中线拟合范围
            Left_peak.pin += (Row_Array[0] - Row_Array[1]);

            return;
        }

				if(Roundabout_State == Out_Round)
        {
            //填充数值,拉,就摁拉线
            Col_Array[0] = 20;//10
            Row_Array[0] = 80;
            Col_Array[1] = 105;
            Row_Array[1] = 10;

           
            //左边线补线
            Normal_LineFit(Col_Array, Row_Array, Fix_Left);
					
						Left_Start_flag = 1;

//						Left_peak.pin = (Row_Array[0] - Row_Array[1]);

            return;
        }

				
    }
}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     环岛处理
// 参数说明     无
// 返回参数     void
// 使用示例     Roundabout_handler();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Roundabout_handler(void)
{
    //左环岛处理
    if(Roundabout_DirFlag == LeftRound)
    {

        //刚进入环岛
        if(Roundabout_State == Find_Round)
        {
            //检测到环岛中心
            if(Left_Row_Per <= 0.83)
            {
                //过渡标志
                Roundabout_State = Check_Round;
                return;
            }
						
						 //补线
            Roundabout_FixLine();

            //正常行驶(应该是寻右线直行)
            StraightRace_handler();
						
        }

        //环岛检查,防止误判
        if(Roundabout_State == Check_Round)
        {
            if(Left_Row_Per >= 0.93)
            {
                Roundabout_State = Into_Round;
                return;
            }

            //左边线存在较长且完整的边线,大概在出环,判断错误,退出环岛模式
            if(Left_peak.Row < 20 && Left_line.Row[0] > 70 && Left_line.Row[0] != none)
            {
                Roundabout_State = No_Round;
                Roundabout_DirFlag = NonDir;
                Work_State = No_Duel;
                return;
            }

            //正常行驶
						Roundabout_FixLine();
            StraightRace_handler();
           
        }

        //拉线入环
        if(Roundabout_State == Into_Round)
        {
           

            //出现完整的右边线弧线
            if(Right_line.Row[1] >= 65 && Right_line.Row[1] != none && (Right_peak.Col !=none && Right_peak.Col < (Race_Center - 30)))
            {
                Roundabout_State = Round_Turn;
                return;
            }

						 //补线
            Roundabout_FixLine();
						
            //将左边线起始点标志位清除,使用右边线修正后数值拟合
            Left_Start_flag = 0;
            Right_Start_flag = 1;
            //中线拟合行驶
            StraightRace_handler();
			
          
        }

        //环岛绕弯
        if(Roundabout_State == Round_Turn)
        {
            //
//            if((Right_peak.Row >= 75 && Right_peak.Row != none) || (Left_peak.Row <= 20 && Right_peak.Row <= 20 ))
					    if(Right_peak.Row == none)
            {
                Roundabout_State = Out_Round;
                return;
            }

            //右边线顶点较低,即将丢失右边线
            if(Right_peak.Row > 40 && Right_peak.Row != none)
            {
                //补线
                Roundabout_FixLine();
                //将左边线起始点标志位清除,使用右边线补线后数值拟合中线
                Left_Start_flag = 0;
            }

            //正常行驶
            StraightRace_handler();
           return;	
        }

        //出环
        if(Roundabout_State == Out_Round)
        {
            //重新找到右边线
            if(Right_peak.Row < 25 && (Right_peak.Col >= Race_Center && Right_peak.Col != none))
            {
                //特殊处理结束,标志位清理
                Work_State = No_Duel;
                Roundabout_State = No_Round;
							  Roundabout_DirFlag = NonDir;
                return;
            }
						
						Roundabout_FixLine();
						StraightRace_handler();
//            SpecialServo_flag = 1;
//            Servo_SetAngle(Servo_Middle + L_Servo_Range);

            
        }
    }

    //右环岛处理
    if(Roundabout_DirFlag == RightRound)
    {
        //刚进入环岛
        if(Roundabout_State == Find_Round)
        {
            //检测到环岛中心
            if(Right_Row_Per <= 0.83)
            {
                //过渡标志
                Roundabout_State = Check_Round;
                return;
            }
						 //补线
            Roundabout_FixLine();
            //正常行驶
            StraightRace_handler();
							
            
        }

        //环岛检查,防止误判
        if(Roundabout_State == Check_Round)
        {
            if(Right_Row_Per >= 0.92)
            {
                Roundabout_State = Into_Round;
                return;
            }

            //右边线存在较长且完整的边线,大概在出环,判断错误,退出环岛模式
            if(Right_peak.Row < 20 && Right_line.Row[0] > 70 && Right_line.Row[0] != none)
            {
                Roundabout_State = No_Round;
                Roundabout_DirFlag = NonDir;
                Work_State = No_Duel;
                return;
            }
						
						Roundabout_FixLine();
            //正常行驶
            StraightRace_handler();
        }

        //拉线入环
        if(Roundabout_State == Into_Round)
        {
            
            //出现完整的左边线弧线
            if(Left_line.Row[1] >= 65 && Left_line.Row[3] != none && (Left_peak.Col !=none && Left_peak.Col > Race_Center + 30))
            {
                Roundabout_State = Round_Turn;
                return;
            }
						
						//补线
            Roundabout_FixLine();

            //将右边线起始点标志位清除,使用左边线修正后数值拟合
						Left_Start_flag = 1;
            Right_Start_flag = 0;
            //中线拟合行驶
            StraightRace_handler();
        }

        //环岛绕弯
        if(Roundabout_State == Round_Turn)
        {
//            if((Left_peak.Row >= 75 && Left_peak.Row != none) || (Left_peak.Row <= 20 && Right_peak.Row <= 20 ))
            if(Left_peak.Row == none) 
						{
                Roundabout_State = Out_Round;
                return;
            }

            //左边线顶点较低,即将丢失左边线
            if(Left_peak.Row > 40 && Left_peak.Row != none)
            {
                //补线
                Roundabout_FixLine();
                //将右边线起始点标志位清除,使用左边线补线后数值拟合中线
                Right_Start_flag = 0;
								Left_Start_flag = 1;

            }

            //正常行驶
            StraightRace_handler();
           return;	
        }

        //出环
        if(Roundabout_State == Out_Round)
        {
									
            //重新找到左边线
            if(Left_peak.Row < 25 && (Left_peak.Col <= Race_Center && Left_peak.Col != none))
            {
                //特殊速度处理结束,清理标志位
//                SpecialServo_flag = 0;
                Work_State = No_Duel;
                Roundabout_State = No_Round;
							  Roundabout_DirFlag = NonDir;

                SpecialSpeed_flag = 0;
                return;
            }
						
							Roundabout_FixLine();

						 StraightRace_handler();


        }
    }

}

//================================================圆环处理部分============================================================



//================================================斑马线处理部分============================================================


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     斑马线检查
// 参数说明     min_row         扫描的最低行
// 参数说明     max_row         扫描的最高行
// 参数说明     Needing_num     需要满足的跳变点数量
// 返回参数     uint8_t         满足跳变点需求量的行数
// 使用示例     if(Zebra_Check(55, 65, 4) >= 5)
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8_t Zebra_Check(uint8_t min_row, uint8_t max_row,int8 Needing_num)
{
    uint8_t row = 0,col = 0;
    uint8_t Zebra_Num = 0;          //判定为斑马线的行数
    int8 temp = 0;
    temp = Needing_num;             //一行中有多少跳变才满足

    for(row = min_row; row < max_row; row++)
    {
        for(col = 5; col < MyImage_W - 5; col++)
        {
            if(MyImage[row][col] == white_point && MyImage[row][col - 1] == black_point)
            {
                temp--;
            }

            if(temp <= 0)
            {
                Zebra_Num++;
                break;
            }

        }

        temp = Needing_num;
    }

    return Zebra_Num;
}

//================================================斑马线处理部分============================================================



//================================================箱子处理部分============================================================

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     箱子检查
// 参数说明     无
// 返回参数     void
// 使用示例     Box_Check（）;
// 备注信息     用于冲出赛道保护
//-------------------------------------------------------------------------------------------------------------------
uint8_t Box_Check(void)
{
  if(Box.S>=1700 )//&& X_flag == 0 && Y_flag == 0)
	{
		X_Y_Z_Speed_Set(0,0,0);
		return 1;
	}
	else
	{
		Box.H=0;Box.W=0;
		X_err=0;Y_err=0;
		return 0;
	}		
}


void Box_handler(void)
{
    uint8_t i, white_row = 0;
    int Row, col;
    uint16_t white_num;

		Box_Run();
		
}

//================================================箱子处理部分============================================================



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     全元素检查
// 参数说明     无
// 返回参数     void
// 使用示例     All_Check();
// 备注信息     在主循环中调用
//-------------------------------------------------------------------------------------------------------------------
void All_Check(void)
{
			//箱子检测
		if(Work_State != Dur_Box)
		{
				if(Box_Check()==1)
				{
						Origin_State = Work_State;
						Work_State 	 = Dur_Box;				
						Box_State 	 = Rec_Angle;
//							Box_State = Box_scan;
						X_Y_Z_Speed_Set(0,0,0);					
						//障碍屏幕显示
						ips200_show_string(Col_0, Row_18, "Box       ");
						return;
				}
		}
    if(Work_State == No_Duel)
    {
        
        if(CrossRoad_Check() == 1)
        {
            Work_State = Dur_Cross;
            Cross_State = Before_Turn;
            //十字路口屏幕显示
            ips200_show_string(Col_0, Row_18, "CrossRoad ");
            return;
        }

        if(Roundabout_Check() == 1)
        {
            Work_State = Dur_Round;
            Roundabout_State = Find_Round;
            //环岛屏幕显示
            ips200_show_string(Col_0, Row_18, "Roundabout");
//            ips200_show_uint(140, 296, (unsigned int)Roundabout_DirFlag, 1);
            return;
        }
				
//        if(Zebra_Check(70, 85, 4) >= 5 )
//         {
//            Zebra_flag = 1;
//            return;
//         }

				return;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     各元素处理
// 参数说明     无
// 返回参数     void
// 使用示例     All_handler();
// 备注信息     在主循环中调用
//-------------------------------------------------------------------------------------------------------------------
void All_handler(void)
{
	static int16 err = 0;
    //并没有进入特殊赛段
    if(Work_State == No_Duel)
    {
        //直弯道处理
					StraightRace_handler();
//			  Straight_handler();
//					if(Box.S<1100)
						err=(Center_Line_Col[33])*0.375f + (Center_Line_Col[36])*0.5f + (Center_Line_Col[39])*0.1f - 73.5;
//					else
//					  err=(Center_Line_Col[20])*0.375f + (Center_Line_Col[23])*0.5f + (Center_Line_Col[26])*0.1f - 73.5;

					if(err>=-72 && err<=72)Image_Erro = err;
					else Image_Erro = 0; 
					
				
        //直弯道屏幕显示
        ips200_show_string(Col_0, Row_18, "Straight  ");
        return;
    }

    //进入特殊赛段
    if(Work_State != No_Duel)
    {
			   if(Box_State != No_box)
        {
            //障碍处理
            Box_handler();
					//障碍屏幕显示
					ips200_show_string(Col_0, Row_18, "Box       ");
					ips200_show_uint(Col_11, Row_18, (unsigned int)Box_State, 1);

          return;
        }

			
        //进入十字路口
        if(Cross_State != No_Cross)
        {
            //十字路口处理
            CrossRoad_handler();
//						Intersection_handler();
//						if(Box.S<1100)
							err=(Center_Line_Col[33])*0.375f + (Center_Line_Col[36])*0.5f + (Center_Line_Col[39])*0.1f - 73.5;
//						else
//							err=(Center_Line_Col[20])*0.375f + (Center_Line_Col[23])*0.5f + (Center_Line_Col[26])*0.1f - 73.5;

						if(err>=-72 && err<=72)Image_Erro = err;
						else Image_Erro = 0; 
			

            //显示当前处理状态
					   //十字路口屏幕显示
            ips200_show_string(Col_0, Row_18, "CrossRoad ");
            ips200_show_uint(Col_11, Row_18, (unsigned int)Cross_State, 1);
            return;
        }

        //进入环岛
        if(Roundabout_State != No_Round)
        {
            //环岛处理
            Roundabout_handler();
//						if(Box.S<1100)
							err=(Center_Line_Col[33])*0.375f + (Center_Line_Col[36])*0.5f + (Center_Line_Col[39])*0.1f - 73.5;
//						else
//							err=(Center_Line_Col[20])*0.375f + (Center_Line_Col[23])*0.5f + (Center_Line_Col[26])*0.1f - 73.5;

						if(err>=-72 && err<=72)Image_Erro = err;
						else Image_Erro = 0; 

            //显示当前处理状态
					  //环岛屏幕显示
            ips200_show_string(Col_0, Row_18, "Roundabout");
            ips200_show_uint(Col_11, Row_18, (unsigned int)Roundabout_State, 1);
            return;
        }


//
			}
			
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     小车丢线保护
// 参数说明     无
// 返回参数     void
// 使用示例     Loss_line_Protect();
// 备注信息     在周期中断中调用
//-------------------------------------------------------------------------------------------------------------------
void Loss_line_Protect(void)
{
    uint8_t Loss_Check_Row;
    uint16_t chujie_num = 0;

    for(Loss_Check_Row = 83; Loss_Check_Row < 88; Loss_Check_Row++)
    {
        //从5列到90列进行检查,记录黑点数
        for(int x = 30; x <= 120; x++)
        {
             if(MyImage[Loss_Check_Row][x] == black_point)  //检测图像最下端75行白点数,如果小于一定值,判断为出界
             {
                 chujie_num++;
             }
        }
    }

    if(chujie_num > 400)
    {
        Stop_flag = 1;
    }
		
		else
		{
			Stop_flag = 0;
		}
//	ips200_show_uint(Row_23,Col_17,chujie_num,4);//出界点

}



//================================================边线元素处理部分============================================================

///**@brief   斑马线扫描
//-- @param   uint8(*Bin_Image)[Image_W] 二值化图像
//-- @author  陈锦辉
//-- @date    2024/6/5
//**/
//void Zebra_Seek(uint8(*Bin_Image)[Image_W],uint8* L_Border, uint8* R_Border, uint16 Total_Num_L, uint16 Total_Num_R)
//{
//    uint8 total = 0;
//    uint16 Right_Straight = 0;
//    uint16 Left_Straight = 0;
//    Right_Straight = Straight_Line_Judge(R_Border, Total_Num_R - 10, RightLine);//判断右边是否为长直线
//    Left_Straight = Straight_Line_Judge(L_Border, Total_Num_L - 10, LeftLine);//判断左边是否为长直线

//    if(Left_Straight || Right_Straight)
//    {
//        for(uint8 i = 50;i <= 120;i++)
//        {
//            if(Bin_Image[45][i] == Black_Pixel && Bin_Image[45][i+1] == White_Pixel)
//            {
//                total ++;
//            }
//        }
//    }

//    if((total >= 5))
//    {
//        total = 0;
//        Image_Flag.Zerba = true;
//    }
//}

//#define RESULT_ROW 100 //结果图的行列
//#define RESULT_COL 114
//#define         USED_ROW                120  //用于变换图的行列
//#define         USED_COL                188
//uint8_t *PerImg_ip[RESULT_ROW][RESULT_COL];
//#define PER_IMG     mt9v03x_image_dvp//mt9v03x_image_dvp:用于透视变换的图像 也可以使用二值化图
//#define ImageUsed   *PerImg_ip//*PerImg_ip定义使用的图像，ImageUsed为用于巡线和识别的图像
//static uint8_t BlackColor = 255;	//无内容部分像素值
///******************变换参数******************************/
////去畸变参数
//double cameraMatrix[3][3] = {{296.482019, 0.000000,   152.664982},
//                             {0.000000,   286.375269, 104.540031},
//                             {0.000000,   0.000000,   1.000000}};
//double distCoeffs[5] = {-0.459946, 0.283675, 0.002304, 0.002566, -0.109265};
//int move_xy[2] = {10, 0};
////逆透视参数
//double change_un_Mat[3][3] = {{2.936703,  0.314530, -60.814898},
//                              {-0.263326, 2.308885, -108.340381},
//                              {-0.000835, 0.000896, 0.969316}};
///*******************************************************/
//void find_xy(int x, int y, int local[2]) {
//    double fx = cameraMatrix[0][0]
//    , fy = cameraMatrix[1][1]
//    , ux = cameraMatrix[0][2]
//    , uy = cameraMatrix[1][2]
//    , k1 = distCoeffs[0]
//    , k2 = distCoeffs[1]
//    , k3 = distCoeffs[4]
//    , p1 = distCoeffs[2]
//    , p2 = distCoeffs[3];
//    double xCorrected = (x - ux) / fx;
//    double yCorrected = (y - uy) / fy;
//    double xDistortion, yDistortion;
//    double r2 = xCorrected * xCorrected + yCorrected * yCorrected;
//    double deltaRa = 1. + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2;
//    double deltaRb = 1 / (1.);
//    double deltaTx = 2. * p1 * xCorrected * yCorrected + p2 * (r2 + 2. * xCorrected * xCorrected);
//    double deltaTy = p1 * (r2 + 2. * yCorrected * yCorrected) + 2. * p2 * xCorrected * yCorrected;
//    xDistortion = xCorrected * deltaRa * deltaRb + deltaTx;
//    yDistortion = yCorrected * deltaRa * deltaRb + deltaTy;
//    xDistortion = xDistortion * fx + ux;
//    yDistortion = yDistortion * fy + uy;
//    if (yDistortion >= 0 && yDistortion < USED_ROW && xDistortion >= 0 && xDistortion < USED_COL) {
//        local[0] = (int) yDistortion;
//        local[1] = (int) xDistortion;
//    } else {
//        local[0] = -1;
//        local[1] = -1;
//    }
//}

//void find_xy1(int x, int y, int local[2]) {
//    int local_x = (int) ((change_un_Mat[0][0] * x
//                          + change_un_Mat[0][1] * y + change_un_Mat[0][2])
//                         / (change_un_Mat[2][0] * x + change_un_Mat[2][1] * y
//                            + change_un_Mat[2][2]));
//    int local_y = (int) ((change_un_Mat[1][0] * x
//                          + change_un_Mat[1][1] * y + change_un_Mat[1][2])
//                         / (change_un_Mat[2][0] * x + change_un_Mat[2][1] * y
//                            + change_un_Mat[2][2]));
//    if (local_x
//        >= 0 && local_y >= 0) {
//        local[0] = local_y;
//        local[1] = local_x;
//    } else {
//        local[0] = -1;
//        local[1] = -1;
//    }
//}

//void ImageChange_Init() {
//    for (int i = 0; i < RESULT_ROW; i++) {
//        for (int j = 0; j < RESULT_COL; j++) {
//            int local_xy[2] = {-1};
//            find_xy1(j, i, local_xy);
//            if (local_xy[0] != -1 && local_xy[0] != -1) {
//                int local_xy1[2] = {-1};
//                find_xy(local_xy[1] - move_xy[0], local_xy[0] - move_xy[1], local_xy1);
//                if (local_xy1[0] != -1 && local_xy1[1] != -1) {
//                    PerImg_ip[i][j] = &mt9v03x_image_dvp[local_xy1[0]][local_xy1[1]];
//                } else PerImg_ip[i][j] = &BlackColor;
//            } else PerImg_ip[i][j] = &BlackColor;
//        }
//    }
//}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     二值化图像滤波
// 参数说明     无
// 返回参数     void
// 使用示例     Image_BinFilter();
// 备注信息     三黑点三白点感染滤波(?)
//-------------------------------------------------------------------------------------------------------------------
void Image_BinFilter(void)
{
    int Col, Row;
    for(Row = 3; Row < MyImage_H - 2; Row+=2)
    {
			for(Col = 3; Col < MyImage_W - 2; Col+=2)
			{
            if(MyImage[Row][Col] == black_point)   
            {
							 MyImage[Row-1][Col+1] = black_point;                         //把白点变成黑点
							 MyImage[Row-1][Col] = black_point;                         //把白点变成黑点
							 MyImage[Row-1][Col-1] = black_point;                         //把白点变成黑点
               MyImage[Row+1][Col-1] = black_point;                         //把白点变成黑点
               MyImage[Row+1][Col] = black_point;                         //把白点变成黑点
               MyImage[Row+1][Col+1] = black_point;                         //把白点变成黑点
							 MyImage[Row][Col-1] = black_point;                         //把白点变成黑点
							 MyImage[Row][Col+1] = black_point;                         //把白点变成黑点

            }
						else if(MyImage[Row][Col] == white_point)
						{
							 MyImage[Row-1][Col] = white_point;                         //把白点变成黑点
							 MyImage[Row+1][Col] = white_point;                         //把白点变成黑点
							 MyImage[Row][Col-1] = white_point;                         //把白点变成黑点
							 MyImage[Row][Col+1] = white_point;                         //把白点变成黑点
						}
			}

     }
}


//----------------------------------------------Sobel---------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     寻找左边线起点
// 参数说明     void
// 返回参数     void
// 使用示例     LeftStartFind();
// 备注信息     寻找边线辅助用
//-------------------------------------------------------------------------------------------------------------------
void LeftFind(void)
{
    int row,col;

    //寻找左边线起点(此处条件为找到第一个符合的点就停止)
    for(row = MyImage_H - 2; row > (MyImage_H / 2) && Left_line.Row[0] == none; row--)
    {
      for(col =2 ; col < Race_Center + 10; col++)
      {
          if(MyImage[row][col-2] == black_point && MyImage[row][col-1] == black_point)
					{
								//再找到一个白点
							if(MyImage[row][col] == white_point)
							{
								 //发现左边线起始点,进行记录
								 Left_line.Row[0]= row;
								 Left_line.Col[0]= col;
								 Left_Start_flag = 1;
								 //给图像上色
								 MyImage[row][col - 1] = red_point;
								 MyImage[row][col - 2] = red_point;


								
								 break;
							}
					}
        
      }
    }

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     寻找右边线起点
// 参数说明     void
// 返回参数     void
// 使用示例     RightStartFind();
// 备注信息     寻找边线辅助用
//-------------------------------------------------------------------------------------------------------------------
void RightFind(void)
{
  int row,col;

  //寻找右边线起点(此处条件为找到第一个符合的点就停止)
  for(row = MyImage_H - 2; row > (MyImage_H / 2) && Right_line.Row[0] == none; row--)
  {
    for(col = MyImage_W - 2 ; col > Race_Center - 10; col--)
    {
        if(MyImage[row][col+2] == black_point && MyImage[row][col+1] == black_point)
				{
            //再找到一个白点
          if(MyImage[row][col] == white_point)
          {
             //发现左边线起始点,进行记录
             Right_line.Row[0]= row;
             Right_line.Col[0]= col;
             Right_Start_flag = 1;
             //给图像上色
             MyImage[row][col + 1] = red_point;
             MyImage[row][col + 2] = red_point;

					
					}
				}
    }
  }

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取图像左边线
// 参数说明     无
// 返回参数     void
// 使用示例     GetLeftSide();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void GetLeft(void)
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
        colmin = Left_line.Col[pin - 1] - 10;
        colmax = Left_line.Col[pin - 1] + 8;

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
//                    //给图像上色
                    MyImage[row][col - 1] = red_point;
                    MyImage[row][col - 2] = red_point;
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
    if(L_lenth <= 5)
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
void GetRight(void)
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
        colmin = Right_line.Col[pin - 1] - 8;
        colmax = Right_line.Col[pin - 1] + 10;


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
                    //给图像上色
                    MyImage[row][col + 1] = red_point;
                    MyImage[row][col + 2] = red_point;
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
    if(R_lenth <= 5)
    {
        Right_Start_flag = 0;
        Right_line.Row[0] = none;
        Right_line.Col[0] = none;
    }

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取图像左右边线
// 参数说明     无
// 返回参数     void
// 使用示例     GetLRSide();
// 备注信息7
//-------------------------------------------------------------------------------------------------------------------
void GetLR(void)
{

    //寻找起点
    LeftFind();
    RightFind();
//    //寻找边线
    GetLeft();
    GetRight();
	
	
//    //左右边线方差计算
//    L_S = S_Calculate(0);
//    R_S = S_Calculate(1);

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     左边线顶点
// 参数说明     无
// 返回参数     void
// 使用示例     LeftSide_Peak();
// 备注信息     严格上来讲是寻找左边线列(Col),即x最大的点
//-------------------------------------------------------------------------------------------------------------------
void Left_Peak(void)
{

    int temp, pin, col_max = 0;

    //如果不存在左边线起始点
    if(Left_line.Row[0] == none)
    {
        return;
    }

    for(pin = 0; pin < MyImage_H - 2; pin++)
    {
        //若该点无数据
        if(Left_line.Row[pin] == none)
        {
            break;
        }

        //打擂台,寻找左边线列最大处点
        if(col_max <= Left_line.Col[pin])
        {
            col_max = Left_line.Col[pin];
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
void Right_Peak(void)
{
    int temp, pin, col_min = MyImage_W;

    //如果不存在右边线起始点
    if(Right_line.Row[0] == none)
    {
        return;
    }

    for(pin = 0; pin < MyImage_H -2; pin++)
    {
        //若该点无数据
        if(Right_line.Row[pin] == none)
        {
            break;
        }

        //打擂台,寻找右边线列最小处点
        if(col_min >= Right_line.Col[pin])
        {
            col_min = Right_line.Col[pin];
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
void Side_Peak(void)
{
    Left_Peak();
    Right_Peak();
	
	
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     直线处理
// 参数说明     无
// 返回参数     void
// 使用示例     StraightRace_handler();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void Straight_handler(void)
{
	  int Row;
    int pin=1;
	

	
    //两边线起始点都不存在,直接返回
    if(Left_Start_flag == 0 && Right_Start_flag == 0)
    {

        return;
    }
//		//找到双线
    else if(Left_Start_flag == 1 && Right_Start_flag == 1)
		{

							//左边线顶点比较高
					if(Left_peak.pin >= Right_peak.pin + 20)
					{

							for(Row = Left_line.Row[0] - pin; pin < MyImage_H-30; pin++)
							{
										Row = Left_line.Row[0] - pin;
										Center_Line_Row[pin] = Row;																						 //y
										Center_Line_Col[pin] = ((Left_line.Row[0] - pin)/2 + Left_line.Col[pin]);
								
										MyImage[Center_Line_Row[pin]][Center_Line_Col[pin]] = white_point;
											
							}
							return;				

					}
					//右边线顶点比较高
					else if(Left_peak.pin + 20 <= Right_peak.pin)
					{
							for(Row = Right_line.Row[0] - pin; pin < MyImage_H-30; pin++)
							{
										Row = Right_line.Row[0] - pin;
										Center_Line_Row[pin] = Row;																						 //y
										Center_Line_Col[pin] = ( Right_line.Col[pin]-(Right_line.Row[0] - pin)/2);
								
										MyImage[Center_Line_Row[pin]][Center_Line_Col[pin]] = white_point;											
							}
							return;
					}
					else
					{
							for(Row = Left_line.Row[0] - pin; pin < MyImage_H-30; pin++)
							{
										Row = Left_line.Row[0] - pin;
										Center_Line_Row[pin] = Row;																						 //y
										Center_Line_Col[pin] = (Right_line.Col[pin] + Left_line.Col[pin])>>1;				
										MyImage[Center_Line_Row[pin]][Center_Line_Col[pin]] = white_point;	
							}			
						return;
					}
		}
		//仅找到左线
		else if(Left_Start_flag == 1 && Right_Start_flag == 0)
		{
        for(Row = Left_line.Row[0] - pin; pin < MyImage_H-30; pin++)
        {
							Row = Left_line.Row[0] - pin;
							Center_Line_Row[pin] = Row;																						 //y
							Center_Line_Col[pin] = ((Left_line.Row[0] - pin)/2+Left_line.Col[pin]);
					
							MyImage[Center_Line_Row[pin]][Center_Line_Col[pin]] = white_point;
											
				}
				return;
		}
		//仅找到右线
		else if(Left_Start_flag == 0 && Right_Start_flag == 1)
		{
        for(Row = Right_line.Row[0] - pin; pin < MyImage_H-30; pin++)
        {
							Row = Right_line.Row[0] - pin;
							Center_Line_Row[pin] = Row;																						 //y
							Center_Line_Col[pin] = (Right_line.Col[pin] - (Right_line.Row[0] - pin)/2);				
							MyImage[Center_Line_Row[pin]][Center_Line_Col[pin]] = white_point;


								
				}
				return;
		}
		

		
							

}



//----------------------------------------------Sobel---------------------------------------------------------

