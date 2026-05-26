#ifndef __ANGLEMATH_H_
#define __ANGLEMATH_H_
#include "zf_common_headfile.h"

//角度函数
float my_sin(float x, int n);
float my_cos(float x, int n);
float my_tan(float x,int n);
float arctan(float x);
float arcsin(float x);
 
//开根
float Q_rsqrt(float number);
int GFP_abs(int p);

#endif
