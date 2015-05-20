#ifndef _BCH_H_
#define _BCH_H_
#define uc unsigned char

int PSDUcodelen(int N_psdu,int M,int SpreadFactor_R);
uc* BCHEncoding_PSDU(uc* msg_in,int M);
uc* BCHDecoding_PSDU(uc* code_in,int N_psdu_code,int M);
void BCHEncoding_PHYH(uc* data);
uc* BCHDecoding_PHYH(uc* data_in);

#endif
