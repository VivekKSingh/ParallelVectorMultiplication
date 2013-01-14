#include<stdio.h>
#include "array_gen.c"
#include "vectypes.h"
void setvec(
        vectype vtype,     // Type of the vector to set up
        long long *veclen,  // Length of the vector
        double *x,          // The vector 
        long long *rank,    // rank of calling process
        long long *nprocs) // p = number of processes in the communicator
{
        switch(vtype)
       {
                     case 0:
                     // Calling zero    
                      x = zeros(veclen,x);
                           
                     break;
                     
			case 1:
			// Calling ones
                      x = printarr(veclen,x);
			break;
			//Calling alternates
			case 2:
			x = alternate(veclen,x);
			break;
			// Calling linear
			case 3:
			x = lin(veclen,x);
			break;
			//  Calling randomlocal
			case 4:
                     x = random1(veclen,x);
                     break; 	      
                     
			default :
                     x = zeros(veclen,x);    
       }
       
}                
