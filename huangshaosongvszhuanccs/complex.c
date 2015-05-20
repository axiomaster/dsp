//���帴��������
#include<math.h>
typedef struct
{
	double re;
	double im;
}complex;
//�õ�һ������
complex fget(double a)
{
	complex z;
	z.re=cos(a);
	z.im=sin(a);
	return z;
}
//�����ӷ�
complex fadd(complex z1,complex z2)
{
	complex z;
	z.re=z1.re+z2.re;
	z.im=z1.im+z2.im;
	return(z);
}

//��������
complex fsub(complex z1,complex z2)
{
	complex z;
	z.re=z1.re-z2.re;
	z.im=z1.im-z2.im;
	return(z);
}

//�����˷�
complex fmul(complex z1,complex z2)
{
	complex z;
	z.re=(z1.re*z2.re)-(z1.im*z2.im);
	z.im=(z1.re*z2.im)+(z1.im*z2.re);
	return(z);
}

//��������
complex fdiv(complex z1,complex z2)
{
	complex z;
	z.re=((z1.re*z2.re)+(z1.im*z2.im))/(z2.re*z2.re+z2.im*z2.im);
	z.im=((z1.im*z2.re)-(z1.re*z2.im))/(z2.re*z2.re+z2.im*z2.im);
	return(z);
}
//�����Ĺ���
complex fconj(complex a)
{
	complex z;
	z.re=a.re;
	z.im=-a.im;
	return z;
}
