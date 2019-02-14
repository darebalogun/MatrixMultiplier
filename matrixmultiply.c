#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "matrix.h"

int main(){
 //
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
	int mdata[][4] = {
				{1, 2, 3, 4},
				{5, 6, 7, 8},
				{4, 3, 2, 1},
				{8, 7, 6, 5}
			 };
	M.data = (int *) mdata;

	struct matrix N;
	N.columns = 4;
	N.rows = 4;
	int ndata[][4] = {
				{1, 3, 5, 7},
				{2, 4, 6, 8},
				{7, 3, 5, 7},
				{8, 6, 4, 2}
			 };
	N.data = (int *) ndata;

	pid_t pid, wpid;

	pid = fork();

	//result->done = 0;

	switch(pid){
		case -1:
			perror("fork failed");
			exit(EXIT_FAILURE);
		case 0:
			pid = fork();
			switch (pid){
				case -1:
					perror("fork failed");
					exit(EXIT_FAILURE);
				case 0:
					// P3 Calculates row 3 of the resulting matrix
                    printf("P3: %i\n", getppid());

					int row3[4];
					for (int i = 0; i < N.rows; i++){
						int sum = 0;
						for (int j = 0; j < M.columns; j++){
							sum = sum + (M.data[8 + j] * N.data[(j * N.columns) + i]);
						}
						row3[i] = sum;
					}

					for (int i = 0; i < N.columns; i++){
						(result->data)[i + 8] = row3[i];
					}
					
                    exit(EXIT_SUCCESS);
				default:;
			}
			// P2 Calculates the second row of the resulting matrix
            printf("P2: %i\n", getppid());

			int row2[N.columns];
			for (int i = 0; i < N.rows; i++){
				int sum = 0;
				for (int j = 0; j < M.columns; j++){
					sum = sum + (M.data[4 + j] * N.data[(j * N.columns) + i]);
				}
				row2[i] = sum;
			}

			result->rows = M.rows;
			result->columns = N.columns;
			for (int i = 0; i < N.columns; i++){
				(result->data)[i + 4] = row2[i];
			}

            exit(EXIT_SUCCESS);

	}

	pid = fork();

	switch(pid){
		case -1:
			perror("fork failed");
			exit(EXIT_FAILURE);
		case 0:
			// P4 Calculates the 4th row of the result matrix
            printf("P4: %i\n", getppid());

			int row4[4];

			for (int i = 0; i < N.rows; i++){
				int sum = 0;
				for (int j = 0; j < M.columns; j++){
					sum = sum + (M.data[12 + j] * N.data[(j * N.columns) + i]);
				}
				row4[i] = sum;
			}

			for (int i = 0; i < N.columns; i++){
				(result->data)[i + 12] = row4[i];
			}

            exit(EXIT_SUCCESS);
        default:;
	}

	// P1 calculates the first row of the result matrix
    printf("P1: %i\n", getppid());

	int status;

	int row1[N.rows];
	for (int i = 0; i < N.rows; i++){
		int sum = 0;
		for (int j = 0; j < M.columns; j++){
			sum = sum + (M.data[j] * N.data[(j * N.columns) + i]);
		}
		row1[i] = sum;
	}
	result->rows = M.rows;
	result->columns = N.columns;
	result->data = row1;

	int i = 0;
	while(i < 1000000000){
		i++;
	}

	printf("P1: \n");
	for (int i = 1; i <= 16; i++){
		printf("%i ", result->data[i-1]);
		if (i%4 == 0)
			printf("\n");
	}
	
	exit(EXIT_SUCCESS);
}
