#ifndef _COMPLEX_H_
#define _COMPLEX_H_

typedef struct
{
	double re;
	double im;
}complex;

complex fget(double a);
complex fadd(complex z1,complex z2);
complex fsub(complex z1,complex z2);
complex fmul(complex z1,complex z2);
complex fdiv(complex z1,complex z2);
complex fconj(complex a);

#endif



