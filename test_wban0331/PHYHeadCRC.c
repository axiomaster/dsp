//PHYHeadCRCCoding
//输入:已预留校验位空间的数组地址(数组>19)
//输出头信息+4bit校验信息
//生成多项式为：1+X+X^4 
#define uc unsigned char
void PHYHeadCRCCoding(uc* data)
{
int i,k,j;
uc reg[4]={1,1,1,1},temp;
for(i=15;i<19;i++)
    data[i]=0;     //最后四位校验位归零
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
//输入：带有校验位的序列，序列长度
//输出：校验通过1；校验失败0
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
		res=1;  //校验成功
	else
		res=0;  //校验失败
	return res;
}
