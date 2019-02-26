# MatrixMultiplier
Perform matrix multiplication concurrently using multiple processes

*********Files description:

MMULT1.c: This contains the program for part one which computes the result of the multiplication of two 4 X 4 matrices using 4 processes.

matrix.h: Tis header file defines a matrix structure which is used to create the shared memory in the program

MMULT2.c: This contains the program for part 2 which computes the result of the matrix multiplication of two 4 X 4 matrices using 
a variable number of processes (1, 2 or 4) which is input as an argument.

Makefile: A make file to help compile all dependecies of the programs


*********Instructions to run:

In order to compile each individual part, you must go to the terminal and access the directory 
location of the assignment files. Once you get to each individual part's folder you can just type 
make and then run myapp afterwards. 

For Part 1 you do not specify any argument.

For part 2 you must specify a single argument of 1, 2 or 4. Any other input will cause the program to terminate