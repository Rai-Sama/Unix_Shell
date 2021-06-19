// Program to simulate a Unix Shell
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include<fcntl.h>

char sysCmdPath[200]="/bin /usr/bin";    // Initializing path
char *buildinCmd[]={"exit", "help", "cd", "path"};  //Global declaration for built-in functions 

int main(int argc, const char * argv[])    //argv[1] - filename for batch mode
{
    void ExecCommand(char **cmds, char *filename, bool redir);
    char **SplitCommand(char *line, char *delim);
    void ErrorMessage(char *errMsg);
    void CommandProcess(char *line);  

    if(argv[1] != NULL)
    {
        if(argv[2] != NULL)  // Invoked for more than one file
	{
            ErrorMessage("Invoke more than one file.");
            exit(1);
        }
        char const* const filename = argv[1];
        FILE *file = fopen(filename, "r");
        char line[256];
        if(file == NULL)
	{
            ErrorMessage("read file error!");
            exit(1);
        }
        while(fgets(line, sizeof(line), file))   //Loop to process each line of the command
            CommandProcess(line);
        fclose(file);
        exit(0);
    }

    // Loop for interactive mode
    while(1)
    {
        printf("Anshuman’s MiniProject> ");
        char *line = NULL;
        size_t len = 0;
        getline(&line, &len, stdin);   // Getting the command
        CommandProcess(line);        //Processing the command
    }
    return 0;
}   // End of main()


char **SplitCommand(char *line, char *delim)    // Splitting the command by the delimiter
{
    int bufSize = 100;
    char **cmds=malloc(bufSize*sizeof(char *));
    char *cmd=NULL;
    int i=0;
    cmd=strtok(line,delim);
    while(cmd!=NULL)    // Loop for commands seperated by delimiter
    {
        cmds[i]=cmd;
        i++;
        cmd=strtok(NULL,delim);
    }
    cmds[i]=NULL;
    return cmds;
}  // End of SplitCommand()

void ErrorMessage(char *errMsg)     //Displaying error message
{
    if(strcmp(errMsg, "")!=0)
        perror(errMsg);
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}  // End of ErrorMessage

void ExecCommand(char **cmds,char *filename,bool redir)      //Executing the built-in and system commands
{
    int cmdLen=sizeof(buildinCmd)/sizeof(buildinCmd[0]);
    int buildinCmdCode=-1;
    int i;
    for(i=0;i<cmdLen;i++)
    {
        if(strcasecmp(buildinCmd[i],cmds[0])==0)      // The command is a built-in command
        {
            buildinCmdCode = i;
            break;
        }
    }
    if(buildinCmdCode!=-1)    // Executing built-in commands
    {
        switch (buildinCmdCode)
	{
            case 0:
                if(cmds[1]!=NULL)
                    ErrorMessage("exit cannot have any arguments!");
                else
                    exit(0);
                break;

            case 1:   // help
                printf("%s","**** This is a help function ****\n");
                printf("%s","We provide below build in functions:\n");
                for(i=0;i<cmdLen;i++)
                    printf("%s ",buildinCmd[i]);
                printf("\n");
                break;
            case 2:   // cd
                if(cmds[1]==NULL)
                    ErrorMessage("CD cannot find any argument!");
                else 
		    if(cmds[2]!=NULL)
                    ErrorMessage("CD command can only take one argument!");
                else 
                    if(chdir(cmds[1])!=0)
                        ErrorMessage("error");

                break;

            case 3:   //path
                strcpy(sysCmdPath, "");
                int j=1;
                while(cmds[j]!=NULL)
		{
                    strcat(sysCmdPath, cmds[j]);
                    strcat(sysCmdPath," ");
                    j++;
                }
                break;
            default:
                break;
        }
    }
    else
    {   // System functions
        int status;
        pid_t pid = fork();
        if (pid == 0) 
	{    // Child process
            char **cmdPaths=SplitCommand(sysCmdPath, " \t\r\n\a");
            bool findPath=false;
            int i=0;
            char *cmdPath=NULL;
            while(cmdPaths[i]!=NULL)     // Command path exists
	    {      
                cmdPath=(char *)malloc(strlen(cmdPaths[i])+strlen(cmds[0])+1);
                strcpy(cmdPath, cmdPaths[i]);
                strcat(cmdPath, "/");
                strcat(cmdPath, cmds[0]);
                if(access(cmdPath, X_OK)==0)    // Checking for access and execv
		{ 
                    findPath=true;
                    if(!redir){    // Execute directly
                        if(execv(cmdPath, cmds)==-1){
                            ErrorMessage("Error");
                            exit(EXIT_FAILURE);
                        }
                    }
                    break;
                }
                free(cmdPath);
                i++;
            }
            if(redir)     // Output the result
	    {
                int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                dup2(fd, 1);
                dup2(fd, 2);
                close(fd);
                if(findPath)
                    execv(cmdPath,cmds);
            }
            if(!findPath)     // Command path not found
	    {
                ErrorMessage("Invalid Command! Please check your input or searching path!");
                exit(EXIT_FAILURE);
            }

        }
        else if (pid < 0)    // Error in forking
            ErrorMessage("Fork error");

        else        // Wait for child process to complete
	{
            do{
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }
}     // End of ExecCommand

void CommandProcess(char *line){
    char **redCmds=NULL;
    char **parall=NULL;
    char **cmds=NULL;
    parall=SplitCommand(line, "&");
    int i=0;
    while(parall[i]!=NULL)      // Run once if not parallel command
    {
        cmds=NULL;
        char *redErr=strstr(parall[i], ">>");    // Checking redirection
        if(redErr)
	{
            printf("%s","Error: Multiple redirection operators!\n");
            ErrorMessage("");
            break;
        }
        redCmds=SplitCommand(parall[i], ">");     // Checking redirection command
        if(redCmds[1]!=NULL && redCmds[2]!=NULL)
	{
            printf("%s","Error: Multiple redirection operators!\n");
            ErrorMessage("");
            break;
        }
        else if(redCmds[1]!=NULL)     // Redirection
	{
            char **filename=SplitCommand(redCmds[1], " \t\r\n\a");
            if(filename[1]!=NULL)
	    {
                printf("%s","Error: Multiple redirection files!\n");
                ErrorMessage("");
                break;
            }
            cmds=NULL;
            cmds=SplitCommand(redCmds[0]," \t\r\n\a");
            if(cmds[0]!=NULL){
                ExecCommand(cmds, filename[0], true);
            }
        }
        else    // Not redirection
	{       
            cmds=NULL;
            cmds=SplitCommand(redCmds[0]," \t\r\n\a");
            if(cmds[0]!=NULL)
                ExecCommand(cmds, NULL, false);
        }
        i++;
    }
}    // End of CommandProcess
