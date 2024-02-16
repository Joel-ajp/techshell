/*
* Name(s): Chase Dowdy, Joel Porter 
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
	// Grabs the home env variable
	char *homeD = getenv("HOME");
	if(homeD == NULL) {
		perror("getenc() error");
	}
	return homeD;

}

// Function to handle directory changes
void ChangeDirectory(struct ShellCommand command) {
	//If the argument is null Or if the argument is ~ change directory to user home
	if(command.args[1] == NULL || strcmp(command.args[1], "~") == 0) {
		char *homeD = HomeDirectory();
		if(chdir(homeD) != 0) {
			perror("chdir() error");
		}
	// Checks to see if there is only one argument
	} else if (command.args[2] == NULL) {
		if(chdir(command.args[1]) != 0) {
			perror("Error with chdir in the ChangeDirectory function");
		}
	// If is more than one argument, then returns this message
	} else {
		printf("techshell: cd: too many arguments\n");
	}
}

// Display current working directory and return user input
char* CommandPrompt(){ 
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

	// Removes trailling new line character
	input[strcspn(input, "\n")] = '\0';
	
	return input;
}

// Process the user input (Formatting into the ShellCommand struct)
struct ShellCommand ParseCommandLine(char *input){ 
	struct ShellCommand command;
	char *token = strtok(input, " ");
	int argc = 0;
	command.command = strdup(token);

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
				if (token == NULL) { break; }
				command.inputFile = strdup(token);
			} else if (strcmp(token, ">") == 0) {
				command.redirectOutput = 1;
				token = strtok(NULL, " ");
				if (token == NULL) { break; }
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

	// Checks to see if we entered a child process
	if(pid == 0) {

		//Redirect the input if it is needed (<)
		if(command.redirectInput) {
			FILE* infile = fopen(command.inputFile, "r");
			if (infile == NULL) {
				printf("techshell: %s: no such file or directory\n", command.inputFile);
				close(pid);
				return;
			}
			printf("HIT THIS");
			int status_code = dup2(fileno(infile), 0);
			if (status_code == -1) {
				printf("techshell: %s: error opening file\n", command.outputFile);
				close(pid);
				return;
			}
			// TODO read the contents of the file and passing them as an argument to command.command
			fclose(infile);
			close(pid);
		}

		//Redirect the output if it is needed (>)
		if(command.redirectOutput) {
			FILE* outfile = fopen(command.outputFile, "w");
			if (outfile == NULL) {
				printf("techshell: %s: no such file or directory\n", command.outputFile);
				close(pid);
				return;
			}
			int status_code = dup2(fileno(outfile), 1);
			if (status_code == -1) {
				printf("techshell: %s: error opening file\n", command.outputFile);
				close(pid);
				return;
			}
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

		// Checks to see if the input is empty, if so frees the memory and moves to the next interation of the loop
		if (*input == '\0') {
			free(input);
			continue;
		}
		else if (strcmp(input, "exit") == 0){
			free(input);
			break;
		}

		// parse the command line
		command = ParseCommandLine(input);

		// Checks for cd, then executes the command
		if (strcmp(command.command, "cd") == 0) {
			ChangeDirectory(command);
		} else {
			ExecuteCommand(command);
		}

		//free the allocated memory
		free(command.command);
		for (int i = 0; command.args[i] != NULL; i++) {
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