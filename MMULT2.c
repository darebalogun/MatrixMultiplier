#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "matrix.h"

struct matrix N,M;

struct matrix *result;

void printMatrix(int Matrix[16]);

void calculateRow(int x);

int main(int argc, char *argv[]){

	if (argc != 2 | !(strcmp(argv[1], "1") == 0 | strcmp(argv[1], "2") == 0 | strcmp(argv[1], "4") == 0) ){
		printf("Error! Please provide a single argument (1, 2 or 4) to indicate the number of child processes to use\n");
		exit(EXIT_FAILURE);
	}

	void *shared_memory = (void *)0;

	// Result matrix data structure
	//struct matrix *result;

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
	//struct matrix M;
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

	//struct matrix N;
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

	printf("\nMatrix M:\n");
	printMatrix(M.data);
	printf("\n");
	printf("Matrix N:\n");
	printMatrix(N.data);

	int n;
	if (strcmp(argv[1], "1") == 0){
		n = 1;
	} else if (strcmp(argv[1], "2") == 0){
		n = 2;
	} else {
		n = 4;
	}

		// Print input matrices
	printf("\nParent Process %i: To perform matrix multiplication using %i processes...\n", getpid(), n);

	pid_t pid[n], wpid;

	// Variable used by parent process to wait until child process dies
	int tmp;

	// Create 4 child processes


	for (int i = 0; i < n; i++){
		pid[i] = fork();
		if (pid[i] == 0){
			break;
		}
	}

	// Stores the number of rows and columns of result matrix
	result->rows = M.rows;
	result->columns = N.columns;

	switch (n){
		case 1:
			if (pid[0] == 0){
				calculateRow(1);
				calculateRow(2);
				calculateRow(3);
				calculateRow(4);
				exit(EXIT_SUCCESS);
			}
			break;
		case 2:
			if (pid[0] == 0){
				calculateRow(1);
				calculateRow(2);
				exit(EXIT_SUCCESS);
			} else if (pid[1] == 0){
				calculateRow(3);
				calculateRow(4);
				exit(EXIT_SUCCESS);
			}
			break;
		case 4:
			if (pid[0] == 0){
				calculateRow(1);
				exit(EXIT_SUCCESS);
			} else if (pid[1] == 0){
				calculateRow(2);
				exit(EXIT_SUCCESS);
			} else if (pid[2] == 0){
				calculateRow(3);
				exit(EXIT_SUCCESS);
			} else if (pid[3] == 0){
				calculateRow(4);
				exit(EXIT_SUCCESS);
			}			
			break;
	}

	//  Parent process waits for all children to die
	while ((wpid = wait(&tmp)) > 0);
	
	// Print result matrix
	printf("\nResult matrix Q: \n");
	printMatrix(result->data);

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

void printMatrix(int matrix[16]){
	for (int i = 1; i <= 16; i++){
		printf("%3i ", matrix[i-1]);
		if (i%4 == 0)
			printf("\n");
	}
}

void calculateRow(int x){
	if (x >= 1 | x <= 4){
		printf("\nChild Process %i: working with row %i\n", getpid(), x);

		int row[4];

		for (int i = 0; i < N.rows; i++){
			int sum = 0;
			for (int j = 0; j < M.columns; j++){
				sum = sum + (M.data[(4*(x-1)) + j] * N.data[(j * N.columns) + i]);
			}
			row[i] = sum;
		}

		// Store the result of row
		for (int i = 0; i < N.columns; i++){
			(result->data)[i + 4*(x-1)] = row[i];
		}
	}
}