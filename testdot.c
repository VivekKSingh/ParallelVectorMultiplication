// usual systems files
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
//  ....

// MPI, and any personal quirks
#include "mpi.h"


// in-directory functions used 

#include "globaldotproduct.c"
#include "setvec.c"
//#include "vectypes.h"

#include "elapsedtime.c"
//#include "globaldotproduct.h"

//--------------------------------------------------------------------------------
//   
// Program to time a global dotproduct on a parallel machine, for various
// sizes of vectors and numbers of processes. The vector sizes to test are read
// in from a file named "sizes".  
//
// To make sure that the dotproduct of small vectors are computed without clock
// resolution interfering, the dotproducts are timed inside of a timing block.
// The input variable nflops is the minimal number of flops to be computed 
// within a timing block. Since a dotproduct of length n takes 2*n flops, the
// number of repetitions to make for a vector of length n is nflops/(2*n). Beware
// that this likely means nflops needs to hold a larger value than a 32-bit 
// integer can handle. For parallel systems, n itself may be over 2G, which means
// 64-bit integers are needed in general.
//
//---------
// Input 
//---------
// The input file "sizes" will have a single line, with the four values
//      min_length   max_length   stride_length   nflops
// and in Matlab notation, the sizes tested will be the array of lengths
//      min_length:stride_length:max_length
// [Beware: in Fortran the stride would be last, not in the middle]
// For developing, debugging, and validating you can probably use some small
// values initially, e.g. an input file with the single line
//      1000   8000  500  1000000000
// For the timings any conclusions are drawn from, try one run with
//      100000   800000  100000  1000000000
// and another with
//     1000000   80000000  1000000  4000000000
//
//
//----------
// Output 
//----------
// Output is to a file named 'results'. This will allow the data to
// be read into a Matlab script for plotting, etc. So the output format must be
// strictly adhered to. All lines must have the same number of numbers, and any
// other lines must either be blank or be preceeded by a "%" character (the
// Matlab comment indicator).
// 
// Each time the file is opened it should be in append mode, to allow accumulating
// results and timings. When your code is validated and working correctly, you
// can blow out the results file and start it over to accumulate the timings
// you will use to report on the code.
// 
// Each line of output in the results file should have:
//      vector_length   nreps  total_time  average_time  nprocs  dotproductvalue
// where
//      vector_length   = vector length the timing is for
//      nreps           = number of repetitions of the dotproduct inside the 
//                        timing block
//      total_time      = time in seconds required for the timing block
//      average_time    = total_time divided by the number of repetitions
//      nprocs          = how many processes were used for the timing
//      dotproductvalue = the actual number returned for the global dotproduct
// The information in fields 2-4 is has a redundancy, but print all of them out 
// anyway. In addition, the times and the dotproduct value must be printed with 
// 17 significant digits, in exponential format. 
// 
//--------------
// Interfaces 
//--------------
// The interfaces are given in the .h files in this tar-ball. They are
//        ddot.h  for a local dotproduct
//        globaldotproduct.h  for a global dotproduct
//        setvec.h  to set up test cases for validation of correctness
//        elapsedtime.h  for a timing wrapper
// The elapsedtime() function has five options of getting the time. Part of the
// assignment is to comment out the one currently in it, and put in a version 6
// that calls MPI_Wtime. 
//
//-------------------
// Randall Bramley
// Department of Computer Science
// Indiana University
//-------------------
// Started: Thu 26 Apr 2012, 01:49 PM
// Last Modified: Thu 26 Apr 2012, 06:28 PM
//--------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    //----------------
    // Work outline:
    //----------------
    
    //Initializing the variables
       
	FILE    *fp;
    
	int     proc, rank;
	int	 Root = 0;
	long long nflops;
	
	long long ranks;
	long long procs;
	
       MPI_Status status;
       
       //Allocating the buffer
	double *Mybuffer_A, *Mybuffer_B,*Mybuffer_a,*Mybuffer_b;
	double total_value;
	long long parameters[4],RecvCount,*SendCount;
     
        // Initialize MPI
         MPI_Init(&argc, &argv); 
         MPI_Comm comm = MPI_COMM_WORLD;
         MPI_Comm_rank(MPI_COMM_WORLD, &rank);
         MPI_Comm_size(MPI_COMM_WORLD, &proc);
         ranks = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
         procs = MPI_Comm_size(MPI_COMM_WORLD, &proc);
        
    
    
    // Process 0 reads in the file "sizes", sends values to all other processes
      if(rank == Root) {
      //parameters = (long long*)malloc(4*sizeof(long long));
        FILE   *fp;
        int i,j,n;
      
         fp = fopen("sizes", "r");
         fscanf(fp, "%lld %lld %lld %lld",&parameters[0],&parameters[1],&parameters[2],&parameters[3]);
      
         fclose(fp);
       }
     
   
      MPI_Bcast(&parameters, 4, MPI_LONG_LONG, Root, MPI_COMM_WORLD);
 
      long long i,j;
      long long vectorsize;
      int nreps; 
      float start;
      float end;
      float totaltime;
      float averagetime;
      nflops = parameters[3];
     
     // File to write the results in 
     FILE  *ofp;	
     char outputFilename[] = "results";
     
     // Starting the main computation for splitting the vector and computing the dot products of all the vectors	
     for(i=parameters[0];i<=parameters[1];i+=parameters[2])
     { 	
            // Calculating the number of repetitions   
                 nreps = nflops/(2*i);
            // Splitting the data for each process and giving the remainder to the last process      
                  if(rank == (proc-1)){
                   vectorsize = (i/proc) + (i%proc);
                   }else{
                         vectorsize = (i/proc);
                   }
                 
		// Allocating the local buffers	
                 double *buffer_x;
                 double *buffer_y;
                 buffer_x = (double*)malloc(vectorsize*sizeof(double)); 
                 buffer_y = (double*)malloc(vectorsize*sizeof(double));
              // Calling setvec to specify the vector type   
                 setvec(randomlocal,&vectorsize,buffer_x,&ranks,&procs);
                 setvec(randomlocal,&vectorsize,buffer_y,&ranks,&procs);
                 int z;
                 
              // Barrier   	
                 MPI_Barrier(MPI_COMM_WORLD);
		
		// Starting timer	
                 start = wtime();
              // Looping on the repetitions   
                 for(j=0;j<nreps;j++){                                                            
                 globaldotproduct(&vectorsize,buffer_x,buffer_y,&total_value,&rank,&proc,&comm);
		   
                 }
		   
		// Free the local buffers
                 free(buffer_x);
                 free(buffer_y);
		// Barrier  
	          MPI_Barrier(MPI_COMM_WORLD);
		// End timer
                 end = wtime();
		// Calculating total time
  	          totaltime = end+start;
		// Calculating average time
	          averagetime = totaltime/nreps;
              	   
                 if(rank == 0){
	           ofp = fopen(outputFilename,"a+");
       //Writing inside the file
                 
		   printf("%lld\t%lld\t%.17e(E)f\t%.17e(E)f\t%d\t%.17e(E)f\t\n",i,nreps,totaltime,averagetime,proc,total_value);
		   fprintf(ofp,"%lld\t%lld\t%.17e(E)f\t%.17e(E)f\t%d\t%.17e(E)f\t\n",i,nreps,totaltime,averagetime,proc,total_value);
		
                 fclose(ofp);
	          }
  	        
              }
  
		// Finalize      
              MPI_Finalize();       

}    
    
 


