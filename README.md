# Unix_Shell
This is my mini project for the course Operating Systems (Sem-IV)

The project simulates a UNIX shell and is written in C

## Background
A Unix shell is a command-line interpreter (CLI) or shell that provides a command line user interface for Unix-like operating systems. The shell is both an interactive command language and a scripting language, and is used by the operating system to control the execution of the system using shell scripts. Users typically interact with a Unix shell using a terminal emulator; however, direct operation via serial hardware connections or Secure Shell are common for server systems. All Unix shells provide filename wildcarding, piping, command substitution, variables and control structures for condition-testing and iteration.

The shell is the heart of the command-line interface, and thus is central to the Unix/C programming environment. Mastering use of the shell is necessary to become proficient in this world; knowing how the shell itself is built is the focus of this mini project. 

## Problem Statement
The shell created here is very simple (conceptually): it runs in a while loop, repeatedly asking for input to tell it what command to execute. It then executes that command. The loop continues indefinitely, until the user types the built-in exit command. The shell can take as input arbitrarily long lines. Generally, the shell will be run in interactive mode, where the user types a command (one at a time) and the shell acts on it.

The shell also supports batch mode, in which it is given an input file of commands; in this case, the shell should not read user input but rather from this file to get the commands to execute.This goes on till the End Of File(EOF) marker is reached. If execv() is successful, it will not return anything; if it does return, there was an error (e.g., the command does not exist).

The aim of the Mini Project is to implement a command line interpreter (CLI) or, as it is more commonly known, a shell. The shell operates in this basic way: when you type in a command (in response to its prompt), the shell creates a child process that executes the command you entered and then prompts for more user input when it has finished. The shell implemented is similar to, but simpler than, the one we run every day in Unix.

## Flow
![korn0701](https://user-images.githubusercontent.com/48092867/122667030-4f5e9980-d1ce-11eb-83b9-f93f843569ec.gif)

## Working
### Paths
The user must specify a path variable to describe the set of directories to search for executables; the set of directories that comprise the path are sometimes called the search path of the shell. The path variable contains the list of all directories to search, in order, when the user types a command. The shell itself does not implement ls or other commands (except built-ins). All it does is find those executables in one of the directories specified by path and create a new process to run them. To check if a particular file exists in a directory and is executable, consider the access() system call. Most shells allow you to specify a binary specifically without using a search path, using either absolute paths or relative paths. For example, a user could type the absolute path /bin/ls and execute the ls binary without a search path being needed. A user could also specify a relative path which starts with the current working directory and specifies the executable directly.

### Built-in commands
Whenever the shell accepts a command, it checks whether the command is a built-in command or not. If it is, it not executed like other programs. Instead, the shell will invoke the implementation of the built-in command. For example, to implement the exit built-in command, it simply calls exit(0). In this project, the built-in commands are exit, cd, and path.
*	### exit
When the user types exit, the shell simply calls the exit system call with 0 as a parameter. It is an error to pass any arguments to exit. 
*	### cd
cd always takes one argument (0 or >1 args will be signaled as an error). To change directories, the chdir() system call is used with the argument supplied by the user; if chdir() fails, that is also an error. 
*	### path 
The path command takes 0 or more arguments, with each argument separated by whitespace from the others. A typical usage would be like this: Anshuman’s shell> path /bin /usr/bin, which would add /bin and /usr/bin to the search path of the shell. If the user sets path to be empty, then the shell will not be able to run any programs (except built-in commands). The path command always overwrites the old path with the newly specified path.

### Redirection
Many times, a shell user prefers to send the output of a program to a file rather than to the screen. Usually, a shell provides this nice feature with the > character. Formally this is named as redirection of standard output. This shell also includes this feature, but with the slight twist that nothing is printed on the screen when the redirection occurs. Instead, the standard output of the command is rerouted to the file output. In addition, the standard error output of the file is also rerouted to the file output. If the output file exists before the program is run, it is simply overwritten after truncating it. The exact format of redirection is a command  followed by the redirection symbol followed by a filename. Multiple redirection operators or multiple files to the right of the redirection sign are errors.

### Parallel commands
The shell also allows the user to launch parallel commands. This is accomplished with the ampersand operator as follows: 

Anshuman’s shell> cmd1 & cmd2 args1 args2 & cmd3 args1 

In this case, instead of running cmd1 and then waiting for it to finish, the shell runs cmd1, cmd2, and cmd3 (each with whatever arguments the user has passed to it) in parallel, before waiting for any of them to complete. Then, after starting all such processes, wait() (or waitpid) is used to wait for them to complete. After all processes are done, control is returned to the user as usual (or, if in batch mode, to the next line).

