#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "matrix.h"

void printMatrix(int Matrix[16]);

int main(){
 
	void *shared_memory = (void *)0;

	// Result matrix data structure
	struct matrix *result;

	// Shared memory ID
	int shmid;

	// Create shared memory
	srand((unsigned int)getpid());

	shmid = shmget((key_t)1234, sizeof(struct matrix), 0666 | IPC_CREAT);

	if (shmid == -1) {
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	shared_memory = shmat(shmid, (void *)0, 0);

	if (shared_memory == (void *)-1) {
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	result = (struct matrix *)shared_memory;

	// Define M and N
	struct matrix M;
	M.columns = 4;
	M.rows = 4;
	int mdata[16] = {
				1, 2, 3, 4,
				5, 6, 7, 8,
				4, 3, 2, 1,
				8, 7, 6, 5
			 };

	for (int i = 0; i < 16; i++){
		M.data[i] = mdata[i];
	}

	struct matrix N;
	N.columns = 4;
	N.rows = 4;
	int ndata[16] = {
				1, 3, 5, 7,
				2, 4, 6, 8,
				7, 3, 5, 7,
				8, 6, 4, 2
			 };
	for (int i = 0; i < 16; i++){
		N.data[i] = ndata[i];
	}

	printf("\nParent Process: %i To perform matrix multiplication using 4 processes...\n", getpid());

	// Print input matrices
	printf("\nMatrix M:\n");
	printMatrix(M.data);
	printf("\n");
	printf("Matrix N:\n");
	printMatrix(N.data);

	pid_t pid[4], wpid;

	// Variable used by parent process to wait until child process dies
	int tmp;

	// Create 4 child processes
	for (int i = 0; i < 4; i++){
		pid[i] = fork();
		if (pid[i] == 0){
			break;
		}
	}


	if ( pid[0] == 0 ){

		// P1 calculates the first row of the result matrix

		printf("\nChild Process %i: working with row 1\n", getpid());

		int row1[N.rows];
		for (int i = 0; i < N.rows; i++){
			int sum = 0;
			for (int j = 0; j < M.columns; j++){
				sum = sum + (M.data[j] * N.data[(j * N.columns) + i]);
			}
			row1[i] = sum;
		}

		// Stores the number of rows and columns of result matrix
		result->rows = M.rows;
		result->columns = N.columns;

		// Store the result of row 1
		for (int i = 0; i < N.columns; i++){
			(result->data)[i] = row1[i];
		}
	
		exit(EXIT_SUCCESS);

	} else if(pid[1] == 0){

		// P2 Calculates the second row of the resulting matrix

		printf("\nChild Process %i: working with row 2\n", getpid());

		int row2[N.columns];
		for (int i = 0; i < N.rows; i++){
			int sum = 0;
			for (int j = 0; j < M.columns; j++){
				sum = sum + (M.data[4 + j] * N.data[(j * N.columns) + i]);
			}
			row2[i] = sum;
		}

		// Store the result of row 2
		for (int i = 0; i < N.columns; i++){
			(result->data)[i + 4] = row2[i];
		}

		exit(EXIT_SUCCESS);

	} else if (pid[2] == 0){

		// P3 Calculates row 3 of the resulting matrix

		printf("\nChild Process %i: working with row 3\n", getpid());

		int row3[4];
		for (int i = 0; i < N.rows; i++){
			int sum = 0;
			for (int j = 0; j < M.columns; j++){
				sum = sum + (M.data[8 + j] * N.data[(j * N.columns) + i]);
			}
			row3[i] = sum;
		}

		// Store the result of row 3
		for (int i = 0; i < N.columns; i++){
			(result->data)[i + 8] = row3[i];
		}
		
		exit(EXIT_SUCCESS);	

	} else if (pid[3] == 0){

		// P4 Calculates the 4th row of the result matrix

		printf("\nChild Process %i: working with row 4\n", getpid());

		int row4[4];

		for (int i = 0; i < N.rows; i++){
			int sum = 0;
			for (int j = 0; j < M.columns; j++){
				sum = sum + (M.data[12 + j] * N.data[(j * N.columns) + i]);
			}
			row4[i] = sum;
		}

		// Store the result of row 4
		for (int i = 0; i < N.columns; i++){
			(result->data)[i + 12] = row4[i];
		}

		exit(EXIT_SUCCESS);

	} 

	//  Parent process waits for all children to die
	while ((wpid = wait(&tmp)) > 0);
	
	// Print result matrix
	printf("\nResult matrix Q: \n");
	printMatrix(result->data);

	printf("\n");

	// Delete shared memory and exit
	if (shmdt(shared_memory) == -1) {
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}

	if (shmctl(shmid, IPC_RMID, 0) == -1){
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

}

// Helper function to print matrices to console
void printMatrix(int matrix[16]){
	for (int i = 1; i <= 16; i++){
		printf("%3i ", matrix[i-1]);
		if (i%4 == 0)
			printf("\n");
	}
}
