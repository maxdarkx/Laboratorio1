#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#define l_int_size 64
#define r 1600
#define dato_tot 1088

void datashow(uint64_t **data, int size, int posx);                    //funcion para mostrar los datos contenidos en un archivo en hexadecimal

int archivo(char archivo[30], uint64_t **txt, int *size, int *posx);    //funcion para leer un archivo y convertirlo a datos binarios

void hex2bin(uint64_t *data,int size,int **val);            //funcion para convertir datos de hexadecimal a binario



//programa principal. Se deben crear dos punteros: uno para los datos en 64 bits(data) y otro para los datos binarios (bin)
//se debe realizar las asignaciones dinamicas desde aca y no en las funciones secundarias.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    char arch[]="archivo.txt";
    uint64_t **data;
    int **bin;
    int size=0;
    int pos;

    bin=(int **) malloc(sizeof(int *));
    data=(uint64_t **) malloc(sizeof(uint64_t));

    archivo(arch,data,&size,&pos);

    
    if(data!=NULL)
    {    
        datashow(data,size,pos);

        bin=(int**) malloc(size*sizeof(int *));
        for(int i=0;i<size;i++)
        {
            bin[i]=(int*)malloc(l_int_size*sizeof(int));
        }


        //hex2bin(data,size,bin);
    }
    else
    {
        printf("NULL");
    }

    printf("\n");

    free(data);
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void datashow(uint64_t **data,int size,int pos) //funcion para mostrar los datos contenidos en un archivo en hexadecimal
{
    int i=0,j=0;

    for(j=0;j<size;j++)
    {
        for(i=0;i<pos;i++)
        {
            if(i==0)
            {
                printf("\n64:[");
            }
            printf("0x%"PRIx64,data[j][i]);
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
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int archivo(char archivo[30],uint64_t **txt, int *size, int *posx) //funcion para leer un archivo y convertirlo a datos binarios de 64 bits
{
    FILE *arch;

    int i=0,j=0;    //indices utilizados para los ciclos
    int p64=0;      //indicador de pedazos de 64 bits
    int pbits=0;            //t indica  la cantidad de pedazos de 8 bits que quedan sobrando
    int out=0;          //out indica si hay errores en la conversion
    int pos;            //posicion en donde se acabo de leer el archivo y toca leer por 8 bits

    uint8_t *resto;     //puntero de 8 bits, ideal para recolectar los datos sobrantes de 8 bits

    arch=fopen(archivo,"rb");   //se lee el nombre del archivo y si es correcto:

    if(arch!=NULL)
    {
        fseek(arch,0,SEEK_END); //se lee todo el archivo para verificar la cantidad de datos contenidos
        j=ftell(arch);          //y se guarda en j

        pbits=((j*8)%1088)/8;         //se calcula la cantidad de datos huerfanos de 8 bits
        p64=8*j/1088;               //asi como la cantidad de datos de 64 bits

        rewind(arch);           //se devuelve el puntero al inicio del archivo 
        //printf("%d,%d,%d\ta\n",j,k,t);
        

        txt=(uint64_t**) malloc(25*sizeof(uint64_t*));    //se asigna un espacio en memoria segun la cantidad de datos que tenga el archivo
        
        //bin[i]=(int*)malloc(l_int_size*sizeof(int));
        for(int i=0;i<p64;i++)
        {
            txt[i]=(uint64_t*) malloc(p64*sizeof(uint64_t));    
        }

        for(int i=0;i<p64;i++)
        {    
            pos=fread(txt[i],sizeof(uint64_t),17,arch); //se lee la cantidad de datos que se puedan almacenar en variables de 64 bits
        }
        fread(resto,sizeof(uint8_t),pbits,arch);    //se leen los datos huerfanos de 8 bits


        if (pbits>0)    //si t==0, no hay datos huerfanos. pero si los hay, hay que meterlos a la variable de 64 bits
        {
            p64++;
        }
        for(i=0;i<pbits;i++)
        {
            txt[p64-1][pos]=(txt[p64-1][pos]|resto[i]);   //se utiliza una or y sucesivas rotaciones de datos hacia la izquierda
            if(i<pbits-1)                       //para meter los datos huerfanos
            {
                txt[p64-1][pos]=txt[p64-1][pos]<<8;
            }
        }

        printf("mirar\n%d",r);
        datashow(txt,p64,pos);
        printf("\n");
        for(i=pbits;i<l_int_size;i++)    //se agregan los datos del padding
        {
            if(i<(pbits+8))
            {

                txt[p64-1][pos]=txt[p64-1][pos]<<1;
                if(i>=pbits && i<=pbits+4)
                {
                    txt[p64-1][pos]=txt[p64-1][pos] | 1;
                    printf("datos1: %"PRIx64"\n",txt[p64-1][pos]);
                }
                else
                {
                    printf("datos0: %"PRIx64"\n",txt[p64-1][pos]);
                }
                
                
            }
            else
            {
                //txt[p64-1]=txt[p64-1]<<1; //Meter ceros en los espacios restantes   
            }
            
        }




        
       *size=p64;         //se guarda la cantidad de datos de 1600 bits hallados (vectores)
       *posx=pos;         //posicion dentro del vector de datos de 64 bits
        out=0;
    }
    else
    {
        //printf("No se encuentra el archivo");
        out=1;  //si hay algun error con la lectura del archivo se retorna 1
    }
    fclose(arch);
    return out;
    
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hex2bin(uint64_t *data,int size,int **val) //funcion para convertir datos de hexadecimal a binario
{
    uint64_t a=1;   //a, b y temp son variables temporales para la conversion
    uint64_t b=0;
    uint64_t temp;  

    int i=0,j=0;    //indices normalmente usados para ciclos
    
    int cont=0;     //contador para el manejo de la impresion de los datos
    
    //printf("ok\n" );
       
    for(i=0;i<size;i++)
    {
        temp=data[i];
        printf("0x%"PRIx64"\n",temp);
    
        
        for(j=0;j<l_int_size;j++)
        {
            b=temp & a;                      //para convertir un dato de hexadecimal a binario se realiza una and entre
            temp=temp>>1;                    // el dato hexa y un 1, quedando al descubierto el dato binario y rotando 
            val[i][l_int_size-j-1]=(int) b;  //el vector resultante una posicion a la derecha, para repetir el proceso
        }                                    //hasta finalizar la entrada de datos 
    }


    printf("matriz de datos:\n");            //se muestra la matriz resultante
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////