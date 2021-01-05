/*
Title: smallsh
Author: Gabrielle Josephson
Class: CS 344 Operating Systems Fall 2020
Date Due: 11/3/2020
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// Foreground mode is set to off by default.
int FOREGROUND_MODE = 0;

// Struct to hold components of user-inputted command line
struct command {
	char* mainCommand;
	char** commandArgs;
	char* inputFile;
	char* outputFile;
	int background;
};

/*
Initializes a command struct. Memory is allocated for the struct itself, then the array
of pointers (commandArgs), and lastly the inputFile and outputFile strings.
@param: none
@returns: struct command aLine
*/
struct command* commandInit() {
	struct command* aLine = malloc(sizeof(struct command));
	aLine->mainCommand = NULL;
	aLine->commandArgs = malloc(511 * sizeof(char*));

	for (int j = 0; j < 511; j++) {
		aLine->commandArgs[j] = NULL;	// Initialize each array index
	}
	aLine->inputFile = NULL;
	aLine->outputFile = NULL;

	return aLine;
}

/*
Deallocates memory of command struct members. Each of the string
members are freed if they are not already NULL. Then, each string
member is pointed to NULL.
@param: deleteMe
@returns: none
*/
void deleteLine(struct command* deleteMe) {
	int i = 0;

	free(deleteMe->mainCommand);
	deleteMe->mainCommand = NULL;

	while (deleteMe->commandArgs[i] != 0) {
		free(deleteMe->commandArgs[i]);		// Free each array index
		deleteMe->commandArgs[i] = NULL;
		i++;
	}


	if (deleteMe->inputFile != NULL) {
		free(deleteMe->inputFile);
		deleteMe->inputFile = NULL;
	}

	if (deleteMe->outputFile != NULL) {
		free(deleteMe->outputFile);
		deleteMe->outputFile = NULL;
	}

	deleteMe = NULL;
}

/*
Custom signal handler for SIGTSTP. Switches program on and off between "foreground-only mode"
@param: int signo
@returns: none

Source: OSU CS 344 Fall 2020 Exploration: Signal Handling API
*/
void handle_SIGTSTP(int signo) {
	// If foreground-only mode is turned off, turn it on
	if (FOREGROUND_MODE == 0) {
		char* message = "\nEntering foreground-only mode (& is now ignored)\n:";
		write(STDOUT_FILENO, message, 51);
		FOREGROUND_MODE = 1;
	}
	// If foreground only mode is turned on, turn it off
	else {
		char* message = "\nExiting foreground-only mode\n:";
		write(STDOUT_FILENO, message, 31);
		FOREGROUND_MODE = 0;
	}

}

/*
Calculates and returns the number of digits in a given integer.
@param: num
@return: num
Source: https://codeforwin.org/2016/10/c-program-to-count-number-of-digits-in-number.html
*/
int _countDigits(int num) {
	int count = 0;

	// The number of digits is equal to the number of times an integer can be divided by 10
	do {
		count++;
		num /= 10;
	} while (num != 0);

	return count;
}

/*
Detects instances of the substring "$$" and replaces each instance with the PID of the shell.
@param: char* commandArg
@return: char* newStr
*/
char* detectPid(char* commandArg) {
	char *sub = NULL;
	char* newStr;
	int pid;
	int len1 = 2;
	int len2;

	// Allocate a new string for the expanded argument
	newStr = calloc(strlen(commandArg) + 1, sizeof(char));
	strcpy(newStr, commandArg);

	sub = strstr(newStr, "$$");		// Detect of argument contains substring "$$"

	if (sub != NULL) {		// If an instance is detected, get the PID
		pid = getpid();
		const int dig = _countDigits(pid);
		char pidString[dig];
		sprintf(pidString, "%d", pid);

		while (sub[0] != '\0') {	// Continue until there are no more instances of "$$"

			len2 = dig;

			if (len1 != len2) {
				memmove(sub + len2, sub + len1, strlen(sub + len1) + 1);	// Make room for the PID
			}
			memcpy(sub, pidString, len2);		// Replace "$$" with PID
			sub+=len2;
		}
	}

	return newStr;
}

/*
Parses user-inputted command line and places each component into the appropriate command
struct member.
@param: struct command* userCommand
@return: struct command* userCommand
*/
struct command* parseCommand(struct command* userCommand) {
	char orgString[2048];
	char* saveptr = NULL;
	int i;
	char* str = NULL;

	if (userCommand->mainCommand != NULL) {
		deleteLine(userCommand);
	}

	fgets(orgString, sizeof orgString, stdin);		// Get line of input
	fflush(stdout);

	if (orgString[0] != '\n') {		// Parse the string only if the user didn't hit enter without inputting something

		userCommand->background = 0;	// Background process is initially set to 'false'

		char* token = strtok_r(orgString, " \n\r", &saveptr);

		str = detectPid(token);

		// Place first argument into mainCommand

		userCommand->mainCommand = calloc(strlen(str) + 1, sizeof(char));
		strcpy(userCommand->mainCommand, str);

		free(str);
		str = NULL;

		token = strtok_r(NULL, " \n\r", &saveptr);		// Move on to next argument

		i = 0;
		while (token != NULL) {
			if (strcmp(token, "<") == 0) {		// If input redirection is detected, skip the current argument and place the next argument into inputFile
				token = strtok_r(NULL, " \n\r", &saveptr);

				str = detectPid(token);

				userCommand->inputFile = calloc(strlen(str) + 1, sizeof(char));
				strcpy(userCommand->inputFile, str);
			}
			else if (strcmp(token, ">") == 0) {		// If output redirection is detected, skip the current argument and place the next arugment into outputFile
				token = strtok_r(NULL, " \n\r", &saveptr);

				str = detectPid(token);

				userCommand->outputFile = calloc(strlen(str) + 1, sizeof(char));
				strcpy(userCommand->outputFile, str);
			}
			else if (strcmp(token, "&") == 0) {
				if (saveptr[0] == '\0') {				// If the ampersand is at the end of the line, set background process to true
					if (FOREGROUND_MODE == 0) {			// If foreground mode is turned off, let command run in the background. Otherwise, ignore argument
						userCommand->background = 1;
					}
				}
				else {								// If ampersand is not at the end of the line, add '&' to array of command arguments
					userCommand->commandArgs[i] = calloc(strlen(token) + 1, sizeof(char));
					strcpy(userCommand->commandArgs[i], token);
					i++;
				}
			}
			else {	// Place all other command arguments into commandArgs array
				str = detectPid(token);
				
				userCommand->commandArgs[i] = calloc(strlen(str) + 1, sizeof(char));
				strcpy(userCommand->commandArgs[i], str);
				i++;
			}

			free(str);
			str = NULL;

			token = strtok_r(NULL, " \n\r", &saveptr);
		}
	}
	else {														// If user did not enter in command line arguments and simply hit 'return', copy '\n' into mainCommand
		userCommand->mainCommand = calloc(2, sizeof(char));
		strcpy(userCommand->mainCommand, "\n");
	}

	return userCommand;
}

/*
Changes directory the shell is working in according to user input. If no path is specified,
directory is changed to home directory.
@param: char* filePath
@return: none
*/
void cd(char* filePath) {
	char directory[2048];
	getcwd(directory, sizeof(directory));

	if (filePath == NULL) {
		chdir(getenv("HOME"));
	}
	else if (strcmp(filePath, ".") == 0 || strcmp(filePath, "..") == 0){
		strcat(directory, "/");
		strcat(directory, filePath);
		
		chdir(directory);
	}
	else {
		chdir(filePath);
	}
}

/*
Prints the exit or termination status of the last child process to the terminal.
@param: int childStatus
@return: none

Source: OSU CS 344 Operating Systems Fall 2020 Exploration: Process API - Monitoring a Child Process
*/
void status(int childStatus) {
	// If child process exited normally, check and print exit status
	if (WIFEXITED(childStatus) == 1) {
		printf("exit value %d\n", WEXITSTATUS(childStatus));
	}
	// If child process ended abnormally, check and print terminating signal
	else {
		printf("terminated by signal %d\n", WTERMSIG(childStatus));
	}
}

/*
Executes any command that is not built into the shell as a child process. Redirects input/output
and executes command as a background process as specified by the user. If the command is not recognized,
the child process exits with an error code of 2. Otherwise, function returns status of child process.
@param: struct command* userCommand
@returns: int childStatus

Source: OSU CS 344 Operating Systems Fall 2020 Exploration: Executing a Program
Source: https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/dup2.html
Source: http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html
*/
int otherCommand(struct command* userCommand) {
	int childStatus;
	char* newCommand[511];		// Array to pass to execvp
	int i;
	int out;
	int in;

	for (i = 0; i < 511; i++) {	// Initialize array
		newCommand[i] = NULL;
	}

	// Place mainCommand into first index of new array
	newCommand[0] = calloc(strlen(userCommand->mainCommand) + 1, sizeof(char));
	strcpy(newCommand[0], userCommand->mainCommand);

	// Places each index of commandArgs[] into subsequent indices of new array
	i = 0;
	while (userCommand->commandArgs[i] != 0) {
		newCommand[i + 1] = calloc(strlen(userCommand->commandArgs[i]) + 1, sizeof(char));
		strcpy(newCommand[i + 1], userCommand->commandArgs[i]);
		i++;
	}

	pid_t spawnPid = fork();		// Create a child process

	switch (spawnPid)
	{
	case -1:
		perror("fork() failed!");
		exit(1);
		break;
	case 0:
		//Allow default SIGINT action when process is running in the foreground
		if (userCommand->background == 0) {
			struct sigaction ignore_action = { { 0 } };
			ignore_action.sa_handler = SIG_DFL;
			sigfillset(&ignore_action.sa_mask);
			ignore_action.sa_flags = SA_RESTART;
			sigaction(SIGINT, &ignore_action, NULL);
		}

		//Ignore SIGTSTP regardless of whether child is running in the background or foreground
		struct sigaction ignore_stop = { { 0 } };
		ignore_stop.sa_handler = SIG_IGN;
		sigfillset(&ignore_stop.sa_mask);
		ignore_stop.sa_flags = SA_RESTART;
		sigaction(SIGTSTP, &ignore_stop, NULL);

		if (userCommand->inputFile != NULL) {		// Redirect input
			in = open(userCommand->inputFile, O_RDONLY);
			if (in < 0) {
				printf("cannot open %s for input\n", userCommand->inputFile);
				exit(1);
			}
			dup2(in, 0);
		}
		else if (userCommand->background == 1 && FOREGROUND_MODE == 0) {	// Default for background process when input file isn't specified
			in = open("/dev/null", O_RDONLY);
			if (in < 0) {
				printf("cannot open /dev/null for input\n");
			}
			dup2(in, 0);
		}

		if (userCommand->outputFile != NULL) {		// Redirect output
			out = open(userCommand->outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0644);
			dup2(out, 1);
		}
		else if (userCommand->background == 1 && FOREGROUND_MODE == 0) {		// Default for background process when output file isn't specified
			out = open("/dev/null", O_CREAT | O_TRUNC | O_WRONLY, 0644);
			dup2(out, 1);
		}

		execvp(userCommand->mainCommand, newCommand);	// Command and arguments passed as an array to execvp

		exit(2);	// If command doesn't exist, child process exits in error state, but shell just prompts for another command
		break;
	default:
		if (userCommand->background == 0) {
			spawnPid = waitpid(spawnPid, &childStatus, 0);		// Parent process waits for child process to finish
			if (WIFSIGNALED(childStatus) == 1) {				// Print terminating signal for foreground process that ended abnormally
				printf("terminated by signal %d\n", WTERMSIG(childStatus));
			}
			else if (WEXITSTATUS(childStatus) == 2) {			// Print error message for command that doesn't exit
				printf("%s: no such file or directory\n", userCommand->mainCommand);
			}
		}
		else {
			printf("background pid is %d\n", spawnPid);
			spawnPid = waitpid(spawnPid, &childStatus, WNOHANG);
		}
		
		// Deallocate newCommand indicies
		i = 0;
		while (newCommand[i] != NULL) {
			free(newCommand[i]);
			newCommand[i] = NULL;
		}
		return childStatus;
		break;
	}
}

/*
Checks if any background processes have finished. Prints their exit value or terminal signal to the console.
@param: none
@returns: none

Source: OSU CS 344 Operating Systems Fall 2020 Exploration: Monitoring Child Processes
*/
void checkBackground() {
	int status;
	pid_t background;
	
	do {
		background = waitpid(-1, &status, WNOHANG);		// Detect a background process that has finished
		if (background > 0) {
			if (WIFSIGNALED(status) == 1) {		// If it exited abnormally, print terminating signal
				printf("background process %d is done: terminated by signal %d\n", background, WTERMSIG(status));
			}
			else {		// If it exited normall, print exit status
				printf("background process %d is done: exit value %d\n", background, WEXITSTATUS(status));
			}
		}
	} while (background > 0);
}


int main() {
	int prevStatus = -5;
	struct sigaction ignore_action = { { 0 } };

	// Ignore signal handler for SIGINT (^C)
	ignore_action.sa_handler = SIG_IGN;
	sigfillset(&ignore_action.sa_mask);
	ignore_action.sa_flags = SA_RESTART;
	sigaction(SIGINT, &ignore_action, NULL);

	// Custom signal handler for SIGTSTP (^Z)
	struct sigaction enter_foreground = { { 0 } };
	enter_foreground.sa_handler = handle_SIGTSTP;
	sigfillset(&enter_foreground.sa_mask);
	enter_foreground.sa_flags = SA_RESTART;
	sigaction(SIGTSTP, &enter_foreground, NULL);

	struct command* currLine = commandInit();
	

	do {

		checkBackground();		// Check for any background processes that have finished

		printf(":");
		fflush(stdout);

		currLine = parseCommand(currLine);
		
		if (strcmp(currLine->mainCommand, "cd") == 0) {		// Built-in cd command

			cd(currLine->commandArgs[0]);

		}
		else if (strcmp(currLine->mainCommand, "status") == 0) {	// Built-in status command
			status(prevStatus);
		}
		else if (strcmp(currLine->mainCommand, "\n") == 0) {	// No input

			// If user hits "enter", prompt for next command

		}
		else if (currLine->mainCommand[0] == '#') {		// Comment line

			// If user enters a comment (line beginning with '#'), prompt for next command

		}
		
		else if (strcmp(currLine->mainCommand, "exit") != 0) {		// If user enters command that isn't built in, process it and pass it to execvp
			prevStatus = otherCommand(currLine);
		}

			
	} while (strcmp(currLine->mainCommand, "exit") != 0);

	
	deleteLine(currLine);
	free(currLine);
	currLine = NULL;

	kill(0, SIGKILL);	// Kill any remaining background processes


	exit(EXIT_SUCCESS);
}