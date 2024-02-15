/*
* Name(s):Chase Dowdy, Joel Porter 
* Date: 
* Description: **Include what you were and were not able to handle!**
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

//added libaries 
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_INPUT_SIZE 1024

// Command struct 
struct ShellCommand{
	char * command;
	char *args[MAX_INPUT_SIZE / 2 + 1];
	int redirectInput;
	int redirectOutput;
	char *inputFile;
	char *outputFile;
};

//Function to get the home directory
char *HomeDirectory() {
	//this is where getenv is supposed to be used to be able to get back to home
	char *homeD = getenv("HOME");
	if(homeD == NULL) {
		perror("getenc() error");
	}
	return homeD;
}
//function that is supposed to handle cd command but I am having trouble getting it 
//to change to subdirectories after going to the home directory
void ChangeDirectory(struct ShellCommand command) {
	//If the argument is null Or if the argument is ~ cd to the home directory	
	if(command.args[0] == NULL || strcmp(command.args[0], "~") == 0) {
		char *homeD = HomeDirectory();
		if(chdir(homeD) != 0) {
			perror("chdir() error");
		}
	// Checks to see if there is only one argument
	} else if (command.args[1] == NULL) {
		if(chdir(command.args[0]) != 0) {
			perror("Error with chdir in the ChangeDirectory function");
		}
	// If is more than one argument, then returns this message
	} else {
		printf("techshell: cd: too many arguments\n");
	}
}

char* CommandPrompt(){ // Display current working directory and return user input
	char cwd[MAX_INPUT_SIZE];
	if(getcwd(cwd, sizeof(cwd)) != NULL){
		printf("%s$ ", cwd);
	}else {
		perror("getcwd() error");
	}
	char* input = (char*)malloc(MAX_INPUT_SIZE);
	if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
		perror("fgets() error");
		exit(EXIT_FAILURE);
	}

	//This removes the trailing newline characters
	input[strcspn(input, "\n")] = '\0';
	
	return input;
}

struct ShellCommand ParseCommandLine(char *input){ // Process the user input (As a shell command)
	struct ShellCommand command;
	char *token = strtok(input, " ");
	command.command = strdup(token);
	int argc = 0;

	//Initialize redirection flags and file names
	command.redirectInput = 0;
	command.redirectOutput = 0;
	command.inputFile = NULL;
	command.outputFile = NULL;
	command.args[0] = token;

	while (token != NULL) {
		if(token != NULL) {
			if(strcmp(token, "<") == 0) {
				command.redirectInput = 1;
				token = strtok(NULL, " ");
				command.inputFile = strdup(token);
			} else if (strcmp(token, ">") == 0) {
				command.redirectOutput = 1;
				token = strtok(NULL, " ");
				command.outputFile = strdup(token);
			} else {
				command.args[argc] = strdup(token);
				argc++;
			}
		} else {
			command.args[argc] = NULL;
			return command;
		}
		token = strtok(NULL, " ");
	}

	command.args[argc] = NULL;
	return command;
}

void ExecuteCommand(struct ShellCommand command){ //Execute a shell command
	pid_t pid = fork();

	if(pid == -1) {
		perror("fork() error");
		exit(EXIT_FAILURE);
	}

	if(pid == 0) {//This is the child process 
		//Redirect the input if it is needed
		if(command.redirectInput) {
			int input_fd = open(command.inputFile, O_RDONLY);
			FILE* infile = fopen(command.inputFile, "r");
			dup2(fileno(infile), 0);
			fclose(infile);
			close(pid);
		}

		//Redirect the output if it is needed
		if(command.redirectOutput) {
			int output_fd = open(command.outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			FILE* outfile = fopen(command.outputFile, "w");
			dup2(fileno(outfile), 1);
			fclose(outfile);
			close(pid);
		}

		//excute the given command with its args. 
		execvp(command.command, command.args);
		close(pid);
		perror("execvp() error");
		exit(EXIT_FAILURE);
	} else {// This is the parent process
		wait(NULL);
	}
}
int main() {
    // repeatedly prompt the user for input
    for (;;) {
		char* input;
		struct ShellCommand command;
		input = CommandPrompt();
		if (strcmp(input, "exit") == 0){
			free(input);
			break;
		}

		// parse the command line
		command = ParseCommandLine(input);

		// execute the command and checks for cd 
		if (strcmp(command.command, "cd") == 0) {
			ChangeDirectory(command);
		} else {
			ExecuteCommand(command);
		}

		//free the allocated memory
		free(command.command);
		for (int i =0; command.args[i] != NULL; i++) {
			free(command.args[i]);
		}
		if(command.redirectInput) {
			free(command.inputFile);
		}
		if(command.redirectOutput) {
			free(command.outputFile);
		}
		free(input);
    }	
    exit(0);
}
