#ifndef _DPSK_H_
#define _DPSK_H_
#define uc unsigned char


double CalcPhase(uc* BitVector,int M);
complex* DPSK_Mod(uc* BitStream,int LEN,int M);
double* DPSK_DeMod_SoftInfo(complex* Data_Mod_re,int LEN,int M);
uc* DPSK_DeMod_SoftDec(double* D_ang,int len,int M);

#endif



