/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef MYSHELL_H_
#define MYSHELL_H_

// Include your relevant functions declarations here they must start with the 
// extern keyword such as in the following example:
// extern void display_help(void);

extern void tokenize (char *input, char **tokens, char *delim);
extern char** tokenize2(char *input, char *delim);
extern void trim(char padded_string[]);


#endif /* MYSHELL_H_ */
