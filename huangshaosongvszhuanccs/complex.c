//定义复数计算类
#include<math.h>
typedef struct
{
	double re;
	double im;
}complex;
//得到一个复数
complex fget(double a)
{
	complex z;
	z.re=cos(a);
	z.im=sin(a);
	return z;
}
//复数加法
complex fadd(complex z1,complex z2)
{
	complex z;
	z.re=z1.re+z2.re;
	z.im=z1.im+z2.im;
	return(z);
}

//复数减法
complex fsub(complex z1,complex z2)
{
	complex z;
	z.re=z1.re-z2.re;
	z.im=z1.im-z2.im;
	return(z);
}

//复数乘法
complex fmul(complex z1,complex z2)
{
	complex z;
	z.re=(z1.re*z2.re)-(z1.im*z2.im);
	z.im=(z1.re*z2.im)+(z1.im*z2.re);
	return(z);
}

//复数除法
complex fdiv(complex z1,complex z2)
{
	complex z;
	z.re=((z1.re*z2.re)+(z1.im*z2.im))/(z2.re*z2.re+z2.im*z2.im);
	z.im=((z1.im*z2.re)-(z1.re*z2.im))/(z2.re*z2.re+z2.im*z2.im);
	return(z);
}
//求复数的共轭
complex fconj(complex a)
{
	complex z;
	z.re=a.re;
	z.im=-a.im;
	return z;
}
