#ifndef _PHYHEADCRC_H_
#define _PHYHEADCRC_H_
#define uc unsigned char

void PHYHeadCRCCoding(uc* data);
int PHYHeadCRCCheck(uc* data,int N);

#endif
