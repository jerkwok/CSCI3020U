#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define BUFFER_LEN 256

/* structure for passing data to threads */
typedef struct
{
  //row is the first number, col is second 
  int sudokuGrid[9][9];
  //[R/C/B]selector is the row/col/box number we're checking
  int Rselector;
  int Cselector;
  int Bselector;

} parameters;

char** tokenize2(char *input, char *delim){
  //takes an input string with some delimiter and returns an array
  //with all the tokens split by the provided delimiter

  //Sample usage:
  //char buffer[] = "a b c";
  //char **user_output;
  //user_output = tokenize(buffer, " ");

  char** tokens = 0;
  size_t num_elements = 0;
  size_t tokens_index  = 0; //keep tracks of the  tokens offset when adding them
  char* input_cpy = input;
  char *input_cpy2 = malloc(1 + strlen(input)); //used with strtok
  if (input_cpy2){
    strcpy(input_cpy2, input);
  }else{
    printf("error copying input\n");
  }

  //iterate through the intput and count # of delims
  while (*input_cpy != NULL){
    if (*delim == *input_cpy){
      num_elements++;
    }input_cpy++;
  }

  num_elements++; //for last object
  num_elements++; //for null terminating value

  //create enough memory for all the elements
  tokens = malloc(sizeof(char*) * num_elements);
  char* token = strtok(input_cpy2, delim);
  while (token){
    //store the token in the tokens array
    *(tokens + tokens_index++) = strdup(token);   //strdup duplicates the string
    token = strtok(0, delim); //next token
  }
  //finally add null value at the end
  *(tokens + tokens_index) = 0;
  return tokens;
}

void load_sudokuGrid(char *filename[],  parameters *data){
  FILE *f;

  f = fopen(filename,"r");
  char buffer[BUFFER_LEN];
  char rowArray[BUFFER_LEN];

  //out now contains the whole row
  for (int i = 0; i < 9; i++){
    fgets(buffer,BUFFER_LEN, f);
    char **out = tokenize2(buffer, " ");
    for (int j = 0; j < 9; j++){
      data->sudokuGrid[i][j] = atoi(out[j]);
    }
    free(out);
  }
  fclose(f);
}


int *validate_row(void *arg){

  parameters* data = (parameters*) arg;
  int myselector = data -> Rselector;
  // int* rownum = (int*) arg;
  //used track tells which numbers have been used in this row
  int used[10] = {0};

  //iterate through each element in the row
  for (int i = 0; i < 9; i++)
    {
      //if the element is nonzero (actual data)
      int currElement = data -> sudokuGrid[myselector][i];
      if (currElement != 0)
	{
	  //if the element has already been used this row
	  if (used[currElement] == 1)
	    {
	      //row is not valid, return false 0
	      pthread_exit(0);;
	    }else{
	    used[currElement] = 1;
	  }
	}
    }
  //row is valid, return true 1;
  pthread_exit(1);

}

int *validate_col(void *arg){
  parameters* data = (parameters*) arg;
  int myselector = data -> Cselector;
  //used track tells which numbers have been used in this col
  int used[10] = {0};

  //iterate through each element in the col
  for (int i = 0; i < 9; i++)
    {
      //if the element is nonzero (actual data)
      int currElement = data -> sudokuGrid[i][myselector];
      if (currElement != 0)
	{
	  //if the element has already been used this col
	  if (used[currElement] == 1)
	    {
	      //col is not valid, return false 0
	      pthread_exit(0);
	    }else{
	    used[currElement] = 1;
	  }
	}
    }
  //cpl is valid, return true 1;
  pthread_exit(1);
}

int *validate_box(void *arg){
  parameters* data = (parameters*) arg;
  int myselector = data -> Bselector;
  //used track tells which numbers have been used in this box
  int used[10] = {0};

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
	  int currElement = data -> sudokuGrid[i+(myselector/3)*3][j+(myselector%3)*3];
	  if (currElement != 0)
	    {
	      //if the element has already been used this box
	      if (used[currElement] == 1)
		{
		  //box is not valid, return false 0
		  pthread_exit(0);
		}else{
		used[currElement] = 1;
	      }
	    }
	}
    }
  //box is valid, return true 1;
  pthread_exit(1);
}

int solve(int row, int col, parameters *data){
  int *rowvalid = 0 ,*colvalid = 0,*boxvalid = 0;

  pthread_t rowthread;
  pthread_t colthread;
  pthread_t boxthread;

  //row and col is the current square that we are trying to solve

  //if the current square is not 0, skip over it and increase col
  while((row < 9) && (data -> sudokuGrid[row][col] != 0)){
    col++;
    //if the current column is 9 increase the row and set the column to 0
    if (col == 9)
    {
      row++;
      col = 0;
    }
  }

  // if the row is 9 then we are DONE return true
  if (row == 9)
  {
    return 1;
  }

  //iterate through the pssibilities.
  for (int i = 1; i <= 9; i++)
  {

    rowvalid = 0 , colvalid = 0, boxvalid = 0;

    //set the current square to the current guess
    data -> sudokuGrid[row][col] = i;
    //if the current row, box and column are all okay recursively call solve on myself

    data -> Bselector = (col/3) + (row/3)*3;
    pthread_create(&boxthread, 0, validate_box, (void *) data);

    data -> Cselector = col;
    pthread_create(&colthread, 0, validate_col, (void *) data);

    data -> Rselector = row;
    pthread_create(&rowthread, 0, validate_row, (void *) data);


    (void) pthread_join(boxthread,&boxvalid);
    (void) pthread_join(colthread,&colvalid);
    (void) pthread_join(rowthread,&rowvalid);
    //if the recursive call returns true return true

    if ((rowvalid == 1) && (colvalid == 1) && (boxvalid == 1))
    {
      if (solve(row,col,data) == 1)
      {
        return 1;
      }
    }
  }

  //if we have iterated through the possibilities and nothing works then we need to backtrack
  //set current square to 0 and return false
  data -> sudokuGrid[row][col] = 0;
  return 0;
}

int main(int argc,char *argv[])
{

  //init threads
  pthread_t rowthread;
  pthread_t colthread;
  pthread_t boxthread;

  int *rowvalid = 0 ,*colvalid = 0,*boxvalid = 0;

  //init data structure
  parameters *data = (parameters *) malloc(sizeof(parameters));

  //load the sudokuGrid
  load_sudokuGrid("puzzle.txt",data);

  solve(0,0,data);

	FILE *file = fopen("puzzle_solved.txt", "w");

  //print array
  for (int i = 0; i < 9; i++)
  {
      for (int j = 0; j < 9; j++)
      {
				fprintf(file, "%d ", data->sudokuGrid[i][j]);
        printf("%d ", data->sudokuGrid[i][j]);
      }
			fprintf(file, "\n");
      printf("\n");
  } 
									
	fclose(file);

  return 0;
}
