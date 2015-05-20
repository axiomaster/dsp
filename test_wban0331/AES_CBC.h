/*void AES_Encrypt(unsigned char (*state)[4]);
unsigned char Sbox(unsigned char row, unsigned char column);
void Keyexpansion(unsigned char *cipher,unsigned char **expandedkey);
void Rotword(unsigned char *word);
void Subword(unsigned char *rotword);
void AddRoundkey(unsigned char (*state)[4],unsigned char (*temp)[4]);
void ByteSubs(unsigned char (*state)[4]);
void ShiftRows(unsigned char (*state)[4]);
void MixColumns(unsigned char (*state)[4]);
unsigned char Xtime(unsigned char elem,unsigned char num);*/
                                                    

//void AES_Decrypt(unsigned char (*state)[4]);                                                                                                                                                                                                                                                                                                                                                                                  unsigned char InvSbox(unsigned char row, unsigned char column);
//void InvSubBytes(unsigned char (*state)[4]);
//void InvShiftRows(unsigned char (*state)[4]);
//void InvMixColumns(unsigned char (*state)[4]);

void XOR(unsigned char* a, unsigned char* b, int nBytes);


void  CBC_encrypt(
	unsigned char *p,
	unsigned char *c,
	unsigned char CipherKey[16],
	int nBytesOfp
	);
