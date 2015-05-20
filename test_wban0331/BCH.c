#ifdef DEBUG
#include <stdio.h>
#endif
#include"stdlib.h"
#include"math.h"
extern int PSDU_LEN;
#define uc unsigned char

//�������ܣ�ֻ�Ǽ���PSDU���ݾ���BCH����֮��ĳ���
//���룺PSDUԭʼ���ݳ��ȣ�����ָ������չָ��
//�����BCH����֮������ݳ���

int PSDUcodelen(int N_psdu,int M,int SpreadFactor_R)
{
	int N_CW, N_pad, N_shorten, r1, r2;
	int Coding_len;
	N_CW=(int)ceil((double)N_psdu/51);
	N_shorten = N_CW*51-N_psdu;
	r1=N_CW*63-N_shorten;
	r2=(int)ceil(log(M)/log(2));
	N_pad=r2*(int)ceil((double)r1/(double)r2)-r1;//ȷ��BCH�������

	Coding_len=(r1+N_pad)*SpreadFactor_R;
	return Coding_len;

}

//***************BCHDecoding_PSDU***************//
//���룺�������63bits������������
//�����BCH������������ܷ���������
void Decoding(uc* R)
{
	int i,j,s1,s2,s3=-1,s4,na,nb,nd;
	uc S1[6]={0},S2[6]={0},S3[6]={0},S4[6]={0},Sigma1[6];
	int Sigma2[6]={-1};
	uc A1[6],A2[6],A[6],B1[6],B2[6],B[6],D1[6],D2[6],D[6];
	int a1,a2,b1,b2,d1,d2,sigma1,sigma2,sa,sb;
	uc z[6],z1[6],z2[6],bin[6][63]={0},E[63]={0};
	uc gen[2]={0};
	int o=0;
	uc x[63]={33,49,57,61,63,62,31,46,23,42,21,43,52,26,13,39,50,25,45,55,58,29,47,54,27,44,22,11,36,18,9,37,51,56,28,14,7,34,17,41,53,59,60,30,15,38,19,40,20,10,5,35,48,24,12,6,3,32,16,8,4,2,1};
	for (i=0;i<=62;i++)
	{
		for (j=5;j>=0;j--)
		{
			bin[j][i]=x[i]%2;
			x[i]/=2;
		}
	}
	for(j=0;j<=62;j++)//�������ʽS=[S1,S3]
	{
		if (R[j]==1)
		{
			for (i=0;i<=5;i++)
			{
				S1[i]^=bin[i][j];//S1
				S2[i]^=bin[i][62-(62-j)*2%63];
				S3[i]^=bin[i][62-(62-j)*3%63];//S3
				S4[i]^=bin[i][62-(62-j)*4%63];
			}
		}
	}
	for (i=0;i<=62;i++)//����ʽS��������ָ��n
	{
		if (S1[0]==bin[0][i]&&S1[1]==bin[1][i]&&S1[2]==bin[2][i]&&S1[3]==bin[3][i]&&S1[4]==bin[4][i]&&S1[5]==bin[5][i])
			s1=62-i;
		if (S2[0]==bin[0][i]&&S2[1]==bin[1][i]&&S2[2]==bin[2][i]&&S2[3]==bin[3][i]&&S2[4]==bin[4][i]&&S2[5]==bin[5][i])
			s2=62-i;
		if (S3[0]==bin[0][i]&&S3[1]==bin[1][i]&&S3[2]==bin[2][i]&&S3[3]==bin[3][i]&&S3[4]==bin[4][i]&&S3[5]==bin[5][i])
			s3=62-i;
		if (S4[0]==bin[0][i]&&S4[1]==bin[1][i]&&S4[2]==bin[2][i]&&S4[3]==bin[3][i]&&S4[4]==bin[4][i]&&S4[5]==bin[5][i])
			s4=62-i;
	}
	if (S1[0]!=0||S1[1]!=0||S1[2]!=0||S1[3]!=0||S1[4]!=0||S1[5]!=0) //û�д���(S1=0,S3=0),�޷�����(S1=0,S3!=0)
	{
		if (s3==-1)//S3=0,A!=0������������
		{
			a1=(s2*2)%63;
			b1=(s1+s4)%63;
			d1=(s2+s4)%63;
			sigma1=(b1-a1+63)%63;
			sigma2=(d1-a1+63)%63;
			for (i=0;i<=5;i++)
			{
				Sigma2[i]=bin[i][62-sigma2];
				Sigma1[i]=bin[i][62-sigma1];
			}
		}
		else
		{
			a1=(s2*2)%63;
			a2=(s1+s3)%63;
			for (i=0;i<=5;i++)
			{
				A1[i]=bin[i][62-a1];
				A2[i]=bin[i][62-a2];
				A[i]=A1[i]^A2[i];
			}
			if(A[0]==0&&A[1]==0&&A[2]==0&&A[3]==0&&A[4]==0&&A[5]==0)//����ʽA=0���������С��2
			{
				sigma1=(s3-s2+63)%63;
				sigma2=-1;
				for (i=0;i<=5;i++)
					Sigma1[i]=bin[i][62-sigma1];
			}
			else
			{
				b1=(s2+s3)%63;
				b2=(s1+s4)%63;
				d1=(s2+s4)%63;
				d2=(s3*2)%63;
				for (i=0;i<=5;i++)
				{
					D1[i]=bin[i][62-d1];
					D2[i]=bin[i][62-d2];
					D[i]=D1[i]^D2[i];
					B1[i]=bin[i][62-b1];
					B2[i]=bin[i][62-b2];
					B[i]=B1[i]^B2[i];
				}
				for (i=0;i<=62;i++)
				{
					if (A[0]==bin[0][i]&&A[1]==bin[1][i]&&A[2]==bin[2][i]&&A[3]==bin[3][i]&&A[4]==bin[4][i]&&A[5]==bin[5][i])
						na=62-i;
					if (D[0]==bin[0][i]&&D[1]==bin[1][i]&&D[2]==bin[2][i]&&D[3]==bin[3][i]&&D[4]==bin[4][i]&&D[5]==bin[5][i])
						nd=62-i;
					if (B[0]==bin[0][i]&&B[1]==bin[1][i]&&B[2]==bin[2][i]&&B[3]==bin[3][i]&&B[4]==bin[4][i]&&B[5]==bin[5][i])
						nb=62-i;
				}
				sigma1=(nb-na+63)%63;//x��ϵ��Sigma1
				sigma2=(nd-na+63)%63;//x^2��ϵ��Sigma2
				for (i=0;i<=5;i++)
				{
					Sigma2[i]=bin[i][62-sigma2];
					Sigma1[i]=bin[i][62-sigma1];
				}
			}
		}
		for (j=0;j<=62;j++)//Ǯ���������λ��,��λ����
		{
			if (sigma2==-1)//sigma2����0����һ������
			{
				sa=(j+1+sigma1)%63;
				for (i=0;i<=5;i++)
					z[i]=bin[i][62-sa];
			}
			else
			{
				sa=(j+1+sigma1)%63;
				sb=(j*2+2+sigma2)%63;
				for (i=0;i<=5;i++)
				{
					z1[i]=bin[i][62-sa];
					z2[i]=bin[i][62-sb];
					z[i]=z1[i]^z2[i];
				}
			}
			if (z[0]==0&&z[1]==0&&z[2]==0&&z[3]==0&&z[4]==0&&z[5]==1)//��j+1λ����
			{
				o+=1;//���������1
				gen[o-1]=j+1;
			}
		}
		for (i=0;i<=62;i++)//����E����C=R+E
		{
			for (j=0;j<o;j++)
			{
				if (i+1==gen[j])
					E[i]=1;
			}
			R[i]^=E[i];
		}
	}
}

//***************BCHEncoding_PHYH***************//
//���룺�������19bits������������
//�����BCH���������
void BCHEncoding_PHYH(uc* data)
{
	int i,n=32;
	uc x[63]={0};
	uc g[13]={1,0,1,0,1,0,0,1,1,1,0,0,1};//BCH(63,51)��ԭ����ʽϵ��(��->��)
	for (i=32;i<=50;i++)
		{
			x[i]=data[i-32];
		}
	while(n<=63-13)
	{
		if (x[n]==1)//�ж�������Ϣ�����Ƿ�Ϊ0����Ϊ0������ж���һ���أ���������������
		{
			for(i=0;i<=12;i++)
				x[i+n]=x[i+n]^g[i];//���
		}
		n++;
	}
	for (i=19;i<=30;i++)
		data[i]=x[i+32];
}

//***************BCHDEecoding_PHYH***************//
//���룺�������31bits������������
//�����BCH�������Ϣ
uc* BCHDecoding_PHYH(uc* data_in)
{
	uc R[63]={0};
	int i;
	uc *data_out;

	for (i=32;i<=62;i++)
		R[i]=data_in[i-32];
	Decoding(R);

	data_out=(uc*)calloc(19,sizeof(uc));
	for (i=32;i<=50;i++)
		data_out[i-32]=R[i];
	return data_out;
}


//***************BCHEncoding_PSDU***************//
//���룺MAC������msg_in,���䳤��N_psdu������ָ��M
//���������������code_out ,���䳤��N_psdu_code
uc* BCHEncoding_PSDU(uc* msg_in,int M)
{
	int N_CW, N_shorten, N_spcw, N_row, N_pad;
	int k=51, n=63, t, m, i, j, r1, r2;
	uc g[13]={1,0,1,0,1,0,0,1,1,1,0,0,1}, y[13]={0}, x[63]={0};//BCH(63,51)��ԭ����ʽϵ��(��->��)
	uc **p,*code_out;
	
	N_CW=(int)ceil((double)PSDU_LEN/k);
	N_shorten = N_CW*k-PSDU_LEN;
	N_spcw = N_shorten / N_CW;
	N_row = N_shorten % N_CW;
	r1 = N_CW*n - N_shorten;
	r2=(int)ceil(log(M)/log(2));
	if (r1%r2==0)
		N_pad=0;
	else
		N_pad=r2*(r1/r2+1)-r1;

	PSDU_LEN = N_CW*n-N_shorten+N_pad;

	p=(uc**)calloc(N_CW,sizeof(uc *));//�����ڴ�ռ䣬��PSDU��Ϣ������
	for (i=0;i<=N_CW-1;i++)
		p[i]=(uc *)calloc(n,sizeof(uc));

	if (N_shorten==0)
	{
		m=0;
		for (i=0;i<=N_CW-1;i++)
		{
			for (j=0;j<=50;j++)
			{
				p[i][j]=msg_in[m];
				m++;
			}
		}
	}
	else
	{
		m=0;
		for (i=0;i<=N_row-1;i++)
		{
			for (j=0;j<=49-N_spcw;j++)
			{
				p[i][j]=msg_in[m];
				m++;
			}
		}
		for (i=N_row;i<=N_CW-1;i++)
		{
			for (j=0;j<=50-N_spcw;j++)
			{
				p[i][j]=msg_in[m];
				m++;
			}
		}
	}

	for (i=0;i<=N_CW-1;i++)//��ÿһ��PSDU��Ϣ���ݽ���BCH����
	{
		for (j=0;j<=62;j++)//�ݴ��������Ϣ
			x[j]=p[i][j];
		for (j=0;j<=50;j++)//������·�����У��λ��Ϣ
		{
			if (x[j]!=0)
			{
				for (t=0;t<=12;t++)
				{
					y[t]=x[j+t]^g[t];
					x[j+t]=y[t];
				}
			}
		}
		for (j=0;j<=62;j++)//��������Ϣ����
		{
			p[i][j]^=x[j];
		}
	}

	code_out=(uc*)calloc(PSDU_LEN,sizeof(uc));
	m=0;
	if (N_shorten==0)//ȥ��Shortened Bits����������������bits�洢��code_out�����
	{
		for (i=0;i<=N_CW-1;i++)
		{
			for (j=0;j<=62;j++)
			{
				code_out[m]=p[i][j];
				m++;
			}
		}
	}
	else
	{
		for (i=0;i<=N_row-1;i++)
		{
			for (j=0;j<=49-N_spcw;j++)
			{
				code_out[m]=p[i][j];
				m++;
			}
			for (j=51;j<=62;j++)
			{
				code_out[m]=p[i][j];
				m++;
			}
		}
		for (i=N_row;i<=N_CW-1;i++)
		{
			for (j=0;j<=50-N_spcw;j++)
			{
				code_out[m]=p[i][j];
				m++;
			}
			for (j=51;j<=62;j++)
			{
				code_out[m]=p[i][j];
				m++;
			}
		}
	}
	for (i=0;i<=N_CW-1;i++)
		free(p[i]);
	free(p);
	return(code_out);
}

//***************BCHDecoding_PSDU***************//
//���룺���ն˽���������code_in,���䳤��N_psdu_code,����ָ��M 
//��������ն�BCH������PSDU��Ϣ
uc* BCHDecoding_PSDU(uc* code_in,int N_psdu_code,int M)
{
	uc **p,*msg_out;
	int N_CW,N_shorten,N_row,N_spcw,N_pad;
	int k=51,n=63,i,j,m,t,r1,r2;
	
	N_CW=(int)ceil((double)N_psdu_code/n);
	N_shorten=N_CW*n-N_psdu_code;
	N_spcw=N_shorten/N_CW;
	N_row=N_shorten%N_CW;
	r1=N_CW*n-N_shorten;
	r2=(int)ceil(log(M)/log(2));
	if (r1%r2==0)
		N_pad=0;
	else
		N_pad=r2*(r1/r2+1)-r1;
	N_psdu_code-=N_pad;//ȷ��BCH�������
	
	p=(uc**)calloc(N_CW,sizeof(uc *));//�����ڴ�ռ�
	for (i=0;i<=N_CW-1;i++)
		p[i]=(uc *)calloc(n,sizeof(uc));
	msg_out=(uc*)calloc(N_CW*k-N_shorten,sizeof(uc));

	if (N_shorten==0)//��ȫƽ��
	{
		m=0;t=0;
		for (i=0;i<=N_CW-1;i++)
		{
			for (j=0;j<=62;j++)
			{
				p[i][j]=code_in[m];
				m++;
			}
			Decoding(*(p+i));
			for (j=0;j<=50;j++)
			{
				msg_out[t]=p[i][j];
				t++;
			}
		}
	}
	else
	{
		m=0;t=0;
		for (i=0;i<=N_row-1;i++)
		{
			for (j=0;j<=49-N_spcw;j++)//��Ϣλ
			{
				p[i][j]=code_in[m];
				m++;
			}
			for (j=50-N_spcw;j<=50;j++)//shorten bits
			{
				p[i][j]=0;
			}
			for (j=51;j<=62;j++)//У��λ
			{
				p[i][j]=code_in[m];
				m++;
			}
			Decoding(*(p+i));
			for (j=0;j<=49-N_spcw;j++)
			{
				msg_out[t]=p[i][j];
				t++;
			}
		}
		for (i=N_row;i<=N_CW-1;i++)
		{
			for (j=0;j<=50-N_spcw;j++)
			{
				p[i][j]=code_in[m];
				m++;
			}
			for (j=51-N_spcw;j<=50;j++)
			{
				p[i][j]=0;
			}
			for (j=51;j<=62;j++)
			{
				p[i][j]=code_in[m];
				m++;
			}
			Decoding(*(p+i));
			for (j=0;j<=50-N_spcw;j++)
			{
				msg_out[t]=p[i][j];
				t++;
			}
		}
	}
	for (i=0;i<=N_CW-1;i++)
		free(p[i]);
	free(p);
	return (msg_out);
	
}
//**********************************************//

