#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

int main()
{
    FILE *arch;

    char data;
    int j=0;
    int k=0;
    int i=0;
    int t=0;
    int out=0;
    fpos_t punto;   //lugar del archivo que esta siendo recorrido
    int error,error1;
    uint64_t *texto;
    uint8_t *resto;

    arch=fopen("archivo.txt","rb");

    if(arch!=NULL)
    {
    	fseek(arch,0,SEEK_END);
        j=ftell(arch);        

    	t=((j*8)%64)/8;   
    	k=8*j/64;

        printf("cantidad de bytes=%d, bloques de 64 bits leidos= %d, bloques de ocho bits faltantes=%d\n\n",j,k,t);

        rewind(arch);


        texto=(uint64_t *) malloc((k+1)*sizeof(uint64_t));
        resto=(uint8_t *) malloc(t*sizeof(uint8_t));

        fread(texto,sizeof(uint64_t),k,arch);
        fread(resto,sizeof(uint8_t),t,arch);

        if (t>0)
        {
            k++;
        }
        for(i=0;i<t;i++)
        {
            texto[k-1]=(texto[k-1]|resto[i]);
            if(i<t-1)
            {
                texto[k-1]=texto[k-1]<<8;
            }
            printf("0x%"PRIx64,texto[k-1]);
        }



        
        for(i=0;i<k;i++)
        {
            if(i==0)
            {
                printf("\n64:[");
            }
            printf("0x%"PRIx64,texto[i]);
            if(i<k)
            {
                printf(", ");
            }
            else
            {
                printf("]");
            }
        }        
    }
    else
    {
        printf("No se encuentra el archivo");
    }
    free(texto);
    fclose(arch);
    return 0;
}