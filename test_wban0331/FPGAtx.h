#ifndef _FPGATX_H_
#define _FPGATX_H_
/*
typedef struct
{
	double re;
	double im;
}complex;*/
#include "complex.h"
unsigned short int *FPGAtxfun(complex *PPDU_Mod, int length);
void FPGArxfun(Uint16 *buf);


#endif
