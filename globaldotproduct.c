#include<stdio.h>
#include<stdlib.h>
#include "ddot.c"

#include "mpi.h"
void globaldotproduct(long long *localn, double *localx, double *localy, double *dotval, int *localrank,int *nprocs,MPI_Comm *comm)
{
    long long x=1;
    long long y=1;
    double total_value;
    long long i;
    int j;
    

    total_value= ddot(localn,localx,x,localy,y);
    double recieve[*nprocs];
    double send_buff[1];
    send_buff[0]=total_value;
    
    MPI_Allgather(&send_buff, 1, MPI_DOUBLE, &recieve, 1, MPI_DOUBLE, MPI_COMM_WORLD);
    double sum = 0.0;
     for(i=0;i<*nprocs;i++){
       sum += recieve[i];           
     }

      *dotval = sum;
    
}



