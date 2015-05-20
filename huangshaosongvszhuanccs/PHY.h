#ifndef _PHY_H_
#define _PHY_H_

#define FrameNum 1   //仿真设定传输的帧数为1帧
#define PLCP_H_LEN 31
#define PLCP_H_SP_LEN (PLCP_H_LEN*PLCP_H_SpreadFactor)
#define PLCP_P_LEN 90
#define PLCP_LEN (PLCP_H_SP_LEN+PLCP_P_LEN) //扩频后的PLCP_header和PLCP Preamble的长度和
#define PSDU_SP_LEN (PSDU_LEN*PSDU_SpreadFactor)
#define PPDU_LEN (PLCP_LEN+PSDU_LEN)
#define PI 3.1415926
#define uc unsigned char

#endif
