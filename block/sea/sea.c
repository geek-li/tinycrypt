//#define PRINT

//#include "tools.h"

#include <stdint.h>

#define SIZE 64//n : plaintext size, key size. k*6B
#define B 32 //processor (or word) size.
#define NB (SIZE/(2*B)) //nb = n/2b : number of words per Feistel branch.
#define NBROUND 51 // odd number

#define MASK 0xFFFFFFFF

/**********************************************************************/
 void XOR(uint32_t x[NB],uint32_t y[NB])
{
	uint32_t i;
	for(i=0;i<NB;i++)
	{
		x[i]^=y[i];
	}
	return;
}

 void Sub(uint32_t x[NB])
{
	uint32_t i;
	for(i=0;i<(SIZE/(6*B));i++)
	{
		x[3*i]   ^= x[3*i+1] & x[3*i+2];
		x[3*i+1] ^= x[3*i]   & x[3*i+2];
		x[3*i+2] ^= x[3*i+1] | x[3*i];
	}
	return;
}

 void WordRot(uint32_t x[NB])
{
	uint32_t i;
	uint32_t temp=x[NB-1];
	for(i=NB-1;i>0;i--)
	{
		x[i]=x[i-1];
	}
	x[0]=temp;
	return;
}

 void InvWordRot(uint32_t x[NB])
{
	uint32_t i;
	uint32_t temp=x[0];
	for(i=0;i<NB-1;i++)
	{
		x[i]=x[i+1];
	}
	x[NB-1]=temp;
	return;
}

 void BitRot(uint32_t x[NB])
{
	uint32_t i;
	for(i=0;i<NB/3;i++)
	{
		x[3*i]=(x[3*i]>>1)^(x[3*i]<<(B-1));
		x[3*i+2]=(x[3*i+2]<<1)^(x[3*i+2]>>(B-1));
	}
	return;
}

 void Add(uint32_t x[NB],uint32_t y[NB])
{
	uint32_t i;
	for(i=0;i<NB;i++)
	{
		x[i]=(x[i]+y[i])&MASK;
	}
	return;
}

/**********************************************************************/
 void fk(uint32_t kr[NB],uint32_t kl[NB],uint32_t krDest[NB],uint32_t klDest[NB],uint32_t c[NB])
{
	uint32_t i;
	for(i=0;i<NB;i++) krDest[i]=kr[i];
	for(i=0;i<NB;i++) klDest[i]=kr[i];

	Add(krDest,c);
	Sub(krDest);
	BitRot(krDest);
	WordRot(krDest);
	XOR(krDest,kl);
	return;
}

 void fe(uint32_t r[NB],uint32_t l[NB],uint32_t k[NB])
{
	uint32_t temp[NB],i;
	for(i=0;i<NB;i++) temp[i]=r[i];

	Add(r,k);
	Sub(r);
	BitRot(r);
	WordRot(l);
	XOR(r,l);

	for(i=0;i<NB;i++) l[i]=temp[i];

	return;
}

 void fd(uint32_t r[NB],uint32_t l[NB],uint32_t k[NB])
{
	uint32_t temp[NB],i;
	for(i=0;i<NB;i++) temp[i]=r[i];

	Add(r,k);
	Sub(r);
	BitRot(r);
	XOR(r,l);
	InvWordRot(r);
	for(i=0;i<NB;i++) l[i]=temp[i];

	return;
}

 void KeySchedul(uint32_t mkey[2*NB],uint32_t rkey[NBROUND][2*NB])
{
	uint32_t i,j,temp,c[NB];
	for(i=1;i<NB;i++) c[i]=0;
	for(i=0;i<2*NB;i++) rkey[0][i]=mkey[i];
	for(i=1;i<=(NBROUND>>2);i++)
	{
		c[0]=i;
		//[KLi , KRi ] = FK (KLi−1 , KRi−1 , C(i));
		fk(rkey[i-1],rkey[i-1]+3,rkey[i],rkey[i]+3,c);
	}
	
	for(j=0;j<NB;j++)
	{
		temp=rkey[NBROUND>>2][j];
		rkey[NBROUND>>2][j]=rkey[NBROUND>>2][j+3];
		rkey[NBROUND>>2][j+3]=temp;
	}
	
	for(;i<NBROUND;i++)
	{
		c[0]=NBROUND-i;
		//[KLi , KRi ] = FK (KLi−1 , KRi−1 , C(r − i));
		fk(rkey[i-1],rkey[i-1]+3,rkey[i],rkey[i]+3,c);
	}
	return;
}

 void Decrypt(uint32_t state[2*NB],uint32_t rkey[NBROUND][2*NB])
{
	uint32_t i,temp;
	
	for(i=NBROUND;i>((NBROUND+1)>>2);i--)
	{
		fd(state,state+3,rkey[i-1]+3);
	}
	for(;i>=1;i--)
	{
		fd(state,state+3,rkey[i-1]);
	}
	for(i=0;i<NB;i++)
	{
		temp=state[i];
		state[i]=state[i+3];
		state[i+3]=temp;
	}

	return;
}

 void Encrypt(uint32_t state[2*NB],uint32_t rkey[NBROUND][2*NB])
{
	uint32_t i,temp;
	for(i=1;i<=((NBROUND+1)>>2);i++)
	{
		fe(state,state+3,rkey[i-1]);	
	}
	for(;i<=NBROUND;i++)
	{
		fe(state,state+3,rkey[i-1]+3);		
	}
		
	for(i=0;i<NB;i++)
	{
		temp=state[i];
		state[i]=state[i+3];
		state[i+3]=temp;
	}	
	return;
}

