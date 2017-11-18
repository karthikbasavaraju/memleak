//#include "mem_trac.h"
#include <stdio.h>
#include <stdlib.h>
void allocate_mem(double*** arr, int rows, int cols);
void deallocate_mem(double*** arr, int n);
double** readMatrixf(int rows, int cols);
void readMatrix(double ***a, int rows,int cols);
void printMatrix(double** a, int rows, int cols);
void printMatrixE(double** a, int rows, int cols);
void multMatrixp(double **A, double **B, double **C,int r1,int c1,int r2,int c2);
void multMatrixpp(double **A, double **B, double ***C,int ro1,int co1,int ro2,int co2);
double** multMatrixpf(double **A, double **B, int ro1,int co1,int ro2,int co2);

//______________________________________________________________________________

int main()
{
   int ro1, co1, ro2, co2;
   double **a1, **b1, **c1;    

ro1=2; co1=3;
ro2=3; co2=4;

printf("Ex1:__________________________________________________"
    "__________________________ \n");


double (*(a[])) = {
(double[]) { 1, 3, 5},
(double[]) {2, 4, 0}};


double (*(b[])) = {
(double[]) {6, 2, 4, 8},
(double[]) {1, 7, 0, 9},
(double[]) {0, 3, 5, 1}};


printMatrix(a,ro1,co1);    
printMatrix(b,ro2,co2);


printf("MatMult \n");
double **c;
allocate_mem(&c,ro1,co2);
multMatrixp(a, b, c, ro1, co1, ro2, co2);  
printMatrix(c,ro1,co2);        
printMatrixE(c,ro1,co2);      

deallocate_mem(&c,ro1);  


printf("Ex2:__________________________________________________"
    "__________________________ \n");

scanf("%d%d", &ro1, &co1);
readMatrix(&a1,ro1,co1);
printMatrix(a1,ro1,co1); 
//deallocate_mem(&a1,ro1); 
//printMatrix(a1,ro1,co1); 


scanf("%d%d", &ro2, &co2);
readMatrix(&b1,ro2,co2);
printMatrix(b1,ro2,co2);

printf("MatMult \n");
multMatrixpp(a1, b1, &c1, ro1, co1, ro2, co2);  
printMatrix(c1,ro1,co2); 
printMatrixE(c1,ro1,co2);      

deallocate_mem(&a1,ro1); 
deallocate_mem(&b1,ro2);
deallocate_mem(&c1,ro1);  


printf("Ex3:__________________________________________________"
    "__________________________ \n");

scanf("%d%d", &ro1, &co1);
a1=readMatrixf(ro1,co1);
printMatrix(a1,ro1,co1); 
//deallocate_mem(&a1,ro1); 
//printMatrix(a1,ro1,co1); 


scanf("%d%d", &ro2, &co2);
b1=readMatrixf(ro2,co2);
printMatrix(b1,ro2,co2);

printf("MatMult \n");
c1=multMatrixpf(a1, b1, ro1, co1, ro2, co2);  
printMatrix(c1,ro1,co2); 
printMatrixE(c1,ro1,co2);      

deallocate_mem(&a1,ro1); 
deallocate_mem(&b1,ro2);
deallocate_mem(&c1,ro1);  



    return 0;
}

//______________________________________________________________________________
void allocate_mem(double*** arr, int rows, int cols)
{
  int i;
  *arr = (double**)malloc(rows*sizeof(double*));
  for( i=0; i<rows; i++)
    (*arr)[i] = (double*)malloc(cols*sizeof(double));
} 

//______________________________________________________________________________
void deallocate_mem(double*** arr, int rows){
 int i;
    for (i = 0; i < rows; i++)
        free((*arr)[i]);
    free(*arr); 
}

//______________________________________________________________________________
double** readMatrixf(int rows, int cols)
{
double    **a; // Define a local pointer to keep rest of the code intact
int i, j;

a= (double**) malloc(rows*sizeof(double*));
for(i=0;i<rows;i++)
a[i]=(double*)malloc(cols*sizeof(double));

    for(i=0;i<rows;i++)
        for(j=0;j<cols;j++)
            scanf("%lf",&a[i][j]);
   return a;            

}

//______________________________________________________________________________

void readMatrix(double ***a, int rows,int cols)
{
   int i, j;

*a= (double**) malloc(rows*sizeof(double*));
for(i=0;i<rows;i++)
(*a)[i]=(double*)malloc(cols*sizeof(double));
    for(i=0;i<rows;i++)
        for(j=0;j<cols;j++)
            scanf("%lf",&(*a)[i][j]);
}

//______________________________________________________________________________
void printMatrix(double** a, int rows, int cols)
{
    int i, j;
   printf("Matrix[%d][%d]\n",rows,cols);    
   for(i=0;i<rows;i++){
      for(j=0;j<cols;j++)
      printf("%8.3lf ",a[i][j]);
      printf("\n");
   }
   printf("\n");   
}

//______________________________________________________________________________
void printMatrixE(double** a, int rows, int cols)
{
    int i, j;
   printf("Matrix[%d][%d]\n",rows,cols);    
   for(i=0;i<rows;i++){
      for(j=0;j<cols;j++)
      printf("%9.2e ",a[i][j]);
      printf("\n");
   } 
   printf("\n");     
}


//______________________________________________________________________________

void multMatrixp(double **A, double **B, double **C,int ro1,int co1,int ro2,int co2)
{
    int i, j, k;
    for(i = 0; i < ro1; i++) {
        for(j = 0; j < co2; j++) {
            C[i][j] = 0;
            for(k = 0; k < co1; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

//______________________________________________________________________________

void multMatrixpp(double **A, double **B, double ***C,int ro1,int co1,int ro2,int co2)
{
    int i, j, k;
*C= (double**) malloc(ro1*sizeof(double*));
for(i=0;i<ro1;i++)
(*C)[i]=(double*)malloc(co2*sizeof(double));

    for(i = 0; i < ro1; i++) {
        for(j = 0; j < co2; j++) {
            (*C)[i][j] = 0.0;
            for(k = 0; k < co1; k++) {
                (*C)[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}


//______________________________________________________________________________

double** multMatrixpf(double **A, double **B, int ro1,int co1,int ro2,int co2)
{
    int i, j, k;
    double **C;
C= (double**) malloc(ro1*sizeof(double*));
for(i=0;i<ro1;i++)
C[i]=(double*)malloc(co2*sizeof(double));

    for(i = 0; i < ro1; i++) {
        for(j = 0; j < co2; j++) {
            C[i][j] = 0.0;
            for(k = 0; k < co1; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
     return C;              
}
