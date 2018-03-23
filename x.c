#include <stdio.h>

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#define l_int_size 64


void hex2bin(uint64_t data,int *val); //funcion para convertir datos de hexadecimal a binario
void datashow(uint64_t *data,int size);  //funcion para mostrar los datos contenidos en un archivo en hexadecimal
void stateArrayShow(uint64_t **A); //Muestra el contenido de una matriz tipo State Array
int archivo(char archivo[30],uint64_t *txt, int size); //funcion para leer un archivo y convertirlo a datos binarios
uint64_t doblar(uint64_t d); //gira los datos para que queden en el orden requerido mas adelante
int string(uint64_t *a,int size,int act,uint64_t* b); //funcion que inserta el string a(directo del archivo, con padding) en un arreglo b de 25 posiciones de 64 bits
void stateArray(uint64_t *s,uint64_t **a);  //funcion para crear un state Array... la famosa Matriz M(w(x+5y)+z)
int mod(int x,int n);                           //funcion para hallar el modulo de una division
int tamano(char archivo[]);                     //funcion que halla el tamano de un archivo en bytes
void muestrahash(uint64_t **S,int bits);                 //funcion que muestra el hash resultante


void Theta  (uint64_t **S,  uint64_t **S1);     //iteraciones y conversiones requeridas
void Rho    (uint64_t **S1, uint64_t **S2);
void Pi     (uint64_t **S2, uint64_t **S3);
void Chi    (uint64_t **S3, uint64_t **S4);
void Iota   (uint64_t **S4, uint64_t **SF, int r);
void Ronda  (uint64_t **S,  uint64_t **SF);

void Keccak (uint64_t *msg, int sz64, int bits);
void xor64  (uint64_t **S1,  uint64_t **S2);

const uint64_t RC[25]={ 0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000, 0x000000000000808B,
                        0x0000000080000001, 0x8000000080008081, 0x8000000000008009, 0x000000000000008A, 0x0000000000000088,
                        0x0000000080008009, 0x000000008000000A, 0x000000008000808B, 0x800000000000008B, 0x8000000000008089,
                        0x8000000000008003, 0x8000000000008002, 0x8000000000000080, 0x000000000000800A, 0x800000008000000A,
                        0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
                    };




//programa principal. Se deben crear dos punteros: uno para los datos en 64 bits(data) y otro para los datos binarios (bin)
//se debe realizar las asignaciones dinamicas desde aca y no en las funciones secundarias.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int num, char **dat)
{
    char arch[20];      //archivo del cual se lee el mensaje
    uint64_t *data;                 //puntero que recoge los datos que hay en el mensaje
    int size=0,p64;                     //tamano del String
    int verBits;


    if (num==2)
    {
        strcpy(arch,dat[1]);
        verBits=1088;
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
        strcpy(arch,"archivo.txt");
        verBits=1088;
        printf("Demasiados argumentos. Usando configuracion estandar. Archivo.txt 1088 bits\n\n");
    }


    size=tamano(arch);  //se halla el tamano del archivo

    if(size!=-1)
    {
        p64=8*size/64+1;        //se toman datos de 64 bits del archivo
        data=(uint64_t *) calloc(p64,sizeof(uint64_t));

        archivo(arch,data,size);    //se lee el archivo y se llena la variable data con todo el archivo
        if(p64==0)                  //si no hay datos debo considerar que al menos debo tener el padding
        {
            p64++;
        }
        Keccak(data,p64,verBits);
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
int string(uint64_t *a,int size,int act,uint64_t* b) //funcion que inserta el string a(directo del archivo, con padding) en un arreglo b de 25 posiciones de 64 bits
{
    int i;

    for (i=0;i<25;i++)
    {

        if((act)<size && i<17)
        {
            b[i]=a[act];
            //printf("\n1(%d,%d,%d)\n",i,size,act);
            act++;

            

        }
        if(i==16 && act==size)
        {
               // printf("\n2(%d,%d,%d)\n",i,size,act);
                b[i]=b[i]|0x8000000000000000;
        }
        

        //printf("(%d)b: %"PRIx64"\n",act+i,b[i]);
    }
    return act;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Keccak (uint64_t *msg, int sz64,int bits)
{
    int i,j,k,r=0;
    int cont=0;
    uint64_t **S;   //State array
    uint64_t **SF;  //state array final
    uint64_t *St;   //State array


    St = (uint64_t *) malloc (25*sizeof(uint64_t));
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

        cont=string(msg,sz64,cont,St);        //lleno el array St, que le agrega el padding y lo ordena en cadenas de 1600 bits

        //printf("\n%d,%d,%d Dato\n",i,r,cont);

        stateArray(St,S);               //se calcula el state array
        //printf("S %d/%d\n",i,r);
        //stateArrayShow(S);
        //printf("SF %d/%d\n",i,r);
        //stateArrayShow(SF);

        xor64(S,SF);    //se calcula la xor entre el dato anterior y el dato actual, y se retorna en S

        for(j=0;j<5;j++)
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
    }while(i<=r);
    printf("Hash: ");
    muestrahash(SF,bits);
    printf("\n");
        
    

    free(St);
    free(S);
    free(SF);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void xor64(uint64_t** S1, uint64_t** S2)
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
    uint64_t temp=0,show;
    int l=0;
    char string[9];

    int contbits=0;

    for(i=0;i<5;i++)
    {
        for(j=0;j<5;j++)
        {
            temp=s[j][i];
            if(contbits<bits-1)
            {
                for(k=0;k<8;k++)
                {
                    show=temp & 0xFF;
                    temp=temp>>8;
                    contbits+=8;
                    sprintf(string,"%lx",show);
                    printf("%s",string);
                }
                        
                printf(" ");
            }
            //printf("\ncontbits: %d\n",contbits);
        }

    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Ronda(uint64_t **S, uint64_t **S5)     //funcion para realizar las rondas para hallar el hash
{
    uint64_t **S1;
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



    for(r=0;r<24;r++)
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
    
    //free(S);
    //free(S1);
    //free(S2);
    //free(S3);
    //free(S4);
    //free(S5);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////

int tamano(char archivo[])
{
    int j;
    FILE *arch;
    arch=fopen(archivo,"rb");       //se lee el nombre del archivo y si es correcto:
    if(arch!=NULL)
    {
        fseek(arch,0,SEEK_END);     //se lee todo el archivo para verificar la cantidad de datos (bytes)contenidos
        j=ftell(arch);              //y se guarda en j
    }
    else
    {
        printf("\nNo se encuentra el archivo\n");
        j=-1;
    }
    fclose(arch);
    return j;
}






int archivo(char archivo[30],uint64_t *fil, int size) //funcion para leer un archivo y convertirlo a datos binarios
{
    FILE *arch;

    int i=0,j=0,k=0;        //indices utilizados para los ciclos
    int out=0;          //out indica si hay errores en la conversion
    int pbits;            //indica  la cantidad de pedazos de 8 bits que quedan sobrando
    int p64;               //indicador de pedazos de 64 bits
    uint8_t *txt;         //puntero de 8 bits, para recolectar los datos directamente del archivo, de 8 bits

    pbits=size+1;         //se calcula la cantidad de datos de 8 bits + padding
    p64=8*size/64+1;      //asi como la cantidad de datos de 64 bits + padding
    txt= (uint8_t*)malloc(pbits*sizeof(uint8_t));
    arch=fopen(archivo,"rb");       //se lee el nombre del archivo y si es correcto:

    if(arch!=NULL)
    {
        fread(txt,sizeof(uint8_t),pbits-1,arch);       //se lee la cantidad de datos que se puedan almacenar en variables de 64 bits
        txt[pbits-1]=0x1F;

        
        for(i=0;i<p64;i++)
        {
            k=8*(i+1)-1;
            for(j=0;j<8;j++)
            {
            
                fil[i]=fil[i]|txt[k];
                if (j<7)
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
        out=1; //si hay algun error con la lectura del archivo se retorna 1
    }
    //free(resto);
    fclose(arch);
    return out;

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
        val[l_int_size-j-1]=(int) b;                     //el vector resultante una posicion a la derecha, para repetir el proceso
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
void Theta(uint64_t **S,uint64_t **S1)
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
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Rho(uint64_t **S1, uint64_t **S2)
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
void Pi(uint64_t **S2,uint64_t **S3)
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

void Chi(uint64_t **S3,uint64_t **S4)
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

void Iota(uint64_t **S4,uint64_t **SF, int r)
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



    temp=RC[r];

    SF[0][0]=S4[0][0]^temp;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
int mod(int x,int n)
{
    return(x%n+n)%n;    //dolores de cabeza...resueltos.
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////end
