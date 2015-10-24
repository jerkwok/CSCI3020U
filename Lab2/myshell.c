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
    char arg[20][BUFFER_LEN] = { 0 };
    char *pwdvar;
    char startenv[BUFFER_LEN] = { "shell=" };

    //set environment variable
    pwdvar= getenv("PWD");
    //display prompt
    printf("%s$ ",pwdvar);
    strcat(pwdvar,"/myshell");
    strcat(startenv, pwdvar);
    putenv(startenv);

    printf("%d\n",argc );
	FILE *stream;

    if (argc == 1){
    	stream = stdin;
    }else if (argc == 2){
    	stream = fopen(argv[1],"r");
    }

    // Parse the commands provided using argc and argv

    // Perform an infinite loop getting command input from users
    while (fgets(buffer, BUFFER_LEN, stream) != NULL)
    {
        // Perform string tokenization to get the command and argument
        buffer[strlen(buffer)-1] = 0;              //remove the newline from last char
        // printf("%s\n",buffer );
        tokenize(buffer,user_output," ");              //store all the strings delimited by a space into an array
        strcpy(command,user_output[0]);

        //if we have an argument, set arg to that argument. If we dont set it to string 0.
        int counter = 1;
        strcpy(arg[1],"0");
        // strcpy(arg[1],user_output[1]);
        printf("UO0: %s\n",user_output[0]);
        printf("UO1: %s\n",user_output[1]);
        //copy the arguments into a array
        while(1)
        {
        	if (user_output[counter] != NULL){
		        strcpy(arg[counter],user_output[counter]);
		        printf("A%d: %s\n", counter, arg[counter]);
		        counter++;
        	}else{
        		break;
        	}
        }

        printf("Buffer:%s\n",buffer );
        printf("Command:%s\n", command);
		printf("Arg:%s\n", arg[1]);
    	printf("User Output[1]: %s\n", user_output[1]);
    	printf("---------------\n");

        // Check the command and execute the operations for each command
        // cd command -- change the current directory
        if (strcmp(command, "cd") == 0)
        {
            // your code here

			// printf("Command:%s\n", command);
			// printf("Arg:%s\n", arg);
			// printf("User Output[1]: %s\n", user_output[1]);

        	if (strcmp(arg[1],"0") != 0)
        	{
        		int ret;
    		    // char directory[BUFFER_LEN] = { "/" };
	            // strcat(arg,directory);
	            // printf("%s\n",directory);
	            ret = chdir(arg[1]);
	            if (ret == -1)
	            {
	            	printf("No such directory %s\n",arg[1]);
	            }
			}
            system("pwd");
            pwdvar= getenv("PWD");
            // strcpy(pwdvar,"$ ");
            // printf("pwdvar:%s\n",pwdvar);
        }
        // Clears the terminal by pushing everything up off the screen
        else if (strcmp(command, "clr") == 0)
        {
          system("clear");
        }
        // Changes the directory to the directory specified as an arguement
        else if (strcmp(command, "dir") == 0)
        {	
        	strcpy(command,"ls");
        	if (strcmp(arg[1],"0") != 0){
	        	strcat(command, " ");
	        	strcat(command, arg[1]);
        	}
        	printf("Command:%s\n",command );
        	printf("arg[1]:%s\n",arg[1] );
	    	system(command);
        }
        // Displays all environment variables
        else if (strcmp(command, "environ") == 0)
        {
          system("printenv");
        }
        // Displays the argument passed in the shell
        else if (strcmp(command, "echo") == 0)
        {

			// for (int i = 0; i < sizeof(user_output)/sizeof(user_output[0]); i++) {
			// 		char *pos = user_output[i];
			// 		while (*pos != '\0') {
			// 				printf("%c ", *(pos++));
			// 		}
			// }
			counter = 1;
			printf("arg1:%s\n",user_output[1]);
			printf("arg2:%s\n",user_output[2]);
			while(1)
	        {
	        	if (user_output[counter] != NULL){
	        		printf("%s\n",user_output[counter]);
			        counter++;
	        	}else{
	        		break;
	        	}
	        }

        }
        // Displays the manual using more
        else if (strcmp(command, "help") == 0)
        {
          system("more README");
        }
        // Pauses the shell until enter is pressed
        else if (strcmp(command, "pause") == 0)
        {
          fgets(buffer, BUFFER_LEN, stdin);
        }
        // quit command -- exit the shell
        else if (strcmp(command, "quit") == 0)
        {
            return EXIT_SUCCESS;
        }
        else if (strcmp(command, "pwd") == 0)
        {
            // getcwd(pwdvar, BUFFER_LEN);
            // printf("pwdvar:%s\n",pwdvar);
        }
        // Unsupported command
        else
        {
            fputs("Unsupported command, use help to display the manual\n", stderr);
        }
        //display prompt
	    // strcat(pwdvar,"$ ");
	    getcwd(pwdvar, BUFFER_LEN);
	    printf("%s$ ",pwdvar);
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
