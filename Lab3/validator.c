#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

int *sudokuGrid[9][9];

/* structure for passing data to threads */
typedef struct
{
	int row;
	int column;
} parameters;

int *validate_row(void *arg){
	int* rownum = (int*) arg;
	int used[9] = 0;
}

int *validate_col(void *arg){

}

int *validate_box(void *arg){

}

int main(int argc,char *argv[])
{
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			sudokuGrid[i][j] = (i+j) % 9;
		}
	}

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			printf("%d ", sudokuGrid[i][j]); ;
		}
		printf("\n");
	}

  return 0;
}
