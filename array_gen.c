  
#include <stdio.h>

#include <math.h>

//Generates all arrays of ones
double *printarr(long long *n,double *x) {
    long long i;
    
    for(i = 0;i<*n;i++) {
          x[i] = 1.0;
        //printf("array %lf\n",x[i]);
    }
    
    return x;
}
// Generates all arrays of zeroes
double *zeros(long long *n,double *x) {
    long long i;
    
    for(i = 0;i<*n;i++) {
          x[i] = 0.0;
        //printf("%lf\n",a[i]);
    }
    
    return x;
}

// Generates randomlocal
double *random1(long long *n,double *x) {
    long long i;
    
    for(i = 0;i<*n;i++) {
          x[i] = -1.0 + 2.0*drand48();
          
        //printf("%lf\n",a[i]);
    }
    
    return x;
}
//Generates alternating vector
double *alternate(long long *n,double *x) {
    long long i;
    
    for(i = 0;i<*n;i++) {
          if(i % 2 == 0){
		x[i] = 1;
          }else{
		x[i] = -1;
		}
    }
    
    return x;
}

// Generates linear vector
double *lin(long long *n,double *x) {
    double j = (double)rand()/RAND_MAX;
    long long i;
    for(i = 0;i<*n;i++) {
          x[i] = (*n)*j + 1;
          
        //printf("%lf\n",a[i]);
    }
    
    return x;
}



