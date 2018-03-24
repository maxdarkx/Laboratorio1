#include <stdio.h>

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#define l_int_size 64



void datashow(uint64_t *data,int size);                 //funcion para mostrar los datos contenidos en un archivo en hexadecimal
void stateArrayShow(uint64_t **A);                      //Muestra el contenido de una matriz tipo State Array
int tamano(char archivo[]);                             //funcion que halla el tamano de un archivo en bytes
int archivo(char archivo[30],uint64_t *txt, int size);  //funcion para leer un archivo y convertirlo a datos binarios
uint64_t doblar(uint64_t d);                            //gira los datos para que queden en un orden especifico
int string(uint64_t *a,int size,int act,uint64_t* b);   //funcion que inserta el string a(directo del archivo, con padding) en un arreglo b de 25 posiciones de 64 bits
void stateArray(uint64_t *s,uint64_t **a);              //funcion para crear un state Array... la famosa Matriz M(w(x+5y)+z)
int mod(int x,int n);                                   //funcion para hallar el modulo de una division

void muestrahash(uint64_t **S,int bits);                //funcion que muestra el hash resultante

void Keccak (uint64_t *msg, int sz64, int bits);        //funcion que utiliza los step mappings theta, rho, pi, chi, e iota
                                                        //mediante una serie de rondas para crear el hash

void Theta  (uint64_t **S,  uint64_t **S1);             //iteraciones y conversiones requeridas
void Rho    (uint64_t **S1, uint64_t **S2);
void Pi     (uint64_t **S2, uint64_t **S3);
void Chi    (uint64_t **S3, uint64_t **S4);
void Iota   (uint64_t **S4, uint64_t **SF, int r);
void Ronda  (uint64_t **S,  uint64_t **SF);


void xor64  (uint64_t **S1,  uint64_t **S2);            //calcula el resultado de la operacion xor entre dos state array
void hex2bin(uint64_t data,int *val);                   //funcion para convertir datos de hexadecimal a binario


//constante RC, requerida para iota
const uint64_t RC[25]={ 0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000, 0x000000000000808B,
                        0x0000000080000001, 0x8000000080008081, 0x8000000000008009, 0x000000000000008A, 0x0000000000000088,
                        0x0000000080008009, 0x000000008000000A, 0x000000008000808B, 0x800000000000008B, 0x8000000000008089,
                        0x8000000000008003, 0x8000000000008002, 0x8000000000000080, 0x000000000000800A, 0x800000008000000A,
                        0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
                    };




//programa principal. Se llama a las funciones tamano, archivo y keccak para que realicen las funciones principales del programa
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int num, char **dat)
{
    char arch[20];                  //archivo del cual se lee el mensaje
    uint64_t *data;                 //puntero que recoge los datos que hay en el mensaje
    int size=0,p64;                 //tamano del String
    int verBits;                    //cantidad de bits del hash


    if (num==2)                     //primero se revisan las opciones que el usuario ingreso al invocar el programa
    {
        strcpy(arch,dat[1]);        //y se guardan las selecciones en arch(nombre del archivo a leer) y verbits(cantidad de bits)
        verBits=1088;               // del hash
    }
    else if (num==3)
    {
        strcpy(arch,dat[1]);
        verBits=atoi(dat[2]);
    }
    else if(num==0)
    {
        strcpy(arch,"archivo.txt");
        verBits=1088;
    }
    else
    {
        strcpy(arch,"archivo.txt");     //si hay mas de los argumentos deseados se cancela la ejecucion
        verBits=1088;
        printf("Demasiados argumentos. Usando configuracion estandar. Archivo.txt 1088 bits\n\n");
    }


    size=tamano(arch);          //se halla el tamano del archivo

    if(size!=-1)
    {
        p64=8*size/64+1;                                //se halla cuantos pedazos de 64 bits se usaran
        data=(uint64_t *) calloc(p64,sizeof(uint64_t)); //se asigna espacio en memoria y se iguala a cero para guardar el archivo en memoria

        archivo(arch,data,size);                        //se lee el archivo y se llena la variable data con todo el archivo

        Keccak(data,p64,verBits);                       //se ejecuta el subprograma keccak, que calcula el hash
    }
    else
    {
        printf("\nNombre de archivo no valido\n");
    }

    free(data);
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////

int tamano(char archivo[])              //funcion para calcular el tamano en bytes de un archivo para su procesado
{
    int j;
    FILE *arch;
    arch=fopen(archivo,"rb");           //se lee el nombre del archivo y si es correcto:
    if(arch!=NULL)
    {
        fseek(arch,0,SEEK_END);         //se lee todo el archivo para verificar la cantidad de datos (bytes)contenidos
        j=ftell(arch);                  //y se guarda en j
    }
    else
    {
        printf("\nNo se encuentra el archivo\n");
        j=-1;                           //si hay algun error, se retorna -1
    }
    fclose(arch);
    return j;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
int archivo(char archivo[30],uint64_t *fil, int size)   //funcion para leer un archivo y convertirlo a datos binarios
{
    FILE *arch;

    int i=0,j=0,k=0;                                    //indices utilizados para los ciclos
    int out=0;                                          //out indica si hay errores en la conversion
    int pbits;                                          //indica  la cantidad de pedazos de 8 bits a tomar
    int p64;                                            //indicador de pedazos de 64 bits
    uint8_t *txt;                                       //puntero de 8 bits, para recolectar los datos directamente del archivo

    pbits=size+1;                                       //se calcula la cantidad de datos de 8 bits + padding
    p64=8*size/64+1;                                    //asi como la cantidad de datos de 64 bits + padding
    txt= (uint8_t*)malloc(pbits*sizeof(uint8_t));
    arch=fopen(archivo,"rb");                           //se lee el nombre del archivo y si es correcto:

    if(arch!=NULL)
    {
        fread(txt,sizeof(uint8_t),pbits-1,arch);       //se lee la cantidad de datos que se puedan almacenar
        txt[pbits-1]=0x1F;

        
        for(i=0;i<p64;i++)
        {
            k=8*(i+1)-1;
            for(j=0;j<8;j++)
            {
            
                fil[i]=fil[i]|txt[k];                   //se escriben datos de 64 bits, tomando pedazos de a ocho bits por vez
                if (j<7)                                //se debe hacer en orden inverso para conservar la orientacion de los bits
                {
                    fil[i]=fil[i]<<8;
                }
                k--;
            

            }
        }


        //printf("Datos de 64:%d, Datos sobrantes: %d, Bytes de archivo: %d\t\n",p64,pbits,size);
        //datashow(fil,p64);
        out=0;
    }
    else
    {
        printf("No se encuentra el archivo");
        out=1;                                          //si hay algun error con la lectura del archivo se retorna 1
    }
    free(txt);
    fclose(arch);
    return out;

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
int string(uint64_t *a,int size,int act,uint64_t* b) //funcion que inserta el string a en un arreglo b de 25 posiciones de 64 bits
{
    int i;

    for (i=0;i<25;i++)
    {

        if((act)<size && i<17)
        {
            b[i]=a[act];                                //se guardan los datos del texto original en una variable 
            //printf("\n1(%d,%d,%d)\n",i,size,act);     //mas facilmente legible  
            act++;

            

        }
        if(i==16 && act==size)                          //si es el ultimo dato leido, se inserta un 1 al inicio del dato
        {
               // printf("\n2(%d,%d,%d)\n",i,size,act);
                b[i]=b[i]|0x8000000000000000;
        }
        

        //printf("(%d)b: %"PRIx64"\n",act+i,b[i]);
    }
    return act;                                         //se entrega la posicion del ultimo dato leido            
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

void muestrahash(uint64_t **s, int bits) //funcion para mostrar el hash y en el orden correcto
{
    int i,j,k;
    uint64_t temp=0;
    int show;
    int l=0;
    char string1[5],string2[5];

    int contbits=0;

    for(i=0;i<5;i++)                    //se recorre todo el string array, moviendose de a cuatro posiciones por vez, para
    {                                   //mostrar correctamente en pantalla cada uno de los terminos del hash                
        for(j=0;j<5;j++)
        {
            temp=s[j][i];
            if(contbits<bits-1)
            {
                for(k=0;k<8;k++)
                {
                    show=(int)temp & 0xF;
                    temp=temp>>4;
                    sprintf(string1,"%x",show);

                    show=(int)temp & 0xF;
                    temp=temp>>4;
                    sprintf(string2,"%x",show);

                    contbits+=8;
                    printf("%s%s",string2,string1);
                }
                        
                printf(" ");
            }
            //printf("\ncontbits: %d\n",contbits);
        }

    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void stateArrayShow(uint64_t **A) //Muestra el contenido de una matriz tipo State Array
{

    int i,j;

    printf("{\n");
    for(i=0;i<5;i++)
    {
        printf("[\n ");
        for(j=0;j<5;j++)
        {
            printf("%"PRIx64" ",A[i][j]);
        }
        printf("];\n");
    }
    printf("{\n\n\n");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Keccak (uint64_t *msg, int sz64,int bits)           //funcion que utiliza los step mappings theta, rho, pi, chi, e iota
{                                                        //mediante una serie de rondas para crear el hash    
   int i,j,k,r=0;                                        //indices para la operacion de los for
    int cont=0;                                          //guarda cual es la posicion actual en la que se ha leido
    uint64_t **S;                                        //State array
    uint64_t **SF;                                       //state array final
    uint64_t *St;                                        //State array


    St = (uint64_t *) malloc (25*sizeof(uint64_t));      //se asigna espacio en memoria y se inicializan los array
    S = (uint64_t **) calloc (5,sizeof(uint64_t*));
    for (i = 0; i< 5; i++)
    {
        S[i] = (uint64_t *) calloc (5,sizeof(uint64_t));
    }

    SF = (uint64_t **) calloc (5,sizeof(uint64_t*));
    for (i = 0; i< 5; i++)
    {
        SF[i] = (uint64_t *) calloc (5,sizeof(uint64_t));
    }

    r=sz64/17;
    i=0;


    do
    {

        for(j=0;j<25;j++)
        {
            St[j]=0;
        }

        cont=string(msg,sz64,cont,St);          //lleno el array St, que le agrega el padding y lo ordena en cadenas de 1600 bits

        //printf("\n%d,%d,%d Dato\n",i,r,cont);

        stateArray(St,S);                       //se calcula el state array
        //printf("S %d/%d\n",i,r);
        //stateArrayShow(S);
        //printf("SF %d/%d\n",i,r);
        //stateArrayShow(SF);

        xor64(S,SF);                            //se calcula la xor entre el dato anterior y el dato actual, y se retorna en S

        for(j=0;j<5;j++)                        //se inicializa nuevamente SF para volver a operar
        {
            for(k=0;k<5;k++)
            {
                SF[j][k]=0;
            }
        }
        Ronda(S,SF);                    //se realizan las iteraciones necesarias para el resultado
        //printf("SF %d/%d\n",i,r);
        //stateArrayShow(SF);
        i++;
    }while(i<=r);                       //se realizan tantas rondas como datos haya pendientes por procesar en el archivo
    printf("Hash: ");
    muestrahash(SF,bits);               //se muestra el hash resultante
    printf("\n");
        
    

    free(St);                           //se libera la memoria utilizada
    free(S);
    free(SF);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Ronda(uint64_t **S, uint64_t **S5)     //funcion para realizar las rondas para hallar el hash
{
    uint64_t **S1;                          //se inicializan todos los array necesarios para el calculo del hash
    uint64_t **S2;
    uint64_t **S3;
    uint64_t **S4;
    int r=0,j=0,i=0;

    S1 = (uint64_t **) malloc (5*sizeof(uint64_t*));
    for (r = 0; r< 5; r++)
    {
        S1[r] = (uint64_t *) malloc(5*sizeof(uint64_t));
    }

    S2 = (uint64_t **) malloc (5*sizeof(uint64_t*));
    for (r = 0; r< 5; r++)
    {
        S2[r] = (uint64_t *) malloc(5*sizeof(uint64_t));
    }

    S3 = (uint64_t **) malloc (5*sizeof(uint64_t*));
    for (r = 0; r< 5; r++)
    {
        S3[r] = (uint64_t *) malloc(5*sizeof(uint64_t));
    }

    S4 = (uint64_t **) malloc (5*sizeof(uint64_t*));
    for (r = 0; r< 5; r++)
    {
        S4[r] = (uint64_t *) malloc(5*sizeof(uint64_t));
    }



    for(r=0;r<24;r++)                   //se realizan 24 rondas de los step mappings, segun la documentacion
    {
        if(r==0)
        {
            for(i=0;i<5;i++)
            {
                for(j=0;j<5;j++)
                {
                    S5[i][j]=S[i][j];
                }
            }
        }

        Theta( S5,S1);
        Rho(S1,S2);
        Pi(S2,S3);
        Chi(S3,S4);
        Iota(S4,S5,r);
        /*
        if(r==0 || r==23 )
        {
            printf("Ronda %d",r);
            printf("\nSALIDA THETA\n");
            stateArrayShow(S1);

            printf("\nSALIDA RHO\n");
            stateArrayShow(S2);

            printf("\nSALIDA PI\n");
            stateArrayShow(S3);

            printf("\nSALIDA CHI\n");
            stateArrayShow(S4);

            printf("\nSALIDA IOTA\n");
            stateArrayShow(S5);
        }*/

    }
    free(S1);
    free(S2);
    free(S3);
    free(S4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Theta(uint64_t **S,uint64_t **S1) //step mapping theta
{
    uint64_t *C;
    uint64_t *D;
    int x,y;



    C = (uint64_t *) malloc(5*sizeof(uint64_t ));

    D = (uint64_t *) malloc(5*sizeof(uint64_t ));




    for(x = 0;x<5;x++)
    {
       C[x] = S[x][0]^S[x][1]^S[x][2]^S[x][3]^S[x][4];
    }

    for(x = 0;x<5;x++)
    {
        D[x] = C[mod(x-1,5)]^(C[mod(x+1,5)]>>63|C[mod(x+1,5)]<<1);
    }
    for(x = 0;x<5;x++)
    {
        for (y = 0; y < 5; y++)
        {
            S1 [x][y] = S[x][y] ^ D[x];
        }
    }
    free(C);
    free(D);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Rho(uint64_t **S1, uint64_t **S2)  //step mapping rho
{

    int x=1;
    int y=0;
    int t=0;
    int x1,y1;
    int mov=0;

    S2 [0][0] = S1[0][0];


    for (t = 0; t <24; t++)
    {
        if(t>0)
        {
            x=x1;
            y=y1;
        }
        mov=(t+1)*(t+2)/2;

        S2 [x][y] = S1[x][y]<<mov | S1[x][y]>>(64-mov);


        y1 = mod((2*x+3*y),5);
        x1 = y;

    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Pi(uint64_t **S2,uint64_t **S3)    //step mapping pi
{
    int x;
    int y;

    for(x = 0;x<5;x++)
    {
       for (y = 0; y < 5; y++)
       {

            S3[x][y] = S2[mod(x+3*y,5)][x] ;
       }
   }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Chi(uint64_t **S3,uint64_t **S4)   //step mapping chi
{
    int x,y;

    for(x = 0;x<5;x++)
    {
        for (y = 0; y < 5; y++)
        {
            S4 [x][y] = S3 [x][y] ^ (( S3 [mod(x+1,5)][y] ^ 0XFFFFFFFFFFFFFFFF) & (S3[mod(x+2,5)][y]));
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Iota(uint64_t **S4,uint64_t **SF, int r)   //step mapping iota
{
    int i,j;
    uint64_t temp;

    for (i=0;i<5;i++)
    {
        for (j=0;j<5;j++)
        {
            SF[i][j]=S4[i][j];
        }
    }



    temp=RC[r];                 //se usa la constante RC definida al principio

    SF[0][0]=S4[0][0]^temp;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
int mod(int x,int n)
{
    return(x%n+n)%n;    //calcula el modulo de la division
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////end

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void xor64(uint64_t** S1, uint64_t** S2)            //se calcula la operacion xor entre dos String Array
{
    int i,j;
    for(i=0;i<5;i++)
    {
        for(j=0;j<5;j++)
        {
            S1[i][j]=S1[i][j]^S2[i][j]; 
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void hex2bin(uint64_t data,int *val)                //funcion para convertir datos de hexadecimal a binario
{
    uint64_t a=1;                                   //a, b y temp son variables temporales para la conversion
    uint64_t b=0;
    uint64_t temp;

    int j=0;                                        //indices normalmente usados para ciclos

    //printf("0x%"PRIx64"\n",data);

    temp=data;
    for(j=0;j<l_int_size;j++)
    {
        b=temp & a;                      //para convertir un dato de hexadecimal a binario se realiza una and entre
        temp=temp>>1;                    // el dato hexa y un 1, quedando al descubierto el dato binario y rotando
        val[l_int_size-j-1]=(int) b;     //el vector resultante una posicion a la derecha, para repetir el proceso
    }                                    //hasta finalizar la entrada de datos
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint64_t doblar(uint64_t d)             //gira los datos para que queden en un orden especifico
{
    uint64_t temp1=0,temp2=0,temp3=0;
    int i=0;

    temp1=d;

    for(i=0;i<8;i++)
    {
        temp2=temp1 & 0xFF;     //hago una and entre el dato a atrapar y 11111111
        temp3=temp2 | temp3;    //guardo en el dato anterior el dato actual con el anterior

        if(i<7)
        {
            temp3=temp3<<8;     //si no he terminado, me muevo ocho posiciones para recibir un nuevo dato
            temp1=temp1>>8;     //si no he terminado, me muevo ocho posiciones para mostrar el nuevo dato a escribir
        }
        //printf("\nt1=%"PRIx64" t2=%"PRIx64" t3=%"PRIx64"\n",temp1,temp2,temp3);

    }

    return temp3;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
