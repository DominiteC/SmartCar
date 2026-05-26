#include "AngleMath.h"

/* 2分之PI */
const float PI_2 = 1.570796f;
 
/**----------------------------------------------------数学公式------------------------------------------------------------------------**/
 
/*绝对值函数*/
int GFP_abs(int p)
{
	int q;
	q=p>0?p:(-p);
	return q;
}
 
/**
 * @brief       sin函数
 * @param       x: 璁＄畻鐨勫姬搴?
 * @param       n: 娉板嫆绾ф暟鐨勯」鏁?
 * @retval      sin鍑芥暟鏁板??
 */
float my_sin(float x,int n) 
{
    float term = x; // 绗竴椤规槸 x
    float sin_x = 0.0; // sin(x)鐨勭疮鍔犵粨鏋?
 
    if(x == 0)
    {
        return 0;
    }

    for (int i = 1; i <= n; i++) {
        sin_x += term; // 绱姞褰撳墠椤?
        term *= -x * x / ((2 * i) * (2 * i + 1)); // 璁＄畻涓嬩竴椤?
    }
 
    return sin_x;
}
 
/**
 * @brief       鍒╃敤娉板嫆绾ф暟璁＄畻cos(x)鐨勮繎浼煎??
 * @param       x: 璁＄畻鐨勫姬搴?
 * @param       n: 娉板嫆绾ф暟鐨勯」鏁?
 * @retval      cos鍑芥暟鏁板??
 */
float my_cos(float x,int n) 
{
		return my_sin(PI/2 + x,n);//濂囧彉鍋朵笉鍙橈紝绗﹀彿鐪嬭薄闄?
}

float my_tan(float x,int n)
{
    return my_sin(x, n) / my_cos(x, n);
}
 
/**
 * @brief       鍒╃敤鍙嶆鍒囬害鍏嬪姵鏋楀睍寮?寮忔眰瑙rctan
 * @param       x: 璁＄畻鐨勬暟鍊硷紝鑼冨洿锛?-1锛?1锛?
 * @retval      arctan鍑芥暟姹傝В鐨勫姬搴?
 * @note        闃舵暟瓒婇珮锛屽?艰秺鍑嗙‘   70掳浠ュ唴鏄噯纭殑
 */
float arctan(float x)  
{
	float t = x;
	float result = 0;
	float X2 = x * x;
	unsigned char cnt = 1;
	
	do{
		result += t / ((cnt << 1) - 1);
		t = -t;
		t *= X2;
		cnt++;
	}while(cnt <= 6); //浠呰绠楀墠6椤?
	
	return result;
}
 
/**
 * @brief       鍒╃敤鍙嶆鍒囬害鍏嬪姵鏋楀睍寮?寮忔眰瑙rcsin
 * @param       x: 璁＄畻鐨勬暟鍊硷紝鑼冨洿锛?-1锛?1锛?
 * @retval      arcsin鍑芥暟姹傝В鐨勫姬搴?
 * @note        闃舵暟瓒婇珮锛屽?艰秺鍑嗙‘   42掳浠ュ唴鏄噯纭殑
 */
float arcsin(float x)  
{
	float d=1;
	float t=x;
	unsigned char cnt = 1;
	float result = 0;	
	float X2 = x*x;
	
	if (x >= 1.0f) 
	{
		return PI_2;
	}
	if (x <= -1.0f) 
	{
		return -PI_2;
	}
	do
	{
		result += t / (d * ((cnt << 1) - 1));
		t *= X2 * ((cnt << 1) - 1);//
		d *= (cnt << 1);//2 4 6 8 10 ...
		cnt++;
	}while(cnt <= 6);
 
	return result;
}
 
/**
 * @brief       蹇?熻绠楀钩鏂规牴鍊掓暟鏁板??
 * @param       x: 璁＄畻鐨勫姬搴?
 * @retval      鐩爣鏁板??
 */
float Q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;
 
	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                      
	i  = 0x5f3759df - ( i >> 1 );               
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration 锛堢涓?娆＄墰椤胯凯浠ｏ級
	return y;
}


/**@brief    求绝对值
-- @param    int value 输入的值
-- @return   value 绝对值
-- @author   none
-- @date     2023/10/2
**/
int My_Abs(int value)
{
    if (value >= 0) return value;
    else return -value;
}

/**@brief    限幅函数
-- @param    int16_t x 需要限幅的值
-- @param    int a 最大值
-- @param    int b 最小值
-- @return   x 输出
-- @author   none
-- @date     2023/10/2
**/
int16_t Limit_a_b(int16_t x, int a, int b)
{
    if (x < a) x = a;
    if (x > b) x = b;
    return x;
}

/**@brief    求x,y中的最小值
-- @param    int16_t x x值
-- @param    int16_t y y值
-- @return   返回两值中的最小值
-- @author   none
-- @date     2023/10/2
**/
int16_t Limit1(int16_t x, int16_t y)
{
    if (x > y)             return y;
    else if (x < -y)       return -y;
    else                return x;
}

/**
* @brief 最小二乘法
* @param uint8 begin				输入起点
* @param uint8 end					输入终点
* @param uint8 *border				输入需要计算斜率的边界首地址
*  @see CTest		Slope_Calculate(start, end, border);//斜率
* @return 返回说明
*     -<em>false</em> fail
*     -<em>true</em> succeed
*/
float Slope_Calculate(uint8 begin, uint8 end, uint8* border)
{
    float xsum = 0, ysum = 0, xysum = 0, x2sum = 0;
    int16_t i = 0;
    float result = 0;
    static float resultlast;

    for (i = begin; i < end; i++)
    {
        xsum += i;
        ysum += border[i];
        xysum += i * (border[i]);
        x2sum += i * i;

    }
    if ((end - begin) * x2sum - xsum * xsum) //判断除数是否为零
    {
        result = ((end - begin) * xysum - xsum * ysum) / ((end - begin) * x2sum - xsum * xsum);
        resultlast = result;
    }
    else
    {
        result = resultlast;
    }
    return result;
}

/**
* @brief 计算斜率截距
* @param uint8 start				输入起点
* @param uint8 *border				输入需要计算斜率的边界
* @param uint8 end					输入终点
* @param float *slope_rate			输入斜率地址
* @param float *intercept			输入截距地址
*  @see CTest		calculate_s_i(start, end, R_Border, &slope_l_rate, &intercept_l);
* @return 返回说明
*     -<em>false</em> fail
*     -<em>true</em> succeed
*/
void calculate_s_i(uint8 start, uint8 end, uint8* border, float* slope_rate, float* intercept)
{
    uint16 i, num = 0;
    uint16 xsum = 0, ysum = 0;
    float y_average, x_average;

    num = 0;
    xsum = 0;
    ysum = 0;
    y_average = 0;
    x_average = 0;
    for (i = start; i < end; i++)
    {
        xsum += i;
        ysum += border[i];
        num++;
    }

    //计算各个平均数
    if (num)
    {
        x_average = (float)(xsum / num);
        y_average = (float)(ysum / num);

    }

    /*计算斜率*/
    *slope_rate = Slope_Calculate(start, end, border);//斜率
    *intercept = y_average - (*slope_rate) * x_average;//截距
}

/**@brief    求两点斜率和截距
-- @param    uint8 x1 
-- @param    int16 y y值
-- @return   返回两值中的最小值
-- @author   none
-- @date     2023/10/2
**/
bool Get_K_b(uint8 x1,uint8 y1,uint8 x2,uint8 y2, float* slope_rate, float* intercept)
{
    if (x1 == x2) {
        return false;
    }

    // 计算斜率
    *slope_rate = (float)(y2 - y1) / (x2 - x1);

    // 计算截距
    *intercept = y1 - (*slope_rate * x1);

    return true;
}
