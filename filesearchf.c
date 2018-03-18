#include <stdio.h>

#include <inttypes.h>
#include <stdlib.h>
#define l_int_size 64

void hex2bin(uint64_t *data,int size,int **val); //funcion para convertir datos de hexadecimal a binario
void datashow(uint64_t *data,int size);  //funcion para mostrar los datos contenidos en un archivo en hexadecimal
void stateArrayShow(uint64_t **A); //Muestra el contenido de una matriz tipo State Array
uint64_t* archivo(char archivo[30], int *size); //funcion para leer un archivo y convertirlo a datos binarios
uint64_t doblar(uint64_t d); //gira los datos para que queden en el orden requerido mas adelante
int string(uint64_t *a,int size,int act,uint64_t* b); //funcion que inserta el string a(directo del archivo, con padding) en un arreglo b de 25 posiciones de 64 bits
void stateArray(uint64_t *s,uint64_t **a);  //funcion para crear un state Array... la famosa Matriz M(w(x+5y)+z)

//programa principal. Se deben crear dos punteros: uno para los datos en 64 bits(data) y otro para los datos binarios (bin)
//se debe realizar las asignaciones dinamicas desde aca y no en las funciones secundarias.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    char arch[]="archivo.txt";
    uint64_t *data;
    uint64_t *S;
    int i;
    //int **bin;
    int size=0;
    int act=0;
    uint64_t **A;

    //bin=(int **) malloc(sizeof(int *));

    
    S=(uint64_t *)calloc(25,sizeof(uint64_t));
    A=(uint64_t **)calloc(5,sizeof(uint64_t *));
    for(i=0;i<5;i++)
    {
    	A[i]=(uint64_t *) calloc(5,sizeof(uint64_t));
    }








    data=archivo(arch,&size);
    printf("\nVECTOR A:\n");
    datashow(data,size);
    act=string(data,size,act,S);
    
    datashow(S,25);
    printf("StateArray\n");
    stateArray(S,A);

    stateArrayShow(A);







    //free(data);
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////
void datashow(uint64_t *data,int size)  //funcion para mostrar los datos contenidos en un archivo en hexadecimal
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
            printf("]\n");
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint64_t* archivo(char archivo[30], int *size) //funcion para leer un archivo y convertirlo a datos binarios
{
    FILE *arch;

    int i=0,j=0;        //indices utilizados para los ciclos
    int out=0;          //out indica si hay errores en la conversion
    int pbits;            //indica  la cantidad de pedazos de 8 bits que quedan sobrando
    int p64;               //indicador de pedazos de 64 bits
    uint8_t *resto;         //puntero de 8 bits, ideal para recolectar los datos sobrantes de 8 bits
    int cbits;              //cantidad de bits del string tomado del archivo + 8 bits por el padding
    uint64_t* txt;			//puntero que recoge los datos del archivo para ser leidos de a 64 bits por vez

    arch=fopen(archivo,"rb");       //se lee el nombre del archivo y si es correcto:

    if(arch!=NULL)
    {
        fseek(arch,0,SEEK_END);     //se lee todo el archivo para verificar la cantidad de datos contenidos
        j=ftell(arch);              //y se guarda en j
        cbits=j+1;
        pbits=((cbits*8)%64)/8;         //se calcula la cantidad de datos huerfanos de 8 bits
        p64=8*cbits/64;                 //asi como la cantidad de datos de 64 bits

        rewind(arch);               //se devuelve el puntero al inicio del archivo

        txt=(uint64_t *)calloc(p64,sizeof(uint64_t));      //se asigna un espacio en memoria segun la cantidad de datos que tenga el archivo
        resto=(uint8_t *) malloc((pbits+1)*sizeof(uint8_t));			//p64 posiciones por un posible error de padding

        fread(txt,sizeof(uint64_t),p64,arch);       //se lee la cantidad de datos que se puedan almacenar en variables de 64 bits
        
        

        if (pbits>0)    //si pbits==0, no hay datos huerfanos. pero si los hay, hay que meterlos a la variable de 64 bits
        {
            p64++;
            fread(resto,sizeof(uint8_t),pbits,arch);    // y de 8 bits huerfanas
            resto[pbits-1]=0xF8;
            for(i=0;i<pbits;i++)
            {
                
                if(i<pbits)                           //para meter los datos huerfanos
                {
                    txt[p64-1]=txt[p64-1]<<8;
                }
                txt[p64-1]=(txt[p64-1]|resto[i]);       //se utiliza una or y sucesivas rotaciones de datos hacia la izquierda
            }
            for(i=0;i<p64-1;i++)            //girar los datos de 64 bits para que aparezcan en el orden correcto
            {                               //OJO: si el dato es huerfano, ya esta ordenado. por eso solo se ejecuta en p64-1 datos
                txt[i]=doblar(txt[i]);
            }
        }
        else
        {
            resto[0]=0xF8;
            for(i=0;i<p64;i++)            
            {                               
                txt[i]=doblar(txt[i]);
            }
            //txt[p64-1]=txt[p64-1]<<8;
            txt[p64-1]=(txt[p64-1]|resto[0]);
        }
        

        


        printf("Datos de 64:%d, Datos sobrantes: %d, Bytes de archivo: %d\t\n",p64,pbits,cbits);
        datashow(txt,p64);

       *size=p64;
        out=0;
    }
    else
    {
        //printf("No se encuentra el archivo");
        out=1; //si hay algun error con la lectura del archivo se retorna 1
    }
    //free(resto);
    fclose(arch);
    return txt;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hex2bin(uint64_t *data,int size,int **val) //funcion para convertir datos de hexadecimal a binario
{
    uint64_t a=1;   //a, b y temp son variables temporales para la conversion
    uint64_t b=0;
    uint64_t temp;

    int i=0,j=0;    //indices normalmente usados para ciclos

    //int cont=0;     //contador para el manejo de la impresion de los datos

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
////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint64_t doblar(uint64_t d) //gira los datos para que queden en el orden requerido mas adelante
{
	uint64_t temp1=0,temp2=0,temp3=0;
	int i=0;

	temp1=d;

	for(i=0;i<8;i++)
	{
		temp2=temp1 & 0xFF;		//hago una and entre el dato a atrapar y 11111111
		temp3=temp2 | temp3;	//guardo en el dato anterior el dato actual con el anterior

		if(i<7)
		{	
			temp3=temp3<<8;		//si no he terminado, me muevo ocho posiciones para recibir un nuevo dato
			temp1=temp1>>8;		//si no he terminado, me muevo ocho posiciones para mostrar el nuevo dato a escribir
		}
		//printf("\nt1=%"PRIx64" t2=%"PRIx64" t3=%"PRIx64"\n",temp1,temp2,temp3);

	}

	return temp3;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void stateArray(uint64_t *s,uint64_t **a)  //funcion para crear un state Array... la famosa Matriz M(w(x+5y)+z)
{
    int i=0,j=0,k=0;

    for(i=0;i<5;i++)
    {
        for(j=0;j<5;j++)
        {
            a[j][i]=s[k];
            k++;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////
int string(uint64_t *a,int size,int act,uint64_t* b) //funcion que inserta el string a(directo del archivo, con padding) en un arreglo b de 25 posiciones de 64 bits
{
    int i,j;

    for (i=0;i<25;i++)
    {
        if(i<size)
        {
            b[i]=a[act+i];
        }
        else if(i=24)
        {
            b[i]=0x1;
        }
        printf("(%d)b: %"PRIx64"\n",act+i,b[i]);
    }
    return act+i;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void stateArrayShow(uint64_t **A) //Muestra el contenido de una matriz tipo State Array
{

	int i,j;
	printf("{");
	for(i=0;i<5;i++)
	{	
		printf("[");
		for(j=0;j<5;j++)
		{
			printf("%"PRIx64" ",A[i][j]);
		}
		printf("];\n");
	}
	printf("{");
}