#include<stdio.h>
#include<stdlib.h>
//#include "array_gen.c"

double ddot(
        long long *veclen,  
        double *x,        
        long long incx,    
        double *y,           
        long long incy);



double ddot(long long *veclen,double *x, long long incx, double *y, long long incy)
{
       long long i;
      
       int j;
       double local_product=0.0;
           
       for(j=0;j<*veclen;j++){
	           
            local_product += x[j]*y[j];
                  
       }

       return local_product;
         
}
