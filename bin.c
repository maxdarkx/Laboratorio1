#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>


#define SIZE 64
#define OVER 9223372036854775807
int convertir(uint64_t a,int mat[SIZE])
{
	
	int i;
	int mod;
	uint64_t n=a;
	if ((a-OVER)<0)
	{
		mod=1;
	}
	else
	{
		mod=-1;
	}

	for(i=0;i<SIZE;i++)
	{
		mat[i]=0;
	}

	i=0;
	do
	{
		if(n%2==0)
		{
			n=n/2;
			mat[i]=0;
		}
		else
		{
			n=n/2;
			mat[i]=1;
			
		}
		i++;
		printf("%"SCNd64", ",n);
	}while(i<SIZE && n>0);
	printf("\n");
	return mod*i;
}


int main(void)
{
	uint64_t ent;
	int bin[SIZE];
	int j=0;
	int s;
	printf("Introduzca el entero a convertir:\n");
	scanf("%"SCNd64,&ent);
	s=convertir(ent,bin);

	if(s<0)
	{
		printf("\nEl numero sobrepasa los 64 bits (OVERFLOW)\n");
		s=-s;
	}
	printf("El entero convertido es [%d bits]:\n[",s+1);
	for(j=s-1;j>=0;j--)
	{
		printf("%d",bin[j]);
		if(j>0)
		{
			printf(",");
		}
	}
	printf("]\n");
	return 0;
}
