#include <stdio.h>
#include <stdint.h>

int main()
{
    FILE *arch;
    uint64_t texto[10][25];
    uint64_t taux[25];
    char data;
    int j=0;
    int k=0;
    int i=0;
    int t=0;
    int out=0;
    fpos_t punto;   //lugar del archivo que esta siendo recorrido
    int error,error1;
    uint8_t resto[200];
    arch=fopen("archivo.txt","rb");

    if(arch!=NULL)
    {
    	while (feof(arch)==0)
    	{
    		fread(texto,sizeof(char),1,arch);     //Buscar el tamaño del archivo en bytes y guardarlo en j
    		j++; //longitud del archivo
    	}

    	t=j/200;   
    	rewind(arch);

        while(feof(arch)==0 && out==0)
        {
            error=fread(texto[k],sizeof(uint64_t),25,arch); //tomar pedazos de 64 bits hasta tomar 1600 bits 25*64=1600
            
            if(error<25)
            {
                printf("\n%d=ERROR!!\n",error);
                out=1;
                fsetpos(arch,&punto);
                error1=fread(texto[k],sizeof(uint8_t),200,arch); //tomar pedazos de 64 bits hasta tomar 1600 bits 25*64=1600
                printf("\n%d=ERROR!!\n",error1);

                printf("{");
                for(int a=0;a<error1;a++)
                {
                    printf("%d,",resto[a]);
                }
                printf("}\n");


            }
            else
            {
                for(int a=0;a<25;a++)
                {
                    printf("%ld,",texto[k][a]);
                }

                fgetpos(arch,&punto);    
            } 
            k++;
            
        }


        printf("cantidad de bytes=%d, cantidad de bloques=%d\n\n",j,k);
        

        /*for (i=0;i<=t;i++)
        {
        	printf("{");
	        for(j=0;j<25;j++)
	        {
	            printf("%ld,",texto[i][j]);
	        }
	        printf("}\n\n");
	    }*/
    }
    else
    {
        printf("No se encuentra el archivo");
    }

    fclose(arch);
    return 0;
}