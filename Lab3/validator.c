#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


/* structure for passing data to threads */
typedef struct
{
	//row is the first number, col is second 
	int sudokuGrid[9][9];
	//selector is the row/col/box number we're checking
	int selector;
} parameters;

void *validate_row(void *arg){
	parameters* data = (parameters*) arg;
	// int* rownum = (int*) arg;
	//used track tells which numbers have been used in this row
	int used[10] = {0};

	printf("Hello From Thread\n");
	//iterate through each element in the row
	for (int i = 0; i < 9; i++)
	{
		//if the element is nonzero (actual data)
		int currElement = data -> sudokuGrid[data -> selector][i];
		if (currElement != 0)
		{
			//if the element has already been used this row
			if (used[currElement] == 1)
			{
				//row is not valid, return false 0
				printf("0\n");
				return NULL;
			}else{
				used[currElement] = 1;
			}
		}
	}
	//row is valid, return true 1;
	printf("1\n");
	return NULL;
}

int *validate_col(void *arg){
	parameters* data = (parameters*) arg;
	//used track tells which numbers have been used in this col
	int used[10] = {0};

	printf("Hello From Thread\n");
	//iterate through each element in the col
	for (int i = 0; i < 9; i++)
	{
		//if the element is nonzero (actual data)
		int currElement = data -> sudokuGrid[i][data -> selector];
		if (currElement != 0)
		{
			//if the element has already been used this col
			if (used[currElement] == 1)
			{
				//col is not valid, return false 0
				printf("0\n");
				return NULL;
			}else{
				used[currElement] = 1;
			}
		}
	}
	//cpl is valid, return true 1;
	printf("1\n");
	return NULL;
}

int *validate_box(void *arg){
	parameters* data = (parameters*) arg;
	//used track tells which numbers have been used in this box
	int used[10] = {0};

	printf("Hello From Thread\n");
	//iterate through each element in the box
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			//data -> selector => 0 1 2 start at row 0
			//data -> selector => 3 4 5 start at row 3
			//data -> selector => 6 7 8 start at row 6

			//data -> selector => 0 3 6 start at col 0
			//data -> selector => 1 4 7 start at col 3
			//data -> selector => 2 5 8 start at col 6

			//if the element is nonzero (actual data)
			int currElement = data -> sudokuGrid[i+(data -> selector/3)*3][j+(data -> selector%3)*3];
			printf("row:%d col:%d\n",i+(data -> selector/3)*3,j+(data -> selector%3)*3 );
			if (currElement != 0)
			{
				//if the element has already been used this box
				if (used[currElement] == 1)
				{
					//box is not valid, return false 0
					printf("0\n");
					return NULL;
				}else{
					used[currElement] = 1;
				}
			}
		}
	}
	//box is valid, return true 1;
	printf("1\n");
	return NULL;
}

int main(int argc,char *argv[])
{
	pthread_t rowthread;
	pthread_t colthread;
	pthread_t boxthread;
	parameters *data = (parameters *) malloc(sizeof(parameters)); 

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			data->sudokuGrid[i][j] = i;
		}
	}

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			printf("%d ", data -> sudokuGrid[i][j]); ;
		}
		printf("\n");
	}


	data -> selector = 4;
	pthread_create(&boxthread, 0, validate_box, (void *) data);

	(void) pthread_join(boxthread,NULL);
  return 0;
}
