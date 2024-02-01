# 🐚 TechShell

TechShell is a simple C program (techshell.c) that functions as a basic shell, allowing users to enter commands. The program parses the command line, checks for I/O redirection (using `<` for input from a file, `>` for output to a file), and then executes the command in a forked child process utilizing an exec call.

## Features:

- Command line parsing for I/O redirection
- Forks child processes for command execution

## Usage:

1. Clone down the repo
   ```bash
   git clone https://github.com/Joel-ajp/techshell
   ```
   ```bash
   cd techshell
   ```

2. Compile the program
   ```bash
   gcc techshell.c -o techshell
   ```

3. Run the compiled executable
   ```bash
   ./techshell
   ```

3. Enter commands at the prompt and observe I/O redirection functionality.

## Shell Prompt:

The program provides an informative shell prompt that displays the current working directory, followed by a `$` and ending with a space. The prompt is dynamically updated if the current working directory changes.

Feel free to explore and enhance the program according to your requirements!
