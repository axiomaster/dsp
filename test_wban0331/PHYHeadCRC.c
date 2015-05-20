//PHYHeadCRCCoding
//����:��Ԥ��У��λ�ռ�������ַ(����>19)
//���ͷ��Ϣ+4bitУ����Ϣ
//���ɶ���ʽΪ��1+X+X^4 
#define uc unsigned char
void PHYHeadCRCCoding(uc* data)
{
int i,k,j;
uc reg[4]={1,1,1,1},temp;
for(i=15;i<19;i++)
    data[i]=0;     //�����λУ��λ����
for(k=0;k<19;k++)
	{
		temp=reg[0]^data[k];
		reg[3]=reg[3]^reg[0];
		for(j=0;j<3;j++) reg[j]=reg[j+1];
		reg[j]=temp;
	}
for(i=15;i<19;i++) data[i]=(reg[i-15]+1)%2;
}
//PHYHeadCRCCheck
//���룺����У��λ�����У����г���
//�����У��ͨ��1��У��ʧ��0
int PHYHeadCRCCheck(uc* data,int N)
{
	uc reg[4]={1,1,1,1},i,k,sum=0,res,temp;
	for(i=N-4;i<N;i++)
		data[i]=(data[i]+1)%2;
	//for(i=0;i<19;i++)
			//printf("%d ",data[i]);
		//printf("\n");
	for(k=0;k<N;k++)
	{
		temp=reg[0]^data[k];
		reg[3]=reg[3]^reg[0];
		//reg[3]=reg[3]^temp;
		for(i=0;i<3;i++)
			reg[i]=reg[i+1];
		reg[i]=temp;
	}
	for(i=0;i<4;i++)
		sum=sum+reg[i];
	if(0==sum)
		res=1;  //У��ɹ�
	else
		res=0;  //У��ʧ��
	return res;
}
