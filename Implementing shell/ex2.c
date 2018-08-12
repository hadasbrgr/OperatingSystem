/**
 * hadas berger
 */
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUFFER 1024
#define FAIL "Error:The alloc failed.\n"
#define FAILSYS "The system failed.\n"
#define JOBS 50

struct job *arr;
typedef struct job{
    pid_t pid;
    char **Args;
}job;

char *readFromShell();
int changeDir(char *args[]);
char **splitLine(char *line);
int exitProg(struct job *arr, char **pString, int num);
int giveJobs(struct job *arr, char **pString, int num);
void printJob(struct job param);
void removeJob(struct job *arr, pid_t pid);
void addJob(struct job *arr, pid_t pid, char **pString);
void checkAlloc1(char **tokens);
void checkAlloc(char *tokens);
int execute(struct job *arr, char **pString);
int checkStatus(struct job *arr);
void loop();

/**
 * the main function.
 */
int main() {
    loop();
    return 0;
}

/**
 * the body of the main
 */
void loop() {
    struct job jobsArr[JOBS];
    for (int i = 0; i < JOBS; ++i) {
        jobsArr[i].pid=0;
        jobsArr[i].Args=NULL;
    }
    if (!jobsArr) {
        fprintf(stderr, FAIL);
        exit(0);
    }
    char *line, *helperLIne,**args;
    char **commandLineFree = (char **)calloc(JOBS,sizeof(char*));
    int status,i=0;

    do {
        printf("prompt>");
        line = readFromShell();
        helperLIne=line;
        commandLineFree[i] = line;
        args = splitLine(helperLIne);
        status = execute(jobsArr, args);
        i++;
          free(args);
    } while (status);
    for(int j=0;commandLineFree[j]!=NULL;j++)
        free(commandLineFree[j]);
    free(commandLineFree);
}

/**
 * peint the list of jobs
 * @param arr
 * @param pString
 * @param num
 * @return 1
 */
int giveJobs(struct job *arr, char **pString, int num) {
    if(arr==NULL)
        return 1;
    checkStatus(arr);
    for (int i = 0; i < JOBS; ++i) {
        if(arr[i].pid!=0 && arr[i].pid!= -1)
            printJob(arr[i]);
    }
    return 1;
}
/**
 * print specific job
 * @param param
 */
void printJob(struct job param) {
    int j=0;
    if (param.pid != -1) {
        printf("%d\t", param.pid);
        while (param.Args[j] != NULL) {
            printf("\t%s", param.Args[j]);
            printf("\t");
            j++;

        }
        printf("\n");
    }
}
/**
 * the method finish the program
 * @param arr
 * @param pString
 * @param num
 * @return 0 to change the main loop to exit
 */
int exitProg(struct job *arr, char **pString, int num) {
    printf("%d\n", getpid());
    for (int j = 0; j < JOBS; ++j) {
        if (arr[j].Args != NULL && arr[j].pid!=-1)
            free(arr[j].Args);
    }
    return 0;
}
/**
 * count the number of args in specific arr
 * @param arr
 * @return the number
 */
int counter(char **arr) {
    int i=0;
    for(; arr[i]!=NULL;i++);
    return i;
}
/**
 * check if the command include '&' and according this- create fork.
 * @param arr
 * @param pString
 * @return 1 if succeed else 0
 */
int executeJob(struct job arr[JOBS], char **pString) {
    int i=0,checkBack;
    for (; pString[i] != NULL; i++);
    if(strcmp(pString[i-1],"&"))
        checkBack=0;
    else
        checkBack=1;
    if(checkBack==1){
        pString[i-1]='\0';
    }
    pid_t pid= fork();
    if (pid == 0) {             // Child process
        execvp(pString[0], pString);
        fprintf(stderr, FAILSYS);
    }else {
        if (pid < 0) {          // Error forking
            fprintf(stderr, "Failed with fork\n");
            return 0;
        } else if (pid > 0) {   // father process
            printf("%d\n", pid);
            if(checkBack==1)
                addJob(arr, pid, pString);
        }
        if (checkBack == 0 && pid != 0) {
            waitpid(pid, NULL, 0);
        }
    }
    return 1;
}
/**
 * add jobs to the arr
 * @param arr
 * @param pid
 * @param pString
 */
void addJob(struct job arr[JOBS], pid_t pid, char **pString) {
    int size = 0;
    for (int i = 0; i < JOBS; ++i) {
        if (arr[i].pid == 0){
            arr[i].pid = pid;
            size = sizeof(pString[0])/sizeof(pString[0][0]);
            arr[i].Args = calloc(size, sizeof(char*));
            memcpy(arr[i].Args, pString, size *sizeof(char*));
            break;
        }
    }
}
/**
 * remove specific jobs after he finish is work
 * @param arr
 * @param pid
 */
void removeJob(struct job *arr, pid_t pid) {
    for(int i=0; i<JOBS; i++){
        if(arr[i].pid==pid){
            arr[i].pid=-1;
            free(arr[i].Args);
        }
    }
}
/**
 * check if the aloc succeed
 * @param tokens
 */
void checkAlloc1(char **tokens){
    if (!tokens) {
        fprintf(stderr, FAIL);
        exit(0);
    }
}
/**
 * split the line to args
 * @param line
 * @return arr of args
 */
char **splitLine(char *line) {
    char **strings = (char**)calloc(BUFFER, sizeof(char *));
    if (!strings) {
        printf(FAIL);
        exit(0);
    }
    int pos = 0, buffer_size = BUFFER;
    char *token = strtok(line, " ");
    while (token!= NULL) {
        strings[pos] = token;
        pos++;
        if (buffer_size<=pos) {
            buffer_size += BUFFER;
            strings = (char **) realloc(strings, buffer_size * sizeof(char));
            if (!strings) {
                printf(FAIL);
                exit(0);
            }
        }
        token = strtok(NULL, " ");
    }
    return strings;
}
/**
 * check if the aloc succeed
 * @param buffer
 */
void checkAlloc(char *buffer){
    if (!buffer) {
        free(buffer);
        fprintf(stderr, FAIL);
        exit(0);
    }
}
/**
 * read information from the shell
 * @return new line
 */
char *readFromShell() {
    int bufsize = BUFFER,place = 0,tav;
    char *buffer = malloc(sizeof(char) * bufsize);
    checkAlloc(buffer);

    while (1) {
        // Read a character
        tav = getchar();
        if (tav != '\n') {
            buffer[place] = tav;
            place++;
        }else {
            buffer[place] = '\0';
            return buffer;
        }

        if (bufsize <= place) { // If we have exceeded the buffer, reallocate.
            bufsize += BUFFER;
            buffer = realloc(buffer,bufsize);
            checkAlloc(buffer);
        }
    }
}
/**
 * play the shell according the command
 * @param arr
 * @param pString
 * @return status- 1 if succeed else 0
 */
int execute(struct job arr[JOBS], char **pString) {
    int jobsNum, i=0;
    for(; arr[i].Args!=NULL;i++);
    jobsNum= i;
    if(pString==NULL || pString[0]==NULL)
        return 1;
    else if(strcmp(pString[0],"cd")==0)
        return changeDir(pString);
    else if(strcmp(pString[0],"jobs")==0)
        return giveJobs(arr,pString,jobsNum);
    else if(strcmp(pString[0],"exit")==0)
        return exitProg(arr,pString,jobsNum);
    else
        return executeJob(arr,pString);

}

/**
 * the cd method.
 * @return 1.
 */
int changeDir(char *args[]){
    printf("%d\n", getpid());
    if(args[1]!=NULL){
        if(chdir(args[1])== (-1)){
            printf("%s: No such directory\n",args[1]);
            return -1;
        }
    }else{
        chdir(getenv("HOME"));
        return 1;
    }
    return 1;
}
/**
 * check the status of specific job and react
 * @param arr
 * @return
 */
int checkStatus(struct job arr[JOBS]){
    int status;

    for (int i = 0; arr[i].Args!=NULL ; ++i) {
        if((waitpid(arr[i].pid,&status,WNOHANG)!=0)) //child is finish.exit status
            removeJob(arr,arr[i].pid);
    }
}





