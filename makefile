CC = mpicc

ping_final: testdot.c
	   $(CC) -o testdot testdot.c