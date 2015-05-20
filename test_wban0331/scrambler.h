#ifndef _SCRAMBLER_H_
#define _SCRAMBLER_H_
#define uc unsigned char

void Scrambler(uc* data,int N,int S_seed);
void Scrambler_Soft(double* softInfo,int len,int S_seed,int M);
#endif
