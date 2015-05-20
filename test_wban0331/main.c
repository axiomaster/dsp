/*
 * main.c
 */
#include "WbanMAC.h"
#include "WbanTasks.h"
#include "CpuTimers.h"
#include "WbanPHY.h"
#include "Wban.h"
#include "hardware.h"



#define MAC_TEST_1002

extern SPIBuffer_ControlInfo mac_spiRxInfo;
extern BYTE mac_SPIBuffer_Rx[mac_SPI_BUFFER_SIZE];
extern BYTE mac_RxBuffer[mac_PACKET_BUFFER_SIZE];
extern WORD mac_rxBufferlen;
extern BYTE Nonce_I[16];
extern BYTE Nonce_R[16];
extern BYTE PTK_KMAC_2B[8];
extern BYTE PTK_KMAC_3A[8];
extern BYTE messageNumber;
extern CURRENT_PACKET mac_currentPacket;
WORD mac_n;

void main(void) {
	psc();     //电源管理
	pinmux();  //管脚复用
	PLL0_init( CORE_FREQ_300M );       //pll初始化
	SPI1_init();
	SPI0_init();

	//初始化MAC
	MACInit();
#ifdef DEBUG
 	printf("***********************************************\n");
#endif

 	/*Uint8 buf[21] = {0xf0, 0x13, 0x3, 0x0, 0x3, 0x2, 0x0, 0x20, 0x0, 0x0, 0xa, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9 };
 	spiflash_cycletx(buf, 21);*/


#ifdef MAC_TEST_1002 //三板测试 HUB端
 	mac_TasksPending.bit.dataIndication = 1;
#endif

#ifdef MAC_TEST_1001    //三板测试 NODE端模拟之前的组网过程
 	macBIB.band = WBAN_BAND;
 	macBIB.channelNumber = 0;
 	macBIB.scramblerSeed = SCRAMBLER_SEED_0;
 	macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
 	macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
 	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
 	macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
 	macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
 	macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
 	macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
 	macBIB.BeaconStartTime.Val = 0;			//660000;
#endif


 	//mac_TasksPending.bit.dataIndication = 1;
#ifdef MAC_TEST_0003
	//MAC_TEST_0003
	//NODE终端MAC层的数据CSMA/CA发送
	WORD payloadLength = 15;	//负载长度
	mac_SPIBuffer_Rx[0] = (BYTE)((payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE)(payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MACS_DATA_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//RecipientID
	mac_SPIBuffer_Rx[4] = 0x02;		//BANID
	mac_SPIBuffer_Rx[5] = 0x00;		//DSN
	mac_SPIBuffer_Rx[6] = mac_N_ACK;
	mac_SPIBuffer_Rx[7] = mac_DATA_FRAME_USER_PRIORITY_2;
	//mac_SPIBuffer_Rx[8] = DATA_RATE_242_9;
	//mac_SPIBuffer_Rx[8] = DATA_RATE_971_4;
	mac_SPIBuffer_Rx[8] = DATA_RATE_121_4;
	mac_SPIBuffer_Rx[9] = mac_LEVEL0;
	mac_SPIBuffer_Rx[10] = (BYTE)(((payloadLength - 10) >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[11] = (BYTE)((payloadLength - 10) & 0x00FF);
	for(mac_n = 12; mac_n < (payloadLength + 2); mac_n++)
		mac_SPIBuffer_Rx[mac_n] = mac_n - 12;

	macBIB.band = WBAN_BAND;
	macBIB.channelNumber = 0;
	macBIB.scramblerSeed = SCRAMBLER_SEED_0;
	macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
	macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
	macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
	macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
	macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
	macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
	macBIB.BeaconStartTime.Val = 0;			//660000;
#endif

#ifdef MAC_TEST_0004
	//MAC_TEST_0004
	//HUB终端MAC层的数据接收，不需要ACK
	mac_rxBufferlen = 10;
	mac_RxBuffer[0] = 0x00;	//N-ACK 0x00; I-ACK 0x02;
	mac_RxBuffer[1] = 0x22;	//优先级为2
	mac_RxBuffer[2] = 0x00;
	mac_RxBuffer[3] = 0x00;		//控制分片 0x09为分片序号1且不是最后的分片  0x02为分片序号2且是最后的分片 0x00表示不 分片
	mac_RxBuffer[4] = macBIB.abbrAddr.v;
	mac_RxBuffer[5] = 0x03;
	mac_RxBuffer[6] = 0x02;
	for(mac_n = 7; mac_n < (mac_rxBufferlen - 2); mac_n++)
		mac_RxBuffer[mac_n] = 0xFF;
	mac_RxBuffer[mac_n] = mac_FCS_LSB;
	mac_RxBuffer[mac_n + 1] = mac_FCS_MSB;

	mac_TasksPending.bit.dataIndication = 1;
#endif

#ifdef MAC_TEST_0005
	//MAC_TEST_0005
	//HUB终端MAC层的数据接收，需要ACK
	mac_rxBufferlen = 10;
	mac_RxBuffer[0] = 0x02;	//N-ACK 0x00; I-ACK 0x02;
	mac_RxBuffer[1] = 0x22;	//优先级为2
	mac_RxBuffer[2] = 0x00;
	mac_RxBuffer[3] = 0x00;		//控制分片 0x09为分片序号1且不是最后的分片  0x02为分片序号2且是最后的分片 0x00表示不 分片
	mac_RxBuffer[4] = macBIB.abbrAddr.v;
	mac_RxBuffer[5] = 0x03;
	mac_RxBuffer[6] = 0x02;
	for(mac_n = 7; mac_n < (mac_rxBufferlen - 2); mac_n++)
		mac_RxBuffer[mac_n] = 0xFF;
	mac_RxBuffer[mac_n] = mac_FCS_LSB;
	mac_RxBuffer[mac_n + 1] = mac_FCS_MSB;

	mac_TasksPending.bit.dataIndication = 1;
#endif
#ifdef MAC_TEST_0006
	//MAC_TEST_0006
	//NODE终端MAC层的数据CSMA/CA发送，收到ACK
	macBIB.abbrAddr.v = 0x03;
	macBIB.hubAddr.v = 0x02;
	macBIB.BANID.v = 0x02;

	WORD payloadLength = 11;	//负载长度
	mac_SPIBuffer_Rx[0] = (BYTE)((payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE)(payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MACS_DATA_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//RecipientID
	mac_SPIBuffer_Rx[4] = 0x02;		//BANID
	mac_SPIBuffer_Rx[5] = 0x00;		//DSN
	mac_SPIBuffer_Rx[6] = mac_I_ACK;
	mac_SPIBuffer_Rx[7] = mac_DATA_FRAME_USER_PRIORITY_2;
	mac_SPIBuffer_Rx[8] = DATA_RATE_121_4;
	mac_SPIBuffer_Rx[9] = mac_LEVEL0;
	mac_SPIBuffer_Rx[10] = (BYTE)(((payloadLength - 10) >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[11] = (BYTE)((payloadLength - 10) & 0x00FF);
	for(mac_n = 12; mac_n < (payloadLength + 2); mac_n++)
		mac_SPIBuffer_Rx[mac_n] = 0xFF;

	macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
	macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
	macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
	macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
	macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
	macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
	macBIB.BeaconStartTime.Val = 1000;

	mac_rxBufferlen = 9;
	mac_RxBuffer[0] = 0x00;	//N-ACK 0x00; I-ACK 0x02;
	mac_RxBuffer[1] = 0x10;	//优先级为2
	mac_RxBuffer[2] = 0x00;
	mac_RxBuffer[3] = 0x00;		//控制分片 0x09为分片序号1且不是最后的分片  0x02为分片序号2且是最后的分片 0x00表示不 分片
	mac_RxBuffer[4] = macBIB.abbrAddr.v;
	mac_RxBuffer[5] = 0x02;
	mac_RxBuffer[6] = 0x02;
	mac_RxBuffer[7] = mac_FCS_LSB;
	mac_RxBuffer[8] = mac_FCS_MSB;
#endif
#ifdef MAC_TEST_0007
	//MAC_TEST_0007
	//NODE终端MAC层的数据CSMA/CA发送，没收到ACK
	macBIB.abbrAddr.v = 0x03;
	macBIB.hubAddr.v = 0x02;
	macBIB.BANID.v = 0x02;

	WORD payloadLength = 11;	//负载长度
	mac_SPIBuffer_Rx[0] = (BYTE)((payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE)(payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MACS_DATA_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//RecipientID
	mac_SPIBuffer_Rx[4] = 0x02;		//BANID
	mac_SPIBuffer_Rx[5] = 0x00;		//DSN
	mac_SPIBuffer_Rx[6] = mac_I_ACK;
	mac_SPIBuffer_Rx[7] = mac_DATA_FRAME_USER_PRIORITY_2;
	mac_SPIBuffer_Rx[8] = DATA_RATE_121_4;
	mac_SPIBuffer_Rx[9] = mac_LEVEL0;
	mac_SPIBuffer_Rx[10] = (BYTE)(((payloadLength - 10) >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[11] = (BYTE)((payloadLength - 10) & 0x00FF);
	for(mac_n = 12; mac_n < (payloadLength + 2); mac_n++)
		mac_SPIBuffer_Rx[mac_n] = 0xFF;

	macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
	macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
	macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
	macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
	macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
	macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
	macBIB.BeaconStartTime.Val = 1000;
#endif
#ifdef MAC_TEST_0008
	//MAC_TEST_0008
	//HUB终端MAC层Beacon帧的发送，原语默认发送
	WORD _payloadLength = 10;				//负载长度(包含了数据 和 其他管理项，增加其他管理项会使数据长度减小)
	mac_SPIBuffer_Rx[0] = ((_payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (_payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MLME_START_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//BANID
	mac_SPIBuffer_Rx[4] = 0x01;		//isDefault   1 TRUE     0 FALSE
	mac_SPIBuffer_Rx[5] = 0x6F;		//beaconPeriodLength
	mac_SPIBuffer_Rx[6] = 0x30;		//allocationSlotLength
	mac_SPIBuffer_Rx[7] = 0x05;		//RAP1Start
	mac_SPIBuffer_Rx[8] = 0x25;		//RAP1End
	mac_SPIBuffer_Rx[9] = 0x30;	//RAP2Start
	mac_SPIBuffer_Rx[10] = 0x50;	//RAP2End
	mac_SPIBuffer_Rx[11] = 0x00;	//SecurityLevel
#endif
#ifdef MAC_TEST_0009
	//MAC_TEST_0009
	//NODE终端MAC层Beacon帧的接收
	mac_rxBufferlen = 25;
	mac_RxBuffer[0] = 0x00;
	mac_RxBuffer[1] = 0x00;
	mac_RxBuffer[2] = 0x01;	//BSN
	mac_RxBuffer[3] = 0x00;
	mac_RxBuffer[4] = mac_BROADCAST_NID;
	mac_RxBuffer[5] = 0x02;
	mac_RxBuffer[6] = 0x02;

	mac_RxBuffer[7] = 0x66; 	//SenderAddress
	mac_RxBuffer[8] = 0x55;
	mac_RxBuffer[9] = 0x44;
	mac_RxBuffer[10] = 0x33;
	mac_RxBuffer[11] = 0x22;
	mac_RxBuffer[12] = 0x11;
	mac_RxBuffer[13] = 0xFF;	//beaconPeriodLength
	mac_RxBuffer[14] = 0xFF;	//allocationSlotLength
	mac_RxBuffer[15] = 0x32;	//RAP1End 50
	mac_RxBuffer[16] = 0x82;	//RAP2Start 130
	mac_RxBuffer[17] = 0xB4;	//RAP2End 180
	mac_RxBuffer[18] = 0x55;	//MACCapability
	mac_RxBuffer[19] = 0x01;
	mac_RxBuffer[20] = 0x08;
	mac_RxBuffer[21] = 0x0F;	//PHYCapability
	mac_RxBuffer[22] = 0x00;	//RAP1Start 0
	mac_RxBuffer[23] = mac_FCS_LSB;
	mac_RxBuffer[24] = mac_FCS_MSB;

	mac_TasksPending.bit.dataIndication = 1;

	macBIB.BANID.v = 0x02;
#endif
#ifdef MAC_TEST_0010
	//MAC_TEST_0010
	//HUB终端MAC层Beacon帧的发送，原语非默认发送
	WORD _payloadLength = 10;				//负载长度(包含了数据 和 其他管理项，增加其他管理项会使数据长度减小)
	mac_SPIBuffer_Rx[0] = ((_payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (_payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MLME_START_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//BANID
	mac_SPIBuffer_Rx[4] = 0x00;		//isDefault   1 TRUE     0 FALSE
	mac_SPIBuffer_Rx[5] = 0x10;		//beaconPeriodLength
	mac_SPIBuffer_Rx[6] = 0x05;		//allocationSlotLength
	mac_SPIBuffer_Rx[7] = 0x02;		//RAP1Start
	mac_SPIBuffer_Rx[8] = 0x05;		//RAP1End
	mac_SPIBuffer_Rx[9] = 0x07;	//RAP2Start
	mac_SPIBuffer_Rx[10] = 0x09;	//RAP2End
	mac_SPIBuffer_Rx[11] = 0x00;	//SecurityLevel
#endif
#ifdef MAC_TEST_0011
	//MAC_TEST_0011
	//HUB终端MAC层Beacon帧的发送，周期发送
	WORD _payloadLength = 10;				//负载长度(包含了数据 和 其他管理项，增加其他管理项会使数据长度减小)
	mac_SPIBuffer_Rx[0] = ((_payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (_payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MLME_START_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//BANID
	mac_SPIBuffer_Rx[4] = 0x01;		//isDefault   1 TRUE     0 FALSE
	mac_SPIBuffer_Rx[5] = 0x6F;		//beaconPeriodLength
	mac_SPIBuffer_Rx[6] = 0x30;		//allocationSlotLength
	mac_SPIBuffer_Rx[7] = 0x05;		//RAP1Start
	mac_SPIBuffer_Rx[8] = 0x25;		//RAP1End
	mac_SPIBuffer_Rx[9] = 0x30;	//RAP2Start
	mac_SPIBuffer_Rx[10] = 0x50;	//RAP2End
	mac_SPIBuffer_Rx[11] = 0x00;	//SecurityLevel
#endif
#ifdef MAC_TEST_0025
	//MAC_TEST_0025
	//RESET重置原语
	WORD _payloadLength = 2;
	mac_SPIBuffer_Rx[0] = ((_payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (_payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MLME_RESET_request;
	mac_SPIBuffer_Rx[3] = 0x01;
#endif
#ifdef MAC_TEST_0012
 	//MAC_TEST_0012
	//NODE终端MAC层的数据CSMA/CA发送（247字节）
	WORD payloadLength = 21;	//负载长度
	mac_SPIBuffer_Rx[0] = (BYTE)((payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE)(payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MACS_DATA_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//RecipientID
	mac_SPIBuffer_Rx[4] = 0x01;		//BANID
	mac_SPIBuffer_Rx[5] = 0x00;		//DSN
	mac_SPIBuffer_Rx[6] = mac_N_ACK;
	mac_SPIBuffer_Rx[7] = mac_DATA_FRAME_USER_PRIORITY_2;
	mac_SPIBuffer_Rx[8] = DATA_RATE_121_4;
	mac_SPIBuffer_Rx[9] = mac_LEVEL0;
	mac_SPIBuffer_Rx[10] = (BYTE)(((payloadLength - 10) >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[11] = (BYTE)((payloadLength - 10) & 0x00FF);
	for(mac_n = 12; mac_n < (payloadLength + 2); mac_n++)
		mac_SPIBuffer_Rx[mac_n] = 0xFF;

	macBIB.band = WBAN_BAND;
	macBIB.channelNumber = 0;
	macBIB.scramblerSeed = SCRAMBLER_SEED_0;
	macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
	macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
 	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
 	macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
 	macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
 	macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
 	macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
 	macBIB.BeaconStartTime.Val = 1000;			//660000;
#endif
#ifdef MAC_TEST_0013
 	//MAC_TEST_0013
	//NODE终端MAC层的数据2分片发送
	WORD payloadLength = 357;	//负载长度
	mac_SPIBuffer_Rx[0] = (BYTE)((payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE)(payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MACS_DATA_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//RecipientID
	mac_SPIBuffer_Rx[4] = 0x01;		//BANID
	mac_SPIBuffer_Rx[5] = 0x00;		//DSN
	mac_SPIBuffer_Rx[6] = mac_N_ACK;
	mac_SPIBuffer_Rx[7] = mac_DATA_FRAME_USER_PRIORITY_2;
	mac_SPIBuffer_Rx[8] = DATA_RATE_121_4;
	mac_SPIBuffer_Rx[9] = mac_LEVEL0;
	mac_SPIBuffer_Rx[10] = (BYTE)(((payloadLength - 10) >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[11] = (BYTE)((payloadLength - 10) & 0x00FF);
	for(mac_n = 12; mac_n < (payloadLength + 2); mac_n++)
		mac_SPIBuffer_Rx[mac_n] = 0xFF;

	macBIB.band = WBAN_BAND;
	macBIB.channelNumber = 0;
	macBIB.scramblerSeed = SCRAMBLER_SEED_0;
	macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
	macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
 	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
 	macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
 	macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
 	macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
 	macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
 	macBIB.BeaconStartTime.Val = 1000;			//66000
#endif
#ifdef MAC_TEST_0016
 	//MAC_TEST_0016
	//NODE终端MAC层的数据8分片发送
	WORD payloadLength = 1986;	//负载长度
	mac_SPIBuffer_Rx[0] = (BYTE)((payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE)(payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MACS_DATA_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//RecipientID
	mac_SPIBuffer_Rx[4] = 0x01;		//BANID
	mac_SPIBuffer_Rx[5] = 0x00;		//DSN
	mac_SPIBuffer_Rx[6] = mac_N_ACK;
	mac_SPIBuffer_Rx[7] = mac_DATA_FRAME_USER_PRIORITY_2;
	mac_SPIBuffer_Rx[8] = DATA_RATE_121_4;
	mac_SPIBuffer_Rx[9] = mac_LEVEL0;
	mac_SPIBuffer_Rx[10] = (BYTE)(((payloadLength - 10) >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[11] = (BYTE)((payloadLength - 10) & 0x00FF);
	for(mac_n = 12; mac_n < (payloadLength + 2); mac_n++)
		mac_SPIBuffer_Rx[mac_n] = 0xFF;

	macBIB.band = WBAN_BAND;
	macBIB.channelNumber = 0;
	macBIB.scramblerSeed = SCRAMBLER_SEED_0;
	macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
	macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
 	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
 	macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
 	macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
 	macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
 	macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
 	macBIB.BeaconStartTime.Val = 1000;			//66000
#endif
#ifdef MAC_TEST_0018
 	//MAC_TEST_0018
	//NODE终端MAC层的调度发送（247字节）
	WORD payloadLength = 257;	//负载长度
	mac_SPIBuffer_Rx[0] = (BYTE)((payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE)(payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MACS_DATA_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//RecipientID
	mac_SPIBuffer_Rx[4] = 0x01;		//BANID
	mac_SPIBuffer_Rx[5] = 0x00;		//DSN
	mac_SPIBuffer_Rx[6] = mac_N_ACK;
	mac_SPIBuffer_Rx[7] = mac_DATA_FRAME_USER_PRIORITY_2;
	mac_SPIBuffer_Rx[8] = DATA_RATE_121_4;
	mac_SPIBuffer_Rx[9] = mac_LEVEL0;
	mac_SPIBuffer_Rx[10] = (BYTE)(((payloadLength - 10) >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[11] = (BYTE)((payloadLength - 10) & 0x00FF);
	for(mac_n = 12; mac_n < (payloadLength + 2); mac_n++)
		mac_SPIBuffer_Rx[mac_n] = 0xFF;

	macBIB.band = WBAN_BAND;
	macBIB.channelNumber = 0;
	macBIB.scramblerSeed = SCRAMBLER_SEED_0;
	macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
	macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
 	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
 	macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
 	macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
 	macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
 	macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
 	macBIB.BeaconStartTime.Val = 1000;			//660000;
 	macBIB.IntervalStart.Val = 50000;
 	macBIB.IntervalEnd.Val = mac_RAP2_START_DEFAULT * temp;
#endif
#ifdef MAC_TEST_0019
	//MAC_TEST_0019
	//NODE终端MAC层的调度发送，没收到ACK
	WORD payloadLength = 257;	//负载长度
	mac_SPIBuffer_Rx[0] = (BYTE)((payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE)(payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MACS_DATA_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//RecipientID
	mac_SPIBuffer_Rx[4] = 0x01;		//BANID
	mac_SPIBuffer_Rx[5] = 0x00;		//DSN
	mac_SPIBuffer_Rx[6] = mac_I_ACK;
	mac_SPIBuffer_Rx[7] = mac_DATA_FRAME_USER_PRIORITY_2;
	mac_SPIBuffer_Rx[8] = DATA_RATE_121_4;
	mac_SPIBuffer_Rx[9] = mac_LEVEL0;
	mac_SPIBuffer_Rx[10] = (BYTE)(((payloadLength - 10) >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[11] = (BYTE)((payloadLength - 10) & 0x00FF);
	for(mac_n = 12; mac_n < (payloadLength + 2); mac_n++)
		mac_SPIBuffer_Rx[mac_n] = 0xFF;

	macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
	macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
	macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
	macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
	macBIB.EAP2Start.Val = mac_EAP2_START_DEFAULT * temp;
	macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
	macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
	macBIB.BeaconStartTime.Val = 1000;
 	macBIB.IntervalStart.Val = 50000;
 	macBIB.IntervalEnd.Val = mac_EAP2_START_DEFAULT * temp;
#endif
#ifdef MAC_TEST_0020
 	//MAC_TEST_0020
 	//HUB终端MAC层的调度接收
 	mac_rxBufferlen = 256;
 	mac_RxBuffer[0] = 0x00;	//N-ACK 0x00; I-ACK 0x02;
 	mac_RxBuffer[1] = 0x22;	//优先级为2
 	mac_RxBuffer[2] = 0x00;
 	mac_RxBuffer[3] = 0x00;
 	mac_RxBuffer[4] = macBIB.abbrAddr.v;
 	mac_RxBuffer[5] = 0x03;
 	mac_RxBuffer[6] = 0x02;
 	for(mac_n = 7; mac_n < (mac_rxBufferlen - 2); mac_n++)
 		mac_RxBuffer[mac_n] = 0xFF;
 	mac_RxBuffer[mac_n] = mac_FCS_LSB;
 	mac_RxBuffer[mac_n + 1] = mac_FCS_MSB;

 	mac_TasksPending.bit.dataIndication = 1;
#endif
#ifdef MAC_TEST_0023
 	//MAC_TEST_0023
	//NODE终端MAC层的加密发送（10字节）
 	WORD payloadLength = 20;	//负载长度
 	mac_SPIBuffer_Rx[0] = (BYTE)((payloadLength >> 8) & 0x00FF);
 	mac_SPIBuffer_Rx[1] = (BYTE)(payloadLength & 0x00FF);
 	mac_SPIBuffer_Rx[2] = MACS_DATA_request;
 	mac_SPIBuffer_Rx[3] = 0x02;		//RecipientID
 	mac_SPIBuffer_Rx[4] = 0x01;		//BANID
 	mac_SPIBuffer_Rx[5] = 0x00;		//DSN
 	mac_SPIBuffer_Rx[6] = mac_N_ACK;
 	mac_SPIBuffer_Rx[7] = mac_DATA_FRAME_USER_PRIORITY_2;
 	mac_SPIBuffer_Rx[8] = DATA_RATE_121_4;
 	mac_SPIBuffer_Rx[9] = mac_LEVEL2;
 	mac_SPIBuffer_Rx[10] = (BYTE)(((payloadLength - 10) >> 8) & 0x00FF);
 	mac_SPIBuffer_Rx[11] = (BYTE)((payloadLength - 10) & 0x00FF);
 	for(mac_n = 12; mac_n < (payloadLength + 2); mac_n++)
 		mac_SPIBuffer_Rx[mac_n] = 0xFF;

 	macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
 	macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
 	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
 	macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
 	macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
 	macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
 	macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
 	macBIB.BeaconStartTime.Val = 1000;

	macBIB.MK[0] =  0x20;
	macBIB.MK[1] =  0x7e;
	macBIB.MK[2] =  0x15;
	macBIB.MK[3] =  0x16;
	macBIB.MK[4] =  0x28;
	macBIB.MK[5] =  0xae;
	macBIB.MK[6] =  0xd2;
	macBIB.MK[7] =  0xa6;
	macBIB.MK[8] =  0xab;
	macBIB.MK[9] =  0xf7;
	macBIB.MK[10] =  0x15;
	macBIB.MK[11] =  0x88;
	macBIB.MK[12] =  0x09;
	macBIB.MK[13] =  0xcf;
	macBIB.MK[14] =  0x4f;
	macBIB.MK[15] =  0x3c;
#endif
#ifdef MAC_TEST_0024
	//MAC_TEST_0024
	//NODE终端MAC层的加密发送，收到ACK
	macBIB.abbrAddr.v = 0x03;
	macBIB.hubAddr.v = 0x02;
	macBIB.BANID.v = 0x02;

	WORD payloadLength = 20;	//负载长度
	mac_SPIBuffer_Rx[0] = (BYTE)((payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE)(payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = MACS_DATA_request;
	mac_SPIBuffer_Rx[3] = 0x02;		//RecipientID
	mac_SPIBuffer_Rx[4] = 0x02;		//BANID
	mac_SPIBuffer_Rx[5] = 0x00;		//DSN
	mac_SPIBuffer_Rx[6] = mac_I_ACK;
	mac_SPIBuffer_Rx[7] = mac_DATA_FRAME_USER_PRIORITY_2;
	mac_SPIBuffer_Rx[8] = DATA_RATE_121_4;
	mac_SPIBuffer_Rx[9] = mac_LEVEL2;
	mac_SPIBuffer_Rx[10] = (BYTE)(((payloadLength - 10) >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[11] = (BYTE)((payloadLength - 10) & 0x00FF);
	for(mac_n = 12; mac_n < (payloadLength + 2); mac_n++)
		mac_SPIBuffer_Rx[mac_n] = 0xFF;

	macBIB.allocationSlotLength = mac_ALLOCATION_SLOT_LENGTH_DEFAULT;
	macBIB.beaconPeriodLength = mac_BEACON_PERIOD_LENGTH_DEFAULT;
	uint64_t temp = mac_CLK_Freq * (mac_pAllocationSlotMin + macBIB.allocationSlotLength * mac_pAllocationSlotResolution);
	macBIB.RAP1Start.Val = mac_RAP1_START_DEFAULT * temp;
	macBIB.RAP1End.Val = mac_RAP1_END_DEFAULT * temp;
	macBIB.RAP2Start.Val = mac_RAP2_START_DEFAULT * temp;
	macBIB.RAP2End.Val = mac_RAP2_END_DEFAULT * temp;
	macBIB.BeaconStartTime.Val = 1000;

	mac_rxBufferlen = 9;
	mac_RxBuffer[0] = 0x00;
	mac_RxBuffer[1] = 0x10;
	mac_RxBuffer[2] = 0x00;
	mac_RxBuffer[3] = 0x00;
	mac_RxBuffer[4] = macBIB.abbrAddr.v;
	mac_RxBuffer[5] = 0x02;
	mac_RxBuffer[6] = 0x02;
	mac_RxBuffer[7] = mac_FCS_LSB;
	mac_RxBuffer[8] = mac_FCS_MSB;

	macBIB.MK[0] =  0x20;
	macBIB.MK[1] =  0x7e;
	macBIB.MK[2] =  0x15;
	macBIB.MK[3] =  0x16;
	macBIB.MK[4] =  0x28;
	macBIB.MK[5] =  0xae;
	macBIB.MK[6] =  0xd2;
	macBIB.MK[7] =  0xa6;
	macBIB.MK[8] =  0xab;
	macBIB.MK[9] =  0xf7;
	macBIB.MK[10] =  0x15;
	macBIB.MK[11] =  0x88;
	macBIB.MK[12] =  0x09;
	macBIB.MK[13] =  0xcf;
	macBIB.MK[14] =  0x4f;
	macBIB.MK[15] =  0x3c;
#endif
#ifdef MAC_TEST_0014
 	//MAC_TEST_0014
 	//HUB终端MAC层的数据接收，不需要ACK，247字节
 	mac_rxBufferlen = 256;
 	mac_RxBuffer[0] = 0x00;	//N-ACK 0x00; I-ACK 0x02;
 	mac_RxBuffer[1] = 0x22;	//优先级为2
 	mac_RxBuffer[2] = 0x00;
 	mac_RxBuffer[3] = 0x00;		//控制分片 0x09为分片序号1且不是最后的分片  0x02为分片序号2且是最后的分片 0x00表示不 分片
 	mac_RxBuffer[4] = macBIB.abbrAddr.v;
 	mac_RxBuffer[5] = 0x03;
 	mac_RxBuffer[6] = 0x02;
 	for(mac_n = 7; mac_n < (mac_rxBufferlen - 2); mac_n++)
 		mac_RxBuffer[mac_n] = mac_n - 7;
 	mac_RxBuffer[mac_n] = mac_FCS_LSB;
 	mac_RxBuffer[mac_n + 1] = mac_FCS_MSB;

 	mac_TasksPending.bit.dataIndication = 1;
#endif
#ifdef MAC_TEST_0015
 	//MAC_TEST_0015
 	//HUB终端MAC层的数据接收，不需要ACK，2分片
 	mac_rxBufferlen = 256;
 	mac_RxBuffer[0] = 0x00;	//N-ACK 0x00; I-ACK 0x02;
 	mac_RxBuffer[1] = 0x22;	//优先级为2
 	mac_RxBuffer[2] = 0x00;
 	mac_RxBuffer[3] = 0x08;//0x08第一帧   0x01 第二帧
 	mac_RxBuffer[4] = macBIB.abbrAddr.v;
 	mac_RxBuffer[5] = 0x03;
 	mac_RxBuffer[6] = 0x02;
 	for(mac_n = 7; mac_n < (mac_rxBufferlen - 2); mac_n++)
 		mac_RxBuffer[mac_n] = mac_n - 7;
 	mac_RxBuffer[mac_n] = mac_FCS_LSB;
 	mac_RxBuffer[mac_n + 1] = mac_FCS_MSB;

 	mac_TasksPending.bit.dataIndication = 1;
#endif
#ifdef MAC_TEST_0017
 	//MAC_TEST_0017
 	//HUB终端MAC层的数据接收，不需要ACK，8分片
 	mac_rxBufferlen = 256;
 	mac_RxBuffer[0] = 0x00;	//N-ACK 0x00; I-ACK 0x02;
 	mac_RxBuffer[1] = 0x22;	//优先级为2
 	mac_RxBuffer[2] = 0x00;
 	mac_RxBuffer[3] = 0x08;
 	mac_RxBuffer[4] = macBIB.abbrAddr.v;
 	mac_RxBuffer[5] = 0x03;
 	mac_RxBuffer[6] = 0x02;
 	for(mac_n = 7; mac_n < (mac_rxBufferlen - 2); mac_n++)
 		mac_RxBuffer[mac_n] = 0xFF;
 	mac_RxBuffer[mac_n] = mac_FCS_LSB;
 	mac_RxBuffer[mac_n + 1] = mac_FCS_MSB;

 	mac_TasksPending.bit.dataIndication = 1;
#endif
#ifdef MAC_TEST_0021
 	//MAC_TEST_0021
 	//HUB终端MAC层的解密接收，不需要ACK
	mac_RxBuffer[0] = 0x96;
	mac_RxBuffer[1] = 0x30;
	mac_RxBuffer[2] = 0x7f;
	mac_RxBuffer[3] = 0xd0;
	mac_RxBuffer[4] = 0x1e;
	mac_RxBuffer[5] = 0x13;
	mac_RxBuffer[6] = 0xb4;
	mac_RxBuffer[7] = 0x4b;
	mac_RxBuffer[8] = 0x24;
	mac_RxBuffer[9] = 0x52;
	mac_RxBuffer[10] = 0xea;
	mac_RxBuffer[11] = 0xcb;
	mac_RxBuffer[12] = 0x55;
	mac_RxBuffer[13] = 0xf1;
	mac_RxBuffer[14] = 0x6e;
	mac_RxBuffer[15] = 0x31;
	mac_RxBuffer[16] = 0x14;
	mac_RxBuffer[17] = 0xfc;
	mac_RxBuffer[18] = 0xae;
	mac_RxBuffer[19] = 0xab;
	mac_RxBuffer[20] = 0x10;
	mac_RxBuffer[21] = 0x69;
	mac_RxBuffer[22] = 0xc8;
	mac_RxBuffer[23] = 0x63;
	mac_RxBuffer[24] = 0xe1;
	mac_RxBuffer[25] = 0x2e;
	mac_RxBuffer[26] = 0x56;
	mac_RxBuffer[27] = 0x16;
	mac_RxBuffer[28] = 0x0b;
	mac_RxBuffer[29] = 0x4f;
	mac_RxBuffer[30] = 0xe4;
	mac_RxBuffer[31] = 0x7c;
	mac_rxBufferlen = 32;

	macBIB.MK[0] =  0x20;
	macBIB.MK[1] =  0x7e;
	macBIB.MK[2] =  0x15;
	macBIB.MK[3] =  0x16;
	macBIB.MK[4] =  0x28;
	macBIB.MK[5] =  0xae;
	macBIB.MK[6] =  0xd2;
	macBIB.MK[7] =  0xa6;
	macBIB.MK[8] =  0xab;
	macBIB.MK[9] =  0xf7;
	macBIB.MK[10] =  0x15;
	macBIB.MK[11] =  0x88;
	macBIB.MK[12] =  0x09;
	macBIB.MK[13] =  0xcf;
	macBIB.MK[14] =  0x4f;
	macBIB.MK[15] =  0x3c;

	mac_TasksPending.bit.dataIndication = 1;
	macBIB.isEncrypt = TRUE;
	macBIB.abbrAddr.v = 0x02;
	macBIB.BANID.v = 0x01;
#endif
#ifdef MAC_TEST_0022
	//MAC_TEST_0022
 	//HUB终端MAC层的解密接收，ACK不加密发送
	mac_RxBuffer[0] = 0xce;
	mac_RxBuffer[1] = 0x37;
	mac_RxBuffer[2] = 0xfb;
	mac_RxBuffer[3] = 0xe3;
	mac_RxBuffer[4] = 0x21;
	mac_RxBuffer[5] = 0xa0;
	mac_RxBuffer[6] = 0x77;
	mac_RxBuffer[7] = 0x8f;
	mac_RxBuffer[8] = 0x6a;
	mac_RxBuffer[9] = 0x8f;
	mac_RxBuffer[10] = 0x4d;
	mac_RxBuffer[11] = 0x0e;
	mac_RxBuffer[12] = 0x0e;
	mac_RxBuffer[13] = 0x7a;
	mac_RxBuffer[14] = 0x86;
	mac_RxBuffer[15] = 0xeb;
	mac_RxBuffer[16] = 0x14;
	mac_RxBuffer[17] = 0xfc;
	mac_RxBuffer[18] = 0xae;
	mac_RxBuffer[19] = 0xab;
	mac_RxBuffer[20] = 0x10;
	mac_RxBuffer[21] = 0x69;
	mac_RxBuffer[22] = 0xc8;
	mac_RxBuffer[23] = 0x63;
	mac_RxBuffer[24] = 0xe1;
	mac_RxBuffer[25] = 0x2e;
	mac_RxBuffer[26] = 0x56;
	mac_RxBuffer[27] = 0x16;
	mac_RxBuffer[28] = 0x0b;
	mac_RxBuffer[29] = 0x4f;
	mac_RxBuffer[30] = 0xe4;
	mac_RxBuffer[31] = 0x7c;
	mac_rxBufferlen = 32;

	macBIB.MK[0] =  0x20;
	macBIB.MK[1] =  0x7e;
	macBIB.MK[2] =  0x15;
	macBIB.MK[3] =  0x16;
	macBIB.MK[4] =  0x28;
	macBIB.MK[5] =  0xae;
	macBIB.MK[6] =  0xd2;
	macBIB.MK[7] =  0xa6;
	macBIB.MK[8] =  0xab;
	macBIB.MK[9] =  0xf7;
	macBIB.MK[10] =  0x15;
	macBIB.MK[11] =  0x88;
	macBIB.MK[12] =  0x09;
	macBIB.MK[13] =  0xcf;
	macBIB.MK[14] =  0x4f;
	macBIB.MK[15] =  0x3c;

	mac_TasksPending.bit.dataIndication = 1;
	macBIB.isEncrypt = TRUE;
	macBIB.abbrAddr.v = 0x02;
#endif
#ifdef MAC_TEST_0101
	//MAC_TEST_0101
	//初始化SPIBuffer_Rx数据
	WORD _payloadLength = 0x0005;//负载长度(包含了数据 和 其他管理项，增加其他管理项会使数据长度减小)
	mac_SPIBuffer_Rx[0] = (BYTE)((_payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE)(_payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = HME_MLME_CONNECT_request;
	mac_SPIBuffer_Rx[3] = 0x02;               //BANID
	mac_SPIBuffer_Rx[4] = mac_UNCONNECTED_BROADCAST_NID;  //RecipientID
	mac_SPIBuffer_Rx[5] = 0x00;              //CapabilityInformation
	mac_SPIBuffer_Rx[6] = 0x02;               //ConnectionSecurityLevel
#endif
#ifdef MAC_TEST_0102
	//MAC_TEST_0102
	//初始化SPIBuffer_Rx数据
	WORD _payloadLength = 0x0005;//负载长度(包含了数据 和 其他管理项，增加其他管理项会使数据长度减小)
	mac_SPIBuffer_Rx[0] = (BYTE)((_payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE)(_payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = HME_MLME_CONNECT_request;
	mac_SPIBuffer_Rx[3] = 0x02;               //BANID
	mac_SPIBuffer_Rx[4] = mac_UNCONNECTED_BROADCAST_NID;  //RecipientID
	mac_SPIBuffer_Rx[5] = 0x00;              //CapabilityInformation
	mac_SPIBuffer_Rx[6] = 0x02;               //ConnectionSecurityLevel


	mac_rxBufferlen = 31;
	mac_RxBuffer[0] = 0x12;
	mac_RxBuffer[1] = 0x08;
	mac_RxBuffer[2] = 0x00;
	mac_RxBuffer[3] = 0x00;
	mac_RxBuffer[4] = 0x02;
	mac_RxBuffer[5] = mac_UNCONNECTED_NID;
	mac_RxBuffer[6] = 0x02;

	mac_RxBuffer[7] = 0x00; 	//RecipientAddress
	mac_RxBuffer[8] = 0x00;
	mac_RxBuffer[9] = 0x00;
	mac_RxBuffer[10] = 0x00;
	mac_RxBuffer[11] = 0x00;
	mac_RxBuffer[12] = 0x00;

	mac_RxBuffer[13] = 0x01; 	//SenderAddress
	mac_RxBuffer[14] = 0x23;
	mac_RxBuffer[15] = 0x45;
	mac_RxBuffer[16] = 0x67;
	mac_RxBuffer[17] = 0x89;
	mac_RxBuffer[18] = 0x01;

	mac_RxBuffer[19] = 0x55;	//MACCapability
	mac_RxBuffer[20] = 0x01;
	mac_RxBuffer[21] = 0x08;
	mac_RxBuffer[22] = 0x0f;	//PHYCapability

	mac_RxBuffer[23] = 0x00;	//ConnectionChangeIndicator
	mac_RxBuffer[24] = 0x11;	//RequestAckRate
	mac_RxBuffer[25] = 0x00;	//RequestedWakeupPeriod
	mac_RxBuffer[26] = 0x00;
	mac_RxBuffer[27] = 0x00;	//RequestedWakeupPhase
	mac_RxBuffer[28] = 0x00;
	mac_RxBuffer[29] = mac_FCS_LSB;
	mac_RxBuffer[30] = mac_FCS_MSB;
#endif
/*
	//T-Poll帧到来
	mac_rxBufferlen = 13;
	mac_RxBuffer[0] = 0x10;
	mac_RxBuffer[1] = 0x17;
	mac_RxBuffer[2] = 0x01;	//PollPostWindow
	mac_RxBuffer[3] = 0x00;	//Next
	mac_RxBuffer[4] = mac_UNCONNECTED_BROADCAST_NID;
	mac_RxBuffer[5] = 0x02;
	mac_RxBuffer[6] = 0x02;

	mac_RxBuffer[7] = 0x00; 		//CurrentAllocationSlotNumber
	mac_RxBuffer[8] = 0x00;			//CurrentAllocationSlotOffset
	mac_RxBuffer[9] = 0x00;
	mac_RxBuffer[10] = 0x0E;        //CurrentAllocationSlotLength

	mac_RxBuffer[11] = mac_FCS_LSB;
	mac_RxBuffer[12] = mac_FCS_MSB;

	mac_TasksPending.bit.dataIndication = 1;
*/
#ifdef MAC_TEST_0301
	//MAC_TEST_0301
	//初始化SPIBuffer_Rx数据
	WORD _payloadLength = 0x0008;
	mac_SPIBuffer_Rx[0] = (BYTE)((_payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (BYTE	)(_payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = NME_MLME_CONNECT_request;

	mac_SPIBuffer_Rx[3] = 0x02;              //BANID
	mac_SPIBuffer_Rx[4] = 0x02;              //HubID
	mac_SPIBuffer_Rx[5] = 0x00;              //CapabilityInformation
	mac_SPIBuffer_Rx[6] = 0x00;              //SecurityLevel
	mac_SPIBuffer_Rx[7] = 0x40;              //DataArrivalRateIndicator
	mac_SPIBuffer_Rx[8] = 0x00;              //FrameSubtype
	mac_SPIBuffer_Rx[9] = 0x02;              //UserPriority

    //macBIB.allocationSlotLength = 0x08;
#endif
#ifdef MAC_TEST_0302
 	//MAC_TEST_0302
 	//初始化SPIBuffer_Rx数据
 	WORD _payloadLength = 0x0008;
 	mac_SPIBuffer_Rx[0] = (BYTE)((_payloadLength >> 8) & 0x00FF);
 	mac_SPIBuffer_Rx[1] = (BYTE	)(_payloadLength & 0x00FF);
 	mac_SPIBuffer_Rx[2] = NME_MLME_CONNECT_request;

 	mac_SPIBuffer_Rx[3] = 0x02;              //BANID
 	mac_SPIBuffer_Rx[4] = 0x02;              //HubID
 	mac_SPIBuffer_Rx[5] = 0x00;              //CapabilityInformation
 	mac_SPIBuffer_Rx[6] = 0x02;              //SecurityLevel
 	mac_SPIBuffer_Rx[7] = 0x40;              //DataArrivalRateIndicator
 	mac_SPIBuffer_Rx[8] = 0x00;              //FrameSubtype
 	mac_SPIBuffer_Rx[9] = 0x02;              //UserPriority

 	macBIB.allocationSlotLength = 0xff;

 	//ACK
 	mac_RxBuffer[0] = 0x00;
 	mac_RxBuffer[1] = 0x10;
 	mac_RxBuffer[2] = 0x00;
 	mac_RxBuffer[3] = 0x00;
	mac_RxBuffer[4] = 0x03;
 	mac_RxBuffer[5] = 0x03;
 	mac_RxBuffer[6] = 0x02;
 	mac_RxBuffer[7] = mac_FCS_LSB;
 	mac_RxBuffer[8] = mac_FCS_MSB;
 	mac_rxBufferlen = 9;
 	Delay(1);
#endif
#ifdef MAC_TEST_0401
    //MAC_TEST_0401
	//ConnectionRequestFrame帧到来
	mac_rxBufferlen = 39;
	mac_RxBuffer[0] = 0x12;
	mac_RxBuffer[1] = 0x08;
	mac_RxBuffer[2] = 0x00;
	mac_RxBuffer[3] = 0x00;
	mac_RxBuffer[4] = 0x02;
	mac_RxBuffer[5] = mac_UNCONNECTED_NID;
	mac_RxBuffer[6] = 0x02;

	mac_RxBuffer[7] = 0x00; 	//RecipientAddress
	mac_RxBuffer[8] = 0x00;
	mac_RxBuffer[9] = 0x00;
	mac_RxBuffer[10] = 0x00;
	mac_RxBuffer[11] = 0x00;
	mac_RxBuffer[12] = 0x00;

	mac_RxBuffer[13] = 0x01; 	//SenderAddress
	mac_RxBuffer[14] = 0x23;
	mac_RxBuffer[15] = 0x45;
	mac_RxBuffer[16] = 0x67;
	mac_RxBuffer[17] = 0x89;
	mac_RxBuffer[18] = 0x01;

	mac_RxBuffer[19] = 0x55;	//MACCapability
	mac_RxBuffer[20] = 0x01;
	mac_RxBuffer[21] = 0x08;
	mac_RxBuffer[22] = 0x0f;	//PHYCapability

	mac_RxBuffer[23] = 0x00;	//ConnectionChangeIndicator
	mac_RxBuffer[24] = 0x11;	//RequestAckRate
	mac_RxBuffer[25] = 0x00;	//RequestedWakeupPeriod
	mac_RxBuffer[26] = 0x00;
	mac_RxBuffer[27] = 0x00;	//RequestedWakeupPhase
	mac_RxBuffer[28] = 0x00;

	mac_RxBuffer[29] = 0x01;	//ElementID
	mac_RxBuffer[30] = 0x06;	//Length
	mac_RxBuffer[31] = 0x20;	//Allocation Length
	mac_RxBuffer[32] = 0x00;
	mac_RxBuffer[33] = 0x00;
	mac_RxBuffer[34] = 0x00;
	mac_RxBuffer[35] = 0x02;
	mac_RxBuffer[36] = 0x00;

	mac_RxBuffer[37] = mac_FCS_LSB;
	mac_RxBuffer[38] = mac_FCS_MSB;

	mac_TasksPending.bit.dataIndication = 1;
#endif
#ifdef MAC_TEST_0501
	//MAC_TEST_0501
	BYTE _payloadLength = 4;

	mac_SPIBuffer_Rx[0] = ((_payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[1] = (_payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[2] = HME_MLME_CONNECT_response;
	mac_SPIBuffer_Rx[3] = 0x03;             //AssocAbbrID
	mac_SPIBuffer_Rx[4] = 0x00;			//Status
	mac_SPIBuffer_Rx[5] = 0x02;            //SecurityLevel

	macBIB.nodeList[0].abbrAddr.v = 0x03;

	BYTE mac_a;
	mac_a = getIndexFromNodeList( macBIB.nodeList[0].abbrAddr );
	macBIB.nodeList[mac_a].fullAddr.v[0] = 0x66;
	macBIB.nodeList[mac_a].fullAddr.v[1] = 0x55;
	macBIB.nodeList[mac_a].fullAddr.v[2] = 0x44;
	macBIB.nodeList[mac_a].fullAddr.v[3] = 0x33;
	macBIB.nodeList[mac_a].fullAddr.v[4] = 0x22;
	macBIB.nodeList[mac_a].fullAddr.v[5] = 0x11;

	macBIB.uplinkRequestIE.AllocationLength = 0x02;
#endif
#ifdef MAC_TEST_0601
 	//MAC_TEST_0601
	//ConnectionAssignmentFrame帧到来
	mac_rxBufferlen = 41;
	mac_RxBuffer[0] = 0x02;
	mac_RxBuffer[1] = 0x09;
	mac_RxBuffer[2] = 0x00;
	mac_RxBuffer[3] = 0x00;
	mac_RxBuffer[4] = 0x03;
	mac_RxBuffer[5] = 0x02;
	mac_RxBuffer[6] = 0x02;

	mac_RxBuffer[7] = 0x66; 	//RecipientAddress
	mac_RxBuffer[8] = 0x55;
	mac_RxBuffer[9] = 0x44;
	mac_RxBuffer[10] = 0x33;
	mac_RxBuffer[11] = 0x22;
	mac_RxBuffer[12] = 0x11;
	mac_RxBuffer[13] = 0x01; 	//SenderAddress
	mac_RxBuffer[14] = 0x23;
	mac_RxBuffer[15] = 0x45;
	mac_RxBuffer[16] = 0x67;
	mac_RxBuffer[17] = 0x89;
	mac_RxBuffer[18] = 0x01;

	mac_RxBuffer[19] = 0x00;	//mode
	mac_RxBuffer[20] = 0x00;	//CurrentSuperframeNumber
	mac_RxBuffer[21] = 0x00;	//EarliestRAP1End
	mac_RxBuffer[22] = 0x11;	//EAP2Start
	mac_RxBuffer[23] = 0x00;	//Minimum_CAP_Length
	mac_RxBuffer[24] = 0x41;	//MACCapability
	mac_RxBuffer[25] = 0x00;
	mac_RxBuffer[26] = 0x08;
	mac_RxBuffer[27] = 0x08;	//PHYCapability
	mac_RxBuffer[28] = 0x00;	//ConnectionChangeIndicator
	mac_RxBuffer[29] = 0x11;    //AssignedAckDataRates
	mac_RxBuffer[30] = 0x00;	//RequestedWakeupPhase
	mac_RxBuffer[31] = 0x00;
	mac_RxBuffer[32] = 0x00;	//RequestedWakeupPeriod
	mac_RxBuffer[33] = 0x00;
	mac_RxBuffer[34] = 0x01;    //ElementID
	mac_RxBuffer[35] = 0x03;	//Length
	mac_RxBuffer[36] = 0x07;
	mac_RxBuffer[37] = 0x32;	//AllocationAssignment
	mac_RxBuffer[38] = 0x34;
	mac_RxBuffer[39] = mac_FCS_LSB;
	mac_RxBuffer[40] = mac_FCS_MSB;

	mac_TasksPending.bit.dataIndication = 1;
	macBIB.abbrAddr.v = 0x03;
#endif
#ifdef MAC_TEST_0701
 	//MAC_TEST_0701
 	BYTE _payloadLength = 10;

 	mac_SPIBuffer_Rx[0] = ((_payloadLength >> 8) & 0x00FF);
 	mac_SPIBuffer_Rx[1] = (_payloadLength & 0x00FF);
 	mac_SPIBuffer_Rx[2] = HME_MLME_CONNECT_response;
 	mac_SPIBuffer_Rx[3] = 0x01;             //NodeAddress
 	mac_SPIBuffer_Rx[4] = 0x02;
 	mac_SPIBuffer_Rx[5] = 0x03;
 	mac_SPIBuffer_Rx[6] = 0x04;
 	mac_SPIBuffer_Rx[7] = 0x05;
 	mac_SPIBuffer_Rx[8] = 0x06;
 	mac_SPIBuffer_Rx[9] = 0x03;             //AssocAbbrID
 	mac_SPIBuffer_Rx[10] = 0x00;			//Status
 	mac_SPIBuffer_Rx[11] = 0x02;            //SecurityLevel

 	macBIB.nodeList[0].abbrAddr.v = 0x03;
    macBIB.nodeList[0].fullAddr.v[0] = 0x01;
 	macBIB.nodeList[0].fullAddr.v[1] = 0x02;
 	macBIB.nodeList[0].fullAddr.v[2] = 0x03;
 	macBIB.nodeList[0].fullAddr.v[3] = 0x04;
 	macBIB.nodeList[0].fullAddr.v[4] = 0x05;
 	macBIB.nodeList[0].fullAddr.v[5] = 0x06;

 	macBIB.uplinkRequestIE.AllocationLength = 0x02;

 	//ACK
 	mac_RxBuffer[0] = 0x00;
 	mac_RxBuffer[1] = 0x10;
 	mac_RxBuffer[2] = 0x00;
 	mac_RxBuffer[3] = 0x00;
 	mac_RxBuffer[4] = 0x02;
 	mac_RxBuffer[5] = 0x03;
 	mac_RxBuffer[6] = 0x02;
 	mac_RxBuffer[7] = mac_FCS_LSB;
 	mac_RxBuffer[8] = mac_FCS_MSB;
 	mac_rxBufferlen = 9;
#endif

#ifdef MAC_TEST_FF01
 	//MAC_TEST_FF01 NODE端 request-->1st association frame
 	BYTE _payloadLength = 10;

 	mac_SPIBuffer_Rx[0] = ((_payloadLength >> 8) & 0x00FF);
 	mac_SPIBuffer_Rx[1] = (_payloadLength & 0x00FF);
 	mac_SPIBuffer_Rx[2] = NME_MLME_SECURITY_request;
 	mac_SPIBuffer_Rx[3] = 0x00;
 	mac_SPIBuffer_Rx[4] = 0x00;
 	mac_SPIBuffer_Rx[5] = 0x00;
 	mac_SPIBuffer_Rx[6] = 0x00;
 	mac_SPIBuffer_Rx[7] = 0x00;
 	mac_SPIBuffer_Rx[8] = 0x00;
 	mac_SPIBuffer_Rx[9] = 0x02;
 	mac_SPIBuffer_Rx[10] = 0x00;
 	mac_SPIBuffer_Rx[11] = 0x00;

 	macBIB.hubAddr.v = 0x02;
 	macBIB.BANID.v = 0x01;

#endif

#ifdef MAC_TEST_FF02
 	//MAC_TEST_FF02 HUB端收到1st assoication frame --> 上传indication原语 并发ack
 	mac_rxBufferlen = 25;
 	mac_RxBuffer[0] = 0x12;
 	mac_RxBuffer[1] = 0x02;
 	mac_RxBuffer[2] = 0x00;
 	mac_RxBuffer[3] = 0x00;
 	mac_RxBuffer[4] = 0x02;
 	mac_RxBuffer[5] = 0x01;
 	mac_RxBuffer[6] = 0x02;

 	mac_RxBuffer[7] = 0x00; 	//RecipientAddress
 	mac_RxBuffer[8] = 0x00;
 	mac_RxBuffer[9] = 0x00;
 	mac_RxBuffer[10] = 0x00;
 	mac_RxBuffer[11] = 0x00;
 	mac_RxBuffer[12] = 0x00;
 	mac_RxBuffer[13] = 0x01; 	//SenderAddress
 	mac_RxBuffer[14] = 0x23;
 	mac_RxBuffer[15] = 0x45;
 	mac_RxBuffer[16] = 0x67;
 	mac_RxBuffer[17] = 0x89;
 	mac_RxBuffer[18] = 0x01;

 	mac_RxBuffer[19] = 0x10;	//mode
 	mac_RxBuffer[20] = 0x00;	//CurrentSuperframeNumber
 	mac_RxBuffer[21] = 0x01;	//EarliestRAP1End
 	mac_RxBuffer[22] = 0x00;	//EAP2Start
 	mac_RxBuffer[23] = mac_FCS_LSB;
 	mac_RxBuffer[24] = mac_FCS_MSB;

 	mac_TasksPending.bit.dataIndication = 1;

 	macBIB.abbrAddr.v = 0x02;
#endif

#ifdef MAC_TEST_FF03
 	//MAC_TEST_FF03 HUB端response --> 2nd association frame
 	BYTE _payloadLength = 9;

 	mac_SPIBuffer_Rx[0] = ((_payloadLength >> 8) & 0x00FF);
 	mac_SPIBuffer_Rx[1] = (_payloadLength & 0x00FF);
 	mac_SPIBuffer_Rx[2] = HME_MLME_SECURITY_response;
 	mac_SPIBuffer_Rx[3] = 0x01;
 	mac_SPIBuffer_Rx[4] = 0x23;
 	mac_SPIBuffer_Rx[5] = 0x45;
 	mac_SPIBuffer_Rx[6] = 0x67;
 	mac_SPIBuffer_Rx[7] = 0x89;
 	mac_SPIBuffer_Rx[8] = 0x01;
 	mac_SPIBuffer_Rx[9] = 0x00;
 	mac_SPIBuffer_Rx[10] = 0x00;

 	macBIB.nodeFullAddrTemp.v[0]=0x01;
 	macBIB.nodeFullAddrTemp.v[1]=0x23;
 	macBIB.nodeFullAddrTemp.v[2]=0x45;
 	macBIB.nodeFullAddrTemp.v[3]=0x67;
 	macBIB.nodeFullAddrTemp.v[4]=0x89;
 	macBIB.nodeFullAddrTemp.v[5]=0x01;
 	macBIB.SecurityLevel=0x02;
#endif

#ifdef MAC_TEST_FF04
 	//MAC_TEST_FF04 NODE端 收到2nd association frame --> 1st PTK 并发ack
 	mac_rxBufferlen = 25;
 	mac_RxBuffer[0] = 0x12;
 	mac_RxBuffer[1] = 0x02;
 	mac_RxBuffer[2] = 0x00;
 	mac_RxBuffer[3] = 0x00;
 	mac_RxBuffer[4] = 0x01;
 	mac_RxBuffer[5] = 0x02;
 	mac_RxBuffer[6] = 0x02;

 	mac_RxBuffer[7] = 0x01; 	//RecipientAddress
 	mac_RxBuffer[8] = 0x23;
 	mac_RxBuffer[9] = 0x45;
 	mac_RxBuffer[10] = 0x67;
 	mac_RxBuffer[11] = 0x89;
 	mac_RxBuffer[12] = 0x01;
 	mac_RxBuffer[13] = 0x02; 	//SenderAddress
 	mac_RxBuffer[14] = 0x31;
 	mac_RxBuffer[15] = 0x57;
 	mac_RxBuffer[16] = 0x89;
 	mac_RxBuffer[17] = 0x10;
 	mac_RxBuffer[18] = 0x45;

 	mac_RxBuffer[19] = 0x30;
 	mac_RxBuffer[20] = 0x00;
 	mac_RxBuffer[21] = 0x02;
 	mac_RxBuffer[22] = 0x00;
 	mac_RxBuffer[23] = mac_FCS_LSB;
 	mac_RxBuffer[24] = mac_FCS_MSB;

 	mac_TasksPending.bit.dataIndication = 1;

 	macBIB.hubAddr.v = 0x02;
 	macBIB.hubFullAddr.v[0]=0x02;
 	macBIB.hubFullAddr.v[1]=0x31;
 	macBIB.hubFullAddr.v[2]=0x57;
 	macBIB.hubFullAddr.v[3]=0x89;
 	macBIB.hubFullAddr.v[4]=0x10;
 	macBIB.hubFullAddr.v[5]=0x45;
 	macBIB.SecurityLevel=0x02;
 	macBIB.BANID.v = 0x02;
#endif

#ifdef MAC_TEST_FF05
 	//MAC_TEST_FF05 HUB端 收到1stPTK --> 2nd PTK 并发ack
 	mac_rxBufferlen = 47;
 	mac_RxBuffer[0] = 0x32;
 	mac_RxBuffer[1] = 0x04;
 	mac_RxBuffer[2] = 0x01;
 	mac_RxBuffer[3] = 0x00;
 	mac_RxBuffer[4] = 0x02;
 	mac_RxBuffer[5] = 0x01;
 	mac_RxBuffer[6] = 0x02;

 	mac_RxBuffer[7] = 0x02; 	//RecipientAddress
 	mac_RxBuffer[8] = 0x31;
 	mac_RxBuffer[9] = 0x57;
 	mac_RxBuffer[10] = 0x89;
 	mac_RxBuffer[11] = 0x10;
 	mac_RxBuffer[12] = 0x45;
 	mac_RxBuffer[13] = 0x01; 	//SenderAddress
 	mac_RxBuffer[14] = 0x23;
 	mac_RxBuffer[15] = 0x45;
 	mac_RxBuffer[16] = 0x67;
 	mac_RxBuffer[17] = 0x89;
 	mac_RxBuffer[18] = 0x01;

 	mac_RxBuffer[19] =0x01;   //Sender Nonce
 	mac_RxBuffer[20] =0x02;
	mac_RxBuffer[21] =0x03;
	mac_RxBuffer[22] =0x04;
	mac_RxBuffer[23] =0x05;
	mac_RxBuffer[24] =0x06;
	mac_RxBuffer[25] =0x07;
	mac_RxBuffer[26] =0x08;
	mac_RxBuffer[27] =0x09;
	mac_RxBuffer[28] =0x10;
	mac_RxBuffer[29] =0x11;
	mac_RxBuffer[30] =0x12;
	mac_RxBuffer[31] =0x13;
	mac_RxBuffer[32] =0x14;
	mac_RxBuffer[33] =0x15;
	mac_RxBuffer[34] =0x16;

	mac_RxBuffer[35] =0x01;
	mac_RxBuffer[36] =0x00;
	mac_RxBuffer[37] =0x00;
	mac_RxBuffer[38] =0x00;
	mac_RxBuffer[40] =0x00;
	mac_RxBuffer[41] =0x00;
	mac_RxBuffer[42] =0x00;
	mac_RxBuffer[43] =0x00;
	mac_RxBuffer[44] =0x00;
	mac_RxBuffer[45] =0x00;

	mac_RxBuffer[46] = mac_FCS_LSB;
 	mac_RxBuffer[47] = mac_FCS_MSB;

 	mac_TasksPending.bit.dataIndication = 1;

	macBIB.nodeFullAddrTemp.v[0]=0x01;
	macBIB.nodeFullAddrTemp.v[1]=0x23;
	macBIB.nodeFullAddrTemp.v[2]=0x45;
	macBIB.nodeFullAddrTemp.v[3]=0x67;
	macBIB.nodeFullAddrTemp.v[4]=0x89;
	macBIB.nodeFullAddrTemp.v[5]=0x01;
	macBIB.SecurityLevel=0x02;
	macBIB.BANID.v = 0x02;

	Nonce_I[0]=0x01;
	Nonce_I[1]=0x02;
	Nonce_I[2]=0x03;
	Nonce_I[3]=0x04;
	Nonce_I[4]=0x05;
	Nonce_I[5]=0x06;
	Nonce_I[6]=0x07;
	Nonce_I[7]=0x08;
	Nonce_I[8]=0x09;
	Nonce_I[9]=0x10;
	Nonce_I[10]=0x11;
	Nonce_I[11]=0x12;
	Nonce_I[12]=0x13;
	Nonce_I[13]=0x14;
	Nonce_I[14]=0x15;
	Nonce_I[15]=0x16;
#endif

#ifdef MAC_TEST_FF06
 	//MAC_TEST_FF06 NODE端 收到2nd PTK --> 3rd PTK 并发ack
	mac_rxBufferlen = 47;
	 	mac_RxBuffer[0] = 0x32;
	 	mac_RxBuffer[1] = 0x04;
	 	mac_RxBuffer[2] = 0x02;
	 	mac_RxBuffer[3] = 0x00;
	 	mac_RxBuffer[4] = 0x01;
	 	mac_RxBuffer[5] = 0x02;
	 	mac_RxBuffer[6] = 0x02;

	 	mac_RxBuffer[7] = 0x01; 	//RecipientAddress
	 	mac_RxBuffer[8] = 0x23;
	 	mac_RxBuffer[9] = 0x45;
	 	mac_RxBuffer[10] = 0x67;
	 	mac_RxBuffer[11] = 0x89;
	 	mac_RxBuffer[12] = 0x01;
	 	mac_RxBuffer[13] = 0x02; 	//SenderAddress
	 	mac_RxBuffer[14] = 0x31;
	 	mac_RxBuffer[15] = 0x57;
	 	mac_RxBuffer[16] = 0x89;
	 	mac_RxBuffer[17] = 0x10;
	 	mac_RxBuffer[18] = 0x45;

		mac_RxBuffer[19] =0xc2;   //Sender Nonce
		mac_RxBuffer[20] =0xc1;
		mac_RxBuffer[21] =0x9e;
		mac_RxBuffer[22] =0x17;
		mac_RxBuffer[23] =0x38;
		mac_RxBuffer[24] =0x3f;
		mac_RxBuffer[25] =0xbd;
		mac_RxBuffer[26] =0x5d;
		mac_RxBuffer[27] =0xc3;
		mac_RxBuffer[28] =0x16;
		mac_RxBuffer[29] =0xf5;
		mac_RxBuffer[30] =0x57;
		mac_RxBuffer[31] =0xe4;
		mac_RxBuffer[32] =0x5;
		mac_RxBuffer[33] =0x11;
		mac_RxBuffer[34] =0xd7;

		mac_RxBuffer[35] =0x02;
		mac_RxBuffer[36] =0x00;

		mac_RxBuffer[37] =0x03;
		mac_RxBuffer[38] =0x27;
		mac_RxBuffer[40] =0x45;
		mac_RxBuffer[41] =0x96;
		mac_RxBuffer[42] =0x78;
		mac_RxBuffer[43] =0x28;
		mac_RxBuffer[44] =0x32;
		mac_RxBuffer[45] =0x76;

	 	mac_RxBuffer[46] = mac_FCS_LSB;
	 	mac_RxBuffer[47] = mac_FCS_MSB;

		mac_TasksPending.bit.dataIndication = 1;

	 	macBIB.hubAddr.v = 0x02;
		macBIB.hubFullAddr.v[0]=0x02;
		macBIB.hubFullAddr.v[1]=0x31;
		macBIB.hubFullAddr.v[2]=0x57;
		macBIB.hubFullAddr.v[3]=0x89;
		macBIB.hubFullAddr.v[4]=0x10;
		macBIB.hubFullAddr.v[5]=0x45;
		macBIB.SecurityLevel=0x02;
		macBIB.BANID.v = 0x02;
		messageNumber = 0x01;

		Nonce_R[0]=0x16;
		Nonce_R[1]=0x15;
		Nonce_R[2]=0x14;
		Nonce_R[3]=0x13;
		Nonce_R[4]=0x12;
		Nonce_R[5]=0x11;
		Nonce_R[6]=0x10;
		Nonce_R[7]=0x09;
		Nonce_R[8]=0x08;
		Nonce_R[9]=0x07;
		Nonce_R[10]=0x06;
		Nonce_R[11]=0x05;
		Nonce_R[12]=0x04;
		Nonce_R[13]=0x03;
		Nonce_R[14]=0x02;
		Nonce_R[15]=0x01;

		PTK_KMAC_2B[0]=0x03;
		PTK_KMAC_2B[1]=0x27;
		PTK_KMAC_2B[2]=0x45;
		PTK_KMAC_2B[3]=0x96;
		PTK_KMAC_2B[4]=0x78;
		PTK_KMAC_2B[5]=0x28;
		PTK_KMAC_2B[6]=0x32;
		PTK_KMAC_2B[7]=0x76;
#endif

#ifdef MAC_TEST_FF07
 	//MAC_TEST_FF07 HUB端  收到3rd PTK --> 上传confirm原语，并发ack
		mac_rxBufferlen = 47;
		 	mac_RxBuffer[0] = 0x32;
		 	mac_RxBuffer[1] = 0x04;
		 	mac_RxBuffer[2] = 0x03;
		 	mac_RxBuffer[3] = 0x00;
		 	mac_RxBuffer[4] = 0x02;
		 	mac_RxBuffer[5] = 0x01;
		 	mac_RxBuffer[6] = 0x02;

		 	mac_RxBuffer[7] = 0x02; 	//RecipientAddress
		 	mac_RxBuffer[8] = 0x31;
		 	mac_RxBuffer[9] = 0x57;
		 	mac_RxBuffer[10] = 0x89;
		 	mac_RxBuffer[11] = 0x10;
		 	mac_RxBuffer[12] = 0x45;
		 	mac_RxBuffer[13] = 0x01; 	//SenderAddress
		 	mac_RxBuffer[14] = 0x23;
		 	mac_RxBuffer[15] = 0x45;
		 	mac_RxBuffer[16] = 0x67;
		 	mac_RxBuffer[17] = 0x89;
		 	mac_RxBuffer[18] = 0x01;

			mac_RxBuffer[19] =0x01;   //Sender Nonce
			mac_RxBuffer[20] =0x02;
			mac_RxBuffer[21] =0x03;
			mac_RxBuffer[22] =0x04;
			mac_RxBuffer[23] =0x05;
			mac_RxBuffer[24] =0x06;
			mac_RxBuffer[25] =0x07;
			mac_RxBuffer[26] =0x08;
			mac_RxBuffer[27] =0x09;
			mac_RxBuffer[28] =0x10;
			mac_RxBuffer[29] =0x11;
			mac_RxBuffer[30] =0x12;
			mac_RxBuffer[31] =0x13;
			mac_RxBuffer[32] =0x14;
			mac_RxBuffer[33] =0x15;
			mac_RxBuffer[34] =0x16;

			mac_RxBuffer[35] =0x03;
			mac_RxBuffer[36] =0x00;

			mac_RxBuffer[37] =0x34;
			mac_RxBuffer[38] =0x26;
			mac_RxBuffer[40] =0x57;
			mac_RxBuffer[41] =0x01;
			mac_RxBuffer[42] =0x20;
			mac_RxBuffer[43] =0x10;
			mac_RxBuffer[44] =0x11;
			mac_RxBuffer[45] =0x29;

		 	mac_RxBuffer[46] = mac_FCS_LSB;
		 	mac_RxBuffer[47] = mac_FCS_MSB;

		 	mac_TasksPending.bit.dataIndication = 1;

			macBIB.nodeFullAddrTemp.v[0]=0x01;
			macBIB.nodeFullAddrTemp.v[1]=0x23;
			macBIB.nodeFullAddrTemp.v[2]=0x45;
			macBIB.nodeFullAddrTemp.v[3]=0x67;
			macBIB.nodeFullAddrTemp.v[4]=0x89;
			macBIB.nodeFullAddrTemp.v[5]=0x01;
			macBIB.SecurityLevel=0x02;
			messageNumber = 0x01;

			Nonce_I[0]=0x01;
			Nonce_I[1]=0x02;
			Nonce_I[2]=0x03;
			Nonce_I[3]=0x04;
			Nonce_I[4]=0x05;
			Nonce_I[5]=0x06;
			Nonce_I[6]=0x07;
			Nonce_I[7]=0x08;
			Nonce_I[8]=0x09;
			Nonce_I[9]=0x10;
			Nonce_I[10]=0x11;
			Nonce_I[11]=0x12;
			Nonce_I[12]=0x13;
			Nonce_I[13]=0x14;
			Nonce_I[14]=0x15;
			Nonce_I[15]=0x16;

			PTK_KMAC_3A[0]=0x34;
			PTK_KMAC_3A[1]=0x26;
			PTK_KMAC_3A[2]=0x57;
			PTK_KMAC_3A[3]=0x01;
			PTK_KMAC_3A[4]=0x20;
			PTK_KMAC_3A[5]=0x10;
			PTK_KMAC_3A[6]=0x11;
			PTK_KMAC_3A[7]=0x29;
#endif

#ifdef MAC_TEST_FF08
 	//MAC_TEST_FF08 NODE端 收到ack， 上传confirm原语
			mac_rxBufferlen = 9;
			 	mac_RxBuffer[0] = 0x00;
			 	mac_RxBuffer[1] = 0x10;
			 	mac_RxBuffer[2] = 0x00;
			 	mac_RxBuffer[3] = 0x00;
			 	mac_RxBuffer[4] = 0x01;
			 	mac_RxBuffer[5] = 0x02;
			 	mac_RxBuffer[6] = 0x02;

				mac_RxBuffer[7] = mac_FCS_LSB;
			 	mac_RxBuffer[8] = mac_FCS_MSB;

				mac_TasksPending.bit.dataIndication = 1;

			 	macBIB.hubAddr.v = 0x02;
				macBIB.hubFullAddr.v[0]=0x02;
				macBIB.hubFullAddr.v[1]=0x31;
				macBIB.hubFullAddr.v[2]=0x57;
				macBIB.hubFullAddr.v[3]=0x89;
				macBIB.hubFullAddr.v[4]=0x10;
				macBIB.hubFullAddr.v[5]=0x45;
				macBIB.SecurityLevel=0x02;
				mac_currentPacket.frameType_Subtype = 0x04;
#endif




	/*
	//HUB发送DISCONNECTION_REQUEST
	BYTE _payloadLength = 1;
	mac_SPIBuffer_Rx[0] = mac_PRIMITIVE_HEAD_LSB;
	mac_SPIBuffer_Rx[1] = mac_PRIMITIVE_HEAD_MSB;
	mac_SPIBuffer_Rx[2] = MLME_DISCONNECT_request;
	mac_SPIBuffer_Rx[3] = (_payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[4] = ((_payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[5] = 0x03;
	mac_SPIBuffer_Rx[6] = mac_PRIMITIVE_END_LSB;
	mac_SPIBuffer_Rx[7] = mac_PRIMITIVE_END_MSB;
#ifdef DEBUG
	printf("\n原语类型：MLME_DISCONNECT_request，负载长度：%d\n",_payloadLength);
	printf("RecipientID: %d\n", mac_SPIBuffer_Rx[5]);
#endif
	macBIB.nodeList[4].abbrAddr.v = 0x03;
	macBIB.nodeListNum++;
	macBIB.nodeList[4].fullAddr.v[0] = 0x11;
	macBIB.nodeList[4].fullAddr.v[1] = 0x22;
	macBIB.nodeList[4].fullAddr.v[2] = 0x33;
	macBIB.nodeList[4].fullAddr.v[3] = 0x44;
	macBIB.nodeList[4].fullAddr.v[4] = 0x55;
	macBIB.nodeList[4].fullAddr.v[5] = 0x66;

	mac_RxBuffer[0] = 0x00;
	mac_RxBuffer[1] = 0x10;
	mac_RxBuffer[2] = 0x00;
	mac_RxBuffer[3] = 0x00;
	mac_RxBuffer[4] = 0x02;
	mac_RxBuffer[5] = 0x03;
	mac_RxBuffer[6] = 0x02;
	mac_RxBuffer[7] = mac_PRIMITIVE_END_LSB;
	mac_RxBuffer[8] = mac_PRIMITIVE_END_MSB;
	mac_rxBufferlen = 9;
*/

	/*
	//DisconnectionrequestFrame帧到来
	mac_rxBufferlen = 21;
	mac_RxBuffer[0] = 0x02;
	mac_RxBuffer[1] = 0x0A;
	mac_RxBuffer[2] = 0x00;
	mac_RxBuffer[3] = 0x00;
	mac_RxBuffer[4] = 0x02;
	mac_RxBuffer[5] = 0x03;
	mac_RxBuffer[6] = 0x02;

	mac_RxBuffer[7] = 0x01; 	//RecipientAddress
	mac_RxBuffer[8] = 0x01;
	mac_RxBuffer[9] = 0x01;
	mac_RxBuffer[10] = 0x01;
	mac_RxBuffer[11] = 0x01;
	mac_RxBuffer[12] = 0x01;
	mac_RxBuffer[13] = 0x11; 	//SenderAddress
	mac_RxBuffer[14] = 0x22;
	mac_RxBuffer[15] = 0x33;
	mac_RxBuffer[16] = 0x44;
	mac_RxBuffer[17] = 0x55;
	mac_RxBuffer[18] = 0x66;
	mac_RxBuffer[19] = mac_PRIMITIVE_END_LSB;
	mac_RxBuffer[20] = mac_PRIMITIVE_END_MSB;

	macBIB.nodeListNum++;
	macBIB.nodeList[4].abbrAddr.v = 0x03;
	macBIB.nodeList[4].fullAddr.v[0] = 0x11;
	macBIB.nodeList[4].fullAddr.v[1] = 0x22;
	macBIB.nodeList[4].fullAddr.v[2] = 0x33;
	macBIB.nodeList[4].fullAddr.v[3] = 0x44;
	macBIB.nodeList[4].fullAddr.v[4] = 0x55;
	macBIB.nodeList[4].fullAddr.v[5] = 0x66;

	mac_TasksPending.bit.dataIndication = 1;*/

	/*
	//NODE发送DISCONNECTION_REQUEST
	BYTE _payloadLength = 1;
	mac_SPIBuffer_Rx[0] = mac_PRIMITIVE_HEAD_LSB;
	mac_SPIBuffer_Rx[1] = mac_PRIMITIVE_HEAD_MSB;
	mac_SPIBuffer_Rx[2] = MLME_DISCONNECT_request;
	mac_SPIBuffer_Rx[3] = (_payloadLength & 0x00FF);
	mac_SPIBuffer_Rx[4] = ((_payloadLength >> 8) & 0x00FF);
	mac_SPIBuffer_Rx[5] = 0x02;
	mac_SPIBuffer_Rx[6] = mac_PRIMITIVE_END_LSB;
	mac_SPIBuffer_Rx[7] = mac_PRIMITIVE_END_MSB;
#ifdef DEBUG
	printf("\n原语类型：MLME_DISCONNECT_request，负载长度：%d\n",_payloadLength);
	printf("RecipientID: %d\n", mac_SPIBuffer_Rx[5]);
#endif
	macBIB.abbrAddr.v = 0x03;
	macBIB.fullAddr.v[0] = 0x11;
	macBIB.fullAddr.v[1] = 0x22;
	macBIB.fullAddr.v[2] = 0x33;
	macBIB.fullAddr.v[3] = 0x44;
	macBIB.fullAddr.v[4] = 0x55;
	macBIB.fullAddr.v[5] = 0x66;
	macBIB.hubAddr.v = 0x02;
	macBIB.hubFullAddr.v[0] = 0x01;
	macBIB.hubFullAddr.v[1] = 0x01;
	macBIB.hubFullAddr.v[2] = 0x01;
	macBIB.hubFullAddr.v[3] = 0x01;
	macBIB.hubFullAddr.v[4] = 0x01;
	macBIB.hubFullAddr.v[5] = 0x01;

	mac_RxBuffer[0] = 0x00;
	mac_RxBuffer[1] = 0x10;
	mac_RxBuffer[2] = 0x00;
	mac_RxBuffer[3] = 0x00;
	mac_RxBuffer[4] = 0x03;
	mac_RxBuffer[5] = 0x02;
	mac_RxBuffer[6] = 0x02;
	mac_RxBuffer[7] = mac_PRIMITIVE_END_LSB;
	mac_RxBuffer[8] = mac_PRIMITIVE_END_MSB;
	mac_rxBufferlen = 9;

*/

	/*
	//DisconnectionrequestFrame帧到NODE
	mac_rxBufferlen = 21;
	mac_RxBuffer[0] = 0x02;
	mac_RxBuffer[1] = 0x0A;
	mac_RxBuffer[2] = 0x00;
	mac_RxBuffer[3] = 0x00;
	mac_RxBuffer[4] = 0x03;
	mac_RxBuffer[5] = 0x02;
	mac_RxBuffer[6] = 0x02;

	mac_RxBuffer[7] = 0x11; 	//RecipientAddress
	mac_RxBuffer[8] = 0x22;
	mac_RxBuffer[9] = 0x33;
	mac_RxBuffer[10] = 0x44;
	mac_RxBuffer[11] = 0x55;
	mac_RxBuffer[12] = 0x66;
	mac_RxBuffer[13] = 0x01; 	//SenderAddress
	mac_RxBuffer[14] = 0x01;
	mac_RxBuffer[15] = 0x01;
	mac_RxBuffer[16] = 0x01;
	mac_RxBuffer[17] = 0x01;
	mac_RxBuffer[18] = 0x01;
	mac_RxBuffer[19] = mac_PRIMITIVE_END_LSB;
	mac_RxBuffer[20] = mac_PRIMITIVE_END_MSB;

	macBIB.abbrAddr.v = 0x03;
	macBIB.fullAddr.v[0] = 0x11;
	macBIB.fullAddr.v[1] = 0x22;
	macBIB.fullAddr.v[2] = 0x33;
	macBIB.fullAddr.v[3] = 0x44;
	macBIB.fullAddr.v[4] = 0x55;
	macBIB.fullAddr.v[5] = 0x66;
	macBIB.hubAddr.v = 0x02;
	macBIB.hubFullAddr.v[0] = 0x01;
	macBIB.hubFullAddr.v[1] = 0x01;
	macBIB.hubFullAddr.v[2] = 0x01;
	macBIB.hubFullAddr.v[3] = 0x01;
	macBIB.hubFullAddr.v[4] = 0x01;
	macBIB.hubFullAddr.v[5] = 0x01;

	mac_TasksPending.bit.dataIndication = 1;
	*/

	//进入程序大循环
	PrimitiveTasks();
}
