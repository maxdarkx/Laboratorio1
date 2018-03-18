#include <stdio.h>

#include <inttypes.h>
#include <stdlib.h>
#define l_int_size 64

void hex2bin(uint64_t data,int *val); //funcion para convertir datos de hexadecimal a binario
void datashow(uint64_t *data,int size);  //funcion para mostrar los datos contenidos en un archivo en hexadecimal
void stateArrayShow(int ***A); //Muestra el contenido de una matriz tipo State Array
uint64_t* archivo(char archivo[30], int *size); //funcion para leer un archivo y convertirlo a datos binarios
uint64_t doblar(uint64_t d); //gira los datos para que queden en el orden requerido mas adelante
int string(uint64_t *a,int size,int act,uint64_t* b); //funcion que inserta el string a(directo del archivo, con padding) en un arreglo b de 25 posiciones de 64 bits
void stateArray(uint64_t *s,int ***a);  //funcion para crear un state Array... la famosa Matriz M(w(x+5y)+z)
void theta(int **C,int **D,int ***S,int ***S1);
int mod(int x,int n);

void Theta 	(int ***S,  int ***S1);
void Rho 	(int ***S1, int ***S2);
void Pi 	(int ***S2, int ***S3);
void Chi 	(int ***S3, int ***S4);



//programa principal. Se deben crear dos punteros: uno para los datos en 64 bits(data) y otro para los datos binarios (bin)
//se debe realizar las asignaciones dinamicas desde aca y no en las funciones secundarias.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    char arch[]="archivo.txt";		//archivo del cual se lee el mensaje
    uint64_t *data;					//puntero que recoge los datos que hay en el mensaje
    uint64_t *St;					//String que recibe los datos del mensaje y lo convierte en cadenas de 1600 bits con padding
    int j;
    //int **bin;
    int size=0;						//tamano del String
    int act=0;						//posicion en la que se esta trabajando en el String

//variables necesarias para las transformaciones

    int k,r,t;
    
    int ***S;
    int ***S1;
    int ***S2;
    int ***S3;
    int ***S4;
    int **D;
    int **C;

    S = (int ***) malloc (5*sizeof(int ***));
    for (r = 0; r< 5; r++)
    {
        S[r] = (int **) malloc(5*sizeof(int*));
        for (k = 0; k < 5; k++)
        {
            S[r][k] = (int*)malloc(64*sizeof(int));
        }
    }

    S1 = (int ***) malloc (5*sizeof(int ***));
    for (r = 0; r< 5; r++)
    {
        S1[r] = (int **) malloc(5*sizeof(int*));
        for (k = 0; k < 5; k++)
        {
            S1[r][k] = (int*)malloc(64*sizeof(int));
        }
    }

    S2 = (int ***) malloc (5*sizeof(int ***));
    for (r = 0; r< 5; r++)
    {
        S2[r] = (int **) malloc(5*sizeof(int*));
        for (k = 0; k < 5; k++)
        {
            S2[r][k] = (int*)malloc(64*sizeof(int));
        }
    }

    S3 = (int ***) malloc (5*sizeof(int ***));
    for (r = 0; r< 5; r++)
    {
        S3[r] = (int **) malloc(5*sizeof(int*));
        for (k = 0; k < 5; k++)
        {
            S3[r][k] = (int*)malloc(64*sizeof(int));
        }
    }

    S4 = (int ***) malloc (5*sizeof(int ***));
    for (r = 0; r< 5; r++)
    {
        S4[r] = (int **) malloc(5*sizeof(int*));
        for (k = 0; k < 5; k++)
        {
            S4[r][k] = (int*)malloc(64*sizeof(int));
        }
    }

    //bin=(int **) malloc(sizeof(int *));


    St=(uint64_t *)calloc(25,sizeof(uint64_t));


    data=archivo(arch,&size);

    //printf("\nVECTOR A:\n");
    //datashow(data,size);

    act=string(data,size,act,St);

    datashow(St,25);

    printf("\nStateArray\n");
    stateArray(St,S);

    stateArrayShow(S);

    printf("\nTheta\n");
    Theta( S,S1);
    stateArrayShow(S1);

    Rho(S1,S2);
    printf("\nSALIDA RHO\n");
    stateArrayShow(S2);

    Pi(S2,S3);
    printf("\nSALIDA PI\n");
    stateArrayShow(S3);

    Chi(S3,S4);
    printf("\nSALIDA CHI\n");
    stateArrayShow(S4);
















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
        //datashow(txt,p64);

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
void hex2bin(uint64_t data,int *val) //funcion para convertir datos de hexadecimal a binario
{
    uint64_t a=1;   //a, b y temp son variables temporales para la conversion
    uint64_t b=0;
    uint64_t temp;

    int j=0;    //indices normalmente usados para ciclos

    //printf("0x%"PRIx64"\n",data);

    temp=data;
    for(j=0;j<l_int_size;j++)
    {
        b=temp & a;                      //para convertir un dato de hexadecimal a binario se realiza una and entre
        temp=temp>>1;                    // el dato hexa y un 1, quedando al descubierto el dato binario y rotando
        val[l_int_size-j-1]=(int) b;					 //el vector resultante una posicion a la derecha, para repetir el proceso
    }                                    //hasta finalizar la entrada de datos
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

void stateArray(uint64_t *s,int ***A)  //funcion para crear un state Array... la famosa Matriz M(w(x+5y)+z)
{

    int i=0,j=0,k=0,l=0;
    uint64_t **a;
    int *bin;


    a = (uint64_t **) malloc(5*sizeof(uint64_t *));
    for (i = 0; i < 5; i++)
    {
        a[i] = (uint64_t *)malloc(64*sizeof(uint64_t));
    }

    bin = (int *) malloc(64*sizeof(int));



    for(i=0;i<5;i++)
    {
        for(j=0;j<5;j++)
        {
            a[j][i]=s[k];
            k++;
        }
    }


    for(i=0;i<5;i++)
    {
        for(j=0;j<5;j++)
        {
            hex2bin(a[i][j],bin);
            for(l=0;l<64;l++)
            A[i][j][l]=bin[l];
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
        //printf("(%d)b: %"PRIx64"\n",act+i,b[i]);
    }
    return act+i;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void stateArrayShow(int ***A) //Muestra el contenido de una matriz tipo State Array
{

    int i,j,k,l;
    uint64_t z=0;

    printf("{\n");
    l=0;
    for(i=0;i<5;i++)
    {
        printf("[\n ");
        for(j=0;j<5;j++)
        {
            z=0;
            for(k=0;k<64;k++)
            {
                z=A[i][j][k] | z;
                if(k<63)
                {
                	z=z<<1;
                }
                //printf("%d:(%d,%"PRIx64")\n",k,A[i][j][k],z);
            }
            printf("%"PRIx64" ",z);
        }
        printf("];\n");
    }
    printf("{\n\n\n");
}


void Theta(int ***S,int ***S1)
{
	int **C;
	int **D;
	int k1;
    int x,y,z;
    int n,m,o;


    C = (int **) malloc(5*sizeof(int *));
    for (k1 = 0; k1 < 5; k1++)
    {
        C[k1] = (int *)malloc(64*sizeof(int));
    }

    D = (int **) malloc(5*sizeof(int *));
    for (k1 = 0; k1 < 5; k1++)
    {
        D[k1] = (int *)malloc(64*sizeof(int));
    }



    for(x = 0;x<5;x++)
    {
        for (z = 0; z < 64; z++)
        {
            C[x][z] = S[x][0][z]^S[x][1][z]^S[x][2][z]^S[x][3][z]^S[x][4][z];
        }
    }

    for(x = 0;x<5;x++)
    {
        for (z = 0; z < 64; z++)
        {
            //m=mod(x-1,5);
            //n=mod(x+1,5);
            //o=mod(z-1,64);

            D[x][z] = C[mod(x-1,5)][z]^C[mod(x+1,5)][mod(z-1,64)];

        }
    }
    for(x = 0;x<5;x++)
    {
        for (y = 0; y < 5; y++)
        {
            for (z = 0; z < 64; z++)
            {
                S1 [x][y][z] = S[x][y][z] ^ D[x][z];
            }
        }
    }
}


void Rho(int ***S1, int ***S2)
{

	int x=1;
	int y=0;
	int z=0;
	int t=0;
	int x1,y1;


    for (z = 0; z < 64; z++) 
    {
    	S2 [0][0][z] = S1[0][0][z];
    }

    for (t = 0; t <24; t++) 
    {
    	if(t>0)
    	{
    		x=x1;
    		y=y1;
    	}
	    for (z = 0; z < 64; z++) 
	    {
        	S2 [x][y][z] = S1[x][y][mod(z-(t+1)*(t+2)/2,64)];
       	}
       	y1 = mod((2*x+3*y),5);
        x1 = y;
        
    }
    
}
void Pi(int ***S2,int ***S3)
{
    int x;
	int y;
	int z;

	for(x = 0;x<5;x++)
	{
       for (y = 0; y < 5; y++) 
       {
           	for (z = 0; z < 64; z++) 
           	{
           		S3[x][y][z] = S2[mod(x+3*y,5)][x][z] ;

           	}
       }
   }
}


void Chi(int ***S3,int ***S4)
{
	int x,y,z;

	for(x = 0;x<5;x++)
	{
        for (y = 0; y < 5; y++) 
        {
           for (z = 0; z < 64; z++) 
           {
	           S4 [x][y][z] = S3 [x][y][z] ^( S3 [mod(x+1,5)][y][z] & S3 [mod(x+2,5)][y][z] ) ;
           }
        }
    }
}

//void Iota(){}


int mod(int x,int n)
{
    return(x%n+n)%n;
}