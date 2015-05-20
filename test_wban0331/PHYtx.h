#ifndef _PHYTX_H_
#define _PHYTX_H_
#define uc unsigned char
#include "complex.h"
#include "Wban.h"

complex *PHYtx(unsigned char *PSDU_MAC, WORD length,int rate);
#endif
