#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#define l_int_size 64

void datashow(uint64_t *data,int size)
{
    int i=0;

    for(i=0;i<size;i++)
    {
        if(i==0)
        {
            printf("\n64:[");
        }
        printf("0x%"PRIx64,data[i]);
        if(i<size-1)
        {
            printf(", ");
        }
        else
        {
            printf("]");
        }
    }
}


int archivo(char archivo[30],uint64_t *txt, int *size)
{
    FILE *arch;

    int i=0,j=0,k=0;
    int t=0;
    int out=0;
    //uint64_t *texto;
    uint8_t *resto;

    arch=fopen(archivo,"rb");

    if(arch!=NULL)
    {
        fseek(arch,0,SEEK_END);
        j=ftell(arch);        

        t=((j*8)%64)/8;   
        k=8*j/64;

        rewind(arch);
        //printf("%d,%d,%d\ta\n",j,k,t);
        txt=realloc(txt,(k+1)*sizeof(uint64_t));
        resto=(uint8_t *) malloc(t*sizeof(uint8_t));

        fread(txt,sizeof(uint64_t),k,arch);
        fread(resto,sizeof(uint8_t),t,arch);

        if (t>0)
        {
            k++;
        }
        for(i=0;i<t;i++)
        {
            txt[k-1]=(txt[k-1]|resto[i]);
            if(i<t-1)
            {
                txt[k-1]=txt[k-1]<<8;
            }
        }
        
       *size=k;
        out=0;
    }
    else
    {
        //printf("No se encuentra el archivo");
        out=1;
    }
    fclose(arch);
    return out;
    
}


void hex2bin(uint64_t *data,int size,int **val)
{
    uint64_t a=1;
    uint64_t b=0;
    int i=0,j=0;
    uint64_t temp;
    int cont=0;
    
    printf("ok\n" );
       
    for(i=0;i<size;i++)
    {
        temp=data[i];
        printf("0x%"PRIx64"\n",temp);
    
        
        for(j=0;j<l_int_size;j++)
        {
            b=temp & a;
            temp=temp>>1;
            val[i][l_int_size-j-1]=(int) b;
        }
    }


    printf("matriz de datos:\n");
    for(i=0;i<size;i++)
    {   
        printf("[\t");
        for(j=0;j<l_int_size;j++)
        {
            printf("%d",val[i][j]);
            if(!((j+1)%8) && j!=0)
            {
                printf("\n\t");
            }
            if(!((j+1)%4) && j!=0)
            {
                printf(" ");
            }
        }
        printf("]\n");
    }


}




int main()
{
    char arch[]="archivo.txt";
    uint64_t *data;
    int **bin;
    int size=0;

    bin=(int **) malloc(sizeof(int *));
    data=(uint64_t *) malloc(sizeof(uint64_t));

    archivo(arch,data,&size);

    
    if(data!=NULL)
    {    
        datashow(data,size);

        bin=(int**) malloc(size*sizeof(int *));
        for(int i=0;i<size;i++)
        {
            bin[i]=(int*)malloc(l_int_size*sizeof(int));
        }


        hex2bin(data,size,bin);
    }
    else
    {
        printf("NULL");
    }

    printf("\n");

    free(data);
    return 0;
}