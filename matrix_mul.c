#include "mem_trac.h"
//#include <malloc.h>
#include<stdio.h>

#include<stdlib.h>

int main()
{

        int *ans,*first,*second;
        int *A,*B,*C;
        int i,j,k=0;
        int rowA,colA,sizeA,sizeB,sizeC;
        int rowB,colB;

        printf("Enter the row's and column of 1st matrix\n");
        scanf("%d%d",&rowA,&colA);


        printf("Enter the row's and column of 2nd matrix\n");
        scanf("%d%d",&rowB,&colB);

        if(colA!=rowB)
        {
            printf("Error => colA must be equal to rowB\n");
       
            exit(EXIT_SUCCESS);
        }


        sizeC = rowA*colB;
        sizeA = rowA*colA;
        sizeB = rowB*colB;

        A  = (int *)malloc(sizeA*sizeof(int *));
        first = A;

        B = (int *)malloc(sizeB*sizeof(int *));
        second = B;

        C    = (int *)malloc(sizeC*sizeof(int *));
        ans = C;


        printf("Enter the elements of the first matrix A\n");

        for(i=0;i<sizeA;i++,first++)
        scanf("%d",first);

        printf("Enter the elements of the second matrix B\n");

        for(i=0;i<sizeB;i++,second++)
        scanf("%d",second);

        first=A;        
        second= B;      

        if(rowA==1 && colB==1)
        {
            for(i=0;i<rowA;i++)
            {
                for(j=0;j<colB;j++)
                {
                *ans=0;
                for(k=0;k<rowB;k++)
                    *ans = *ans + (*(first + (k + i*colA))) * (*(second + (j+k*colB)));
                ans++;
                }//j
            }//i
        }//if

    else
    {
        for(i=0;i<rowA;i++)
        {
        for(j=0;j<colB;j++)
        {
            *ans=0;
            for(k=0;k<rowB;k++)
                *ans = *ans + (*(first + (k + i*colA))) * (*(second + (j+k*rowB)));
            ans++;
        }//j
        }//i

        }

        printf("\nThe value of matrix 'C' = \n");

        ans = C;

        for(i=0;i<rowA;i++)
        {
         printf("\n");
         for(j=0;j<colB;j++,ans++)
         printf("%d\t",*ans);
         }

        //free(A);
        free(B);
        free(C);
         	//leak_result();
  
    }
