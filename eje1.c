#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define LEER 0
#define ESCRIBIR 1
void llenarMatriz(int **matriz, int tam);
int **crearMatriz(int tam);
void imprimirMatriz(int**mat,int tam);
void liberarMatriz(int **mat,int tam);

int main(int argc, char const *argv[])
{
	int N=atoi(argv[1]);
    int Np=atoi(argv[2]);
    int fil,col,suma;
    pid_t pid;
    int **matA,**matB;
    int **matResultado;
    int i,j;
 
    int pipeResul[2],buff[3], pipeT[2];
    int bufferRes[3];
    //Se crean las matrices en las posiciones de memoria
    matA = crearMatriz(N);
    matB = crearMatriz(N);
    matResultado = crearMatriz(N);
    llenarMatriz(matA, N);
    llenarMatriz(matB, N);

    //Se muestran las matrices por pantalla
    printf("Matriz A:\n---------------\n");
 
    imprimirMatriz(matA, N);
 
    printf("Matriz B:\n---------------\n");
 
    imprimirMatriz(matB, N);
 

    //Creamos un pipe para los resultados
    if(pipe(pipeResul)<0)
    {
        perror("Error creando pipe para el padre");
		exit(1);
    }
    //Creamos un pipe para las tareas
    if(pipe(pipeT)<0)
    {
        perror("Error creando pipe para el padre");
        exit(1);
    }
    
    //Creamos N hijos
    for(i=0;i<Np;i++)
    {
        pid=fork();
        if(pid==0)
            break;
    }
    
    //PADRE envia las tareas a sus hijos
    for(fil=0;fil<N;fil++)
    {
        for(col=0;col<N;col++)
        {
            
            
            buff[0] = fil;
            buff[1] = col;
            //printf("padre i:%d j:%d\n",fil, col);
            if(write(pipeT[ESCRIBIR],buff,2*sizeof(buff))<0)
            {
                perror("Error en write del Padre");
                exit(1);
            }
            
        }
    }
    

   
    if (pid == -1)
    {
        	perror("Error al crear hijos");
        	exit(1);
    }else if (pid == 0)
    {
        //HIJO recibimos las tareas
            suma=0;
            for(i=0;i<N*N;i++){
                //Leemos del pipe y los ponemos en el buffer
                if(read(pipeT[LEER],buff,2*sizeof(buff))<0)
                {
                    perror("Error en read del Hijo");
                    exit(1);
                }
                // hacemos el calculo con los indices recibidos y lo almacenamos en suma
                for (int k=0; k<N; k++) {
                    suma += matA[buff[0]][k]*matB[k][buff[1]];
                    printf("sum %d\n", suma);
                }
                //Se guardan los datos en el buffer de reultados
                bufferRes[0]=buff[0];
                bufferRes[1]=buff[1];
                bufferRes[2] = suma;
                //Escribimos al pipe de resultados el contenido del buffer para enviarlo al padre
                if(write(pipeResul[ESCRIBIR],bufferRes,3*sizeof(bufferRes))<0)
                {
                    perror("Error en write del Hijo");
                    exit(1);
                }

            }
            exit(0);
    }
    for (int i = 0; i<N*N; i++) {
        
        //Leemos el resultado del Hijo Correspondiente
        if(read(pipeResul[LEER],bufferRes,3*sizeof(bufferRes))<0)
        {
            perror("Error en el read del pare");
            exit(1);
        }
        //Llenamos la matriz resultado con lo obtenido del hijo
        matResultado[bufferRes[0]][bufferRes[1]]=bufferRes[2];
    }
    
    printf("\n\nLa matriz resultante es:\n---------------\n");
    //Mostramos la solucion
    imprimirMatriz(matResultado, N);
    //liberamos la memoria utilizada
    liberarMatriz(matA, N);
    liberarMatriz(matB, N);
    liberarMatriz(matResultado,N);
    exit(0);
	return 0;
}

void llenarMatriz(int **matriz, int tam){
	for (int i = 0; i < tam; ++i)
	{
		for (int j = 0; j < tam; ++j)
		{
			matriz[i][j] = rand()%20;
		}
	}
}

void liberarMatriz(int **mat, int tam){
    for (int i=0; i<tam; i++) {
        free(mat[i]);
    }
    free(mat);
}

int **crearMatriz(int tam){
	int **array = malloc(tam*sizeof(int*));

	for(int i = 0; i < tam; i++)
	{
		array[i] = malloc(tam * sizeof(int));
	}
	
	return array;
}

void imprimirMatriz(int **mat, int tam){

	for (int i = 0; i < tam; ++i)
	{
		for (int j = 0; j < tam; ++j)
		{
			printf(" | %d | ", mat[i][j]);
		}
		printf("\n");
	}

}
