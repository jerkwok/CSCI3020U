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
char *user_output[BUFFER_LEN];

// Define functions declared in myshell.h here

int main(int argc, char *argv[])
{
    // Input buffer and and commands
    char buffer[BUFFER_LEN] = { 0 };
    char command[BUFFER_LEN] = { 0 };
    char arg[BUFFER_LEN] = { 0 };

    // Parse the commands provided using argc and argv

    //display prompt
    printf(">");
    // Perform an infinite loop getting command input from users
    while (fgets(buffer, BUFFER_LEN, stdin) != NULL)
    {
        //display prompt
        printf(">");
        // Perform string tokenization to get the command and argument
        buffer[strlen(buffer)-1] = 0;              //remove the newline from last char
        printf("%s\n",buffer );
        tokenize(buffer,user_output," ");              //store all the strings delimited by a space into an array
        strcpy(command,user_output[0]);
        printf("%s\n",buffer );

        // Check the command and execute the operations for each command
        // cd command -- change the current directory
        if (strcmp(command, "cd") == 0)
        {
            // your code here
            char directory[BUFFER_LEN];
            // strcpy(directory,user_output[1]);
            strcpy(directory,"..");

            execl("cd", directory);
            printf("dir:%s\n",directory );
            system("ls -al");
            system("pwd");
        }
        // Check the command and execute the operations for each command
        // cd command -- change the current directory
        if (strcmp(command, "cd") == 0)
        {
            // your code here
        }
        // Clears the terminal by pushing everything up off the screen
        else if (strcmp(command, "clr") == 0)
        {

        }
        // Changes the directory to the directory specified as an arguement
        else if (strcmp(command, "dir") == 0)
        {

        }
        // Displays all environment variables
        else if (strcmp(command, "environ") == 0)
        {

        }
        // Displays the argument passed in the shell
        else if (strcmp(command, "echo") == 0)
        {

        }
        // Displays the manual using more
        else if (strcmp(command, "help") == 0)
        {

        }
        // Pauses the shell until enter is pressed
        else if (strcmp(command, "pause") == 0)
        {

        }
        // quit command -- exit the shell
        else if (strcmp(command, "quit") == 0)
        {
            return EXIT_SUCCESS;
        }

        // Unsupported command
        else
        {
            fputs("Unsupported command, use help to display the manual\n", stderr);
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
