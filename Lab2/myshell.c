/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, Elias Amal, Jeremy Kwok, Alexander Mihaylov, Taylor Smith
 * All rights reserved.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include "utility.h"
#include "myshell.h"

// Put macros or constants here using #define
#define BUFFER_LEN 256

// Put global environment variables here
char **user_output;

// Define functions declared in myshell.h here

int main(int argc, char *argv[])
{

    // Input buffer commands and other variables
    char buffer[BUFFER_LEN] = { 0 };
    char arg[20][BUFFER_LEN] = { };
    char *pwdvar;
    char rmdvar[BUFFER_LEN] = { 0 };
    char startenv[BUFFER_LEN] = { "shell=" };
    int inputcommands = 0;

    //set environment variable
    pwdvar = getcwd(buffer, BUFFER_LEN);

    //display prompt
    printf("%s$ ",pwdvar);

    //set the readme variable
    strcpy(rmdvar,pwdvar);
    strcat(rmdvar,"/README");

    //set environment variable
    strcat(pwdvar,"/myshell");
    strcat(startenv, pwdvar);
    putenv(startenv);

    //check for input output flags
    FILE *input_stream = stdin;
    FILE *output_stream = stdout;

    //if there are 2 args then the second one is read
    if (argc == 2){
      input_stream = fopen(argv[1],"r");
      inputcommands = 1;
    }

    //iterate through the arguments
    for(int i = 0; i < argc; i++)
    {
      //if one is "<" then the next argument will replace stdin
      if(strcmp(argv[i], "<") == 0)
      {
        input_stream = fopen(argv[i+1], "r");
      }
      //if one is ">" then the next argument will replace stdout
      if(strcmp(argv[i], ">") == 0)
      {
        output_stream = fopen(argv[i+1], "w");
      }
      //if one is ">>" then the next arguement will replace stdout but will be appended to
      if(strcmp(argv[i], ">>") == 0)
      {
        output_stream = fopen(argv[i+1], "a");
      }
    }

    // Parse the commands provided using argc and argv

    // Perform an infinite loop getting command input from users
    while (fgets(buffer, BUFFER_LEN, input_stream) != NULL)
    {
      // Perform string tokenization to get the command and argument
      buffer[strlen(buffer)-1] = 0;              //remove the newline from last char
      user_output = tokenize2(buffer, " ");

      //if we have an argument, set arg to that argument. If we dont set it to string 0.
      int counter = 1;
      strcpy(arg[1],"0");

      //copy the arguments into a array
      while(1)
      {
        if (user_output[counter] != NULL){
          strcpy(arg[counter],user_output[counter]);
          counter++;
        }else{
          break;
        }
      }

      // Check the command and execute the operations for each command
      // cd command -- change the current directory
      if (strcmp(user_output[0], "cd") == 0)
      {
        if (strcmp(arg[1],"0") != 0)
        {
          int ret;
          ret = chdir(arg[1]);
          if (ret == -1)
          {
            fprintf(output_stream, "No such directory %s\n",arg[1]);
          }
         }

          pwdvar = getcwd(buffer, BUFFER_LEN);
        }
        // Clears the terminal by pushing everything up off the screen
        else if (strcmp(user_output[0], "clr") == 0)
        {
          printf("\033[H\033[2J");
        }

        // Print the contents of the current directory.
        else if (strcmp(user_output[0], "dir") == 0)
        {

          DIR* directory;
          struct dirent * directory_pointer;

          // Open specified directory, if no director is specified opens the
          // curent directory.
          if(user_output[1] != NULL)
          {
            directory = opendir(user_output[1]);
          }
          else
          {
            directory = opendir(".");
          }

          // Make sure the current directory exists.
          if(directory != NULL)
          {
            // Read the next struct from the directory stream and if it is not
            // NULL it will print the name of that struct.
            while((directory_pointer = readdir(directory)) != NULL)
            {
              fprintf(output_stream, "%s \n", directory_pointer->d_name);
            }
          }
          else
          {
            fprintf(output_stream, "Error: Couldn't open specified directory\n");
          }
        }

        // Displays all environment variables
        else if (strcmp(user_output[0], "environ") == 0)
        {

          extern char **environ;

          int i = 0;

          while(environ[i] != NULL){
            printf("%s\n",environ[i] );
            i++;
          }
        }
        // Displays the argument passed in the shell
        else if (strcmp(user_output[0], "echo") == 0)
        {

          counter = 1;
          while(1)
          {
              if (user_output[counter] != NULL){
                fprintf(output_stream, "%s ",user_output[counter]);
                counter++;
              }else{
              break;
              }
          }
          printf("\n");
        }
        // Displays the manual using more
        else if (strcmp(user_output[0], "help") == 0)
        {
	  execlp("more","README", NULL);
	  /*            printf("%s\n", rmdvar);
            char currLine[256];

            FILE *fp;
            fp =fopen(rmdvar, "r");

            if (fp != NULL){
              while(fgets(currLine,256,fp) != NULL){
                printf("%s\n",currLine );
              }
              printf("\n");
            }

            fclose(fp); */
        }
        // Pauses the shell until enter is pressed
        else if (strcmp(user_output[0], "pause") == 0)
        {
          fgets(buffer, BUFFER_LEN, stdin);
        }
        // quit command -- exit the shell
        else if (strcmp(user_output[0], "quit") == 0)
        {
            return EXIT_SUCCESS;
        }

        else
        {
          //Assume that any other command is an executable

          //set the parent environment variable
          char parent_env[BUFFER_LEN];
          strcpy(parent_env,"parent=");
          strcat(parent_env,getenv("PWD"));
      	  strcat(parent_env,"/myshell");
          putenv(parent_env);

          pid_t pid = fork();
          if(pid == 0){

            execvp(user_output[0],user_output);
	           exit(0);
          }else if(pid > 0){
            wait(NULL);//wait for child to terminate  
            //parent process
          }else{
            fprintf(output_stream, "fork failed\n");
            fputs("Unsupported command, use help to display the manual\n", output_stream);
          }
        }
      getcwd(pwdvar, BUFFER_LEN);
      if (inputcommands == 0){
        fprintf(stdout, "%s$ ",pwdvar);
      }
    }
    return EXIT_SUCCESS;
}

void tokenize(char *input, char **tokens, char *delim){

  // const char delim[2] = " ";       //set delimiter
  char *token;                     //placeholder for token
  int i = 0;                       //keep track of array index
  char inputcopy[BUFFER_LEN];

  strcpy(inputcopy,input);
  token = strtok(inputcopy, delim);    //get the first token

  while( token != NULL ){          //walk through other tokens */
    trim(token);
    tokens[i] = token;             //store current token in array
    token = strtok(NULL, delim);   //read next token
    i++;
  }
  tokens[i] = NULL;                //to indicate where the tokens end
}

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

void trim(char padded_string[]){
  //Takes a string and removes any existing whitespace before or after the string
  //@param padded_string A string with whitespace before and/or after it
  char trimmed_string[strlen(padded_string)];  //The original string with the
  int j = 0;                                   //keeps track of the new trimmed string
  int a = 0;                                   //start of trimmed string
  int z = 0;                                   //end of trimmed string

  //find the index for the first nonwhitespace character
  for (int i = 0; i < strlen(padded_string); i++){
    if (padded_string[i] == ' '){
      a++;                                    //only increment a when a whitespace is found
    }else{
      break;                                  //exit for loop once we reach the actual string
    }
  }
  //a no contains the index for where the string actually begins

  //Find the index for the last ninwhitespace character
  for (int i = strlen(padded_string) - 1; i >=0; i--){
    if (padded_string[i] == ' '){
      z++;                                  //only increment z when a whitespace is found
    }else{
      break;                                //exit the for loop once the actual string is found
    }
  }
  //z no contains the number of whitespaces from right to left
  //get the index of where the actual string ends
  z = strlen(padded_string) - z;

  //Now that we have the indicies of where the actual string begins and ends we can copy it
  //copy only the string without whitespace
  //i is used to iterate through original string, j is used as index for the trimmed string
  for (int i = a; i < z; i++){
    trimmed_string[j] = padded_string[i];
    j++;
  }

  trimmed_string[j] = 0;                  //indicate where the string should end
  strcpy(padded_string,trimmed_string);   //copy the actual string into the trimmed one
}

