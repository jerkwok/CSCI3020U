/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
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
char **user_output;//[BUFFER_LEN];

// Define functions declared in myshell.h here

int main(int argc, char *argv[])
{

    // Input buffer and and commands
    char buffer[BUFFER_LEN] = { 0 };
    char arg[20][BUFFER_LEN] = { };
    char *pwdvar;
    char rmdvar[BUFFER_LEN] = { 0 };
    char startenv[BUFFER_LEN] = { "shell=" };

    //set environment variable
    pwdvar = getcwd(buffer, BUFFER_LEN);
    //display prompt
    printf("%s$ ",pwdvar);
    strcpy(rmdvar,pwdvar);
    strcat(pwdvar,"/myshell");
    strcat(startenv, pwdvar);
    putenv(startenv);

    //need to implement input and output
    //iterate through the arguments
    //if one is "<" then the next argument will replace stdin
    //if one is ">" then the next argument will replace stdout
    //if one is ">>" then the next arguement will replace stdout but will be appended to

    FILE *input_stream = stdin;
    FILE *output_stream = stdout;

    if (argc == 2)
      input_stream = fopen(argv[1],"r");

    for(int i = 0; i < argc; i++)
    {

      if(strcmp(argv[i], "<") == 0)
      {
        input_stream = fopen(argv[i+1], "r");
      }

      if(strcmp(argv[i], ">") == 0)
      {
        output_stream = fopen(argv[i+1], "w");
      }

      if(strcmp(argv[i], ">>") == 0)
      {
        output_stream = fopen(argv[i+1], "a");
      }
    }

    //commands from file I/O.
    // printf("%d\n",argc );

    // Parse the commands provided using argc and argv

    // Perform an infinite loop getting command input from users
    while (fgets(buffer, BUFFER_LEN, input_stream) != NULL)
    {
      // Perform string tokenization to get the command and argument
      buffer[strlen(buffer)-1] = 0;              //remove the newline from last char
      // printf("%s\n",buffer );
      //        tokenize(buffer,user_output," ");              //store all the strings delimited by a space into an array
      user_output = tokenize2(buffer, " ");

      //if we have an argument, set arg to that argument. If we dont set it to string 0.
      int counter = 1;
      strcpy(arg[1],"0");
      // printf("UO0: %s\n",user_output[0]);
      // printf("UO1: %s\n",user_output[1]);
      //copy the arguments into a array
      while(1)
      {
        if (user_output[counter] != NULL){
          strcpy(arg[counter],user_output[counter]);
          // printf("A%d: %s\n", counter, arg[counter]);
          counter++;
        }else{
          break;
        }
      }

      // printf("Buffer:%s\n",buffer );
      // printf("Arg:%s\n", arg[1]);
      // printf("User Output[1]: %s\n", user_output[1]);
      // printf("---------------\n");

      // Check the command and execute the operations for each command
      // cd command -- change the current directory
      if (strcmp(user_output[0], "cd") == 0)
      {
          // your code here

      // printf("Command:%s\n", command);
      // printf("Arg:%s\n", arg);
      // printf("User Output[1]: %s\n", user_output[1]);

        if (strcmp(arg[1],"0") != 0)
        {
          int ret;
          ret = chdir(arg[1]);
          if (ret == -1)
          {
            fprintf(output_stream, "No such directory %s\n",arg[1]);
          }
         }

          // system("pwd");
          pwdvar = getcwd(buffer, BUFFER_LEN);
          fprintf(stdout, "%s\n",pwdvar );
          // strcpy(pwdvar,"$ ");
          // printf("pwdvar:%s\n",pwdvar);
        }
        // Clears the terminal by pushing everything up off the screen
        else if (strcmp(user_output[0], "clr") == 0)
        {
          system("clear");
        }
        // Changes the directory to the directory specified as an arguement
        else if (strcmp(user_output[0], "dir") == 0)
        {
          // strcpy(user_output[0],"ls");
          if (strcmp(arg[1],"0") != 0){
            strcat(user_output[0], " ");
            strcat(user_output[0], arg[1]);
          }
          // printf("arg[1]:%s\n",arg[1] );
          system(user_output[0]);
        }
        // Displays all environment variables
        else if (strcmp(user_output[0], "environ") == 0)
        {
          system("printenv");
        }
        // Displays the argument passed in the shell
        else if (strcmp(user_output[0], "echo") == 0)
        {

          // for (int i = 0; i < sizeof(user_output)/sizeof(user_output[0]); i++) {
          //    char *pos = user_output[i];
          //    while (*pos != '\0') {
          //        printf("%c ", *(pos++));
          //    }
          // }

          counter = 1;
          // printf("arg1:%s\n",user_output[1]);
          // printf("arg2:%s\n",user_output[2]);
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
          // printf("%s\n", rmdvar );
          strcpy(startenv,"more ");
          strcat(startenv,rmdvar);
          strcat(startenv,"/README");
          // printf("%s\n",startenv );
          system(startenv);
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
        // else if (strcmp(command, "pwd") == 0)
        // {
        //     getcwd(pwdvar, BUFFER_LEN);
        //     printf("pwdvar:%s\n",pwdvar);
        // }
        // Unsupported command
        else
        {
          //Assume that any other command is an executable

          //set the parent environment variable
          char parent_env[BUFFER_LEN];
          strcpy(parent_env,"parent=");
          strcat(parent_env,getenv("PWD"));
          fprintf(output_stream, "%s\n",parent_env);
          putenv(parent_env);

          pid_t pid = fork();
          if(pid == 0){
            //child process
            fprintf(output_stream, "child process:\n");
            char exec_command[BUFFER_LEN];
            strcpy(exec_command,"/bin/");
            strcat(exec_command,user_output[0]);
            //      printf("%s\n",command);
            //printf("%s\n",arg[1]);
            fprintf(output_stream, "%s\n",exec_command);
            //      printf("%s\n",user_output[0]);
            //      printf("%s\n",user_output[1]);
            execl(exec_command,user_output[0],arg[1],(char *)0);
            //execv(user_output[0],(char *[])user_output);
          }else if(pid > 0){
            fprintf(output_stream, "parent process:\n");
            wait(NULL);//wait for child to terminate
            //parent process
          }else{
            fprintf(output_stream, "fork failed\n");
            fputs("Unsupported command, use help to display the manual\n", output_stream);
          }
        }
        //display prompt
      // strcat(pwdvar,"$ ");
      getcwd(pwdvar, BUFFER_LEN);
      fprintf(stdout, "%s$ ",pwdvar);
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

char** tokenize2(char *input, char *delim)
{
  //takes an input string with some delimiter and returns an array
  //with all the tokens split by the provided delimiter

  //Sample usage:
  //char buffer[] = "a b c";
  //char **user_output;
  //user_output = tokenize(buffer, " ");

  char** tokens = 0;
  size_t num_elements = 0;
  char* input_cpy = input;
  size_t tokens_index  = 0; //keep tracks of the tokens offset when adding them

  //iterate through the intput and count # of delims
  while (*input_cpy != NULL)
    {
      if (*delim == *input_cpy)
        {
    num_elements++;
        }
      input_cpy++;
    }

  num_elements++; //for last object
  num_elements++; //for null terminating value
  //  printf("%d\n",num_elements);

  //create enough memory for all the elements
  tokens = malloc(sizeof(char*) * num_elements);

  char* token = strtok(input, delim);
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
