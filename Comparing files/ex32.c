/*
 * hadas berger
 * 316590215
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/wait.h>

#define ERROR_SIZE 21
#define FAIL "Error in system call\n"
#define STUDENTS 160

typedef struct Student {
    char studentName[160];
    int grade;
    char reasonGrade[20];
} Student;

void parseConfigFile(char configInfo[3][160], char *path);
void checkError(int num);
bool isCFile(struct dirent *pDirent);
void writeToFileResults(Student *pStudent[160], int numOfStudent);
int HandleTheDir(struct dirent *pDirent, char *input, char *output, Student *pStudent,
             char pathTemp[160], char student[160],
             int i,char sourceStudent[256]);
void Compile(char *file, char *input, char *output, Student *pStudent);
void runProg(char *input, char *output, Student *pStudent);
void runTheCompFile(char *output, int out1, Student *pStudent);
void writeIntoTheStudentList(Student *pStudent, int grade, char *reasonGrade);

/**
 * the main program
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct dirent *dp;
    char info[3][160], *dir,*input,*output, pathStudent[160], tempPath[160];
    DIR *pDir;
    Student *st[STUDENTS];
    parseConfigFile(info, argv[1]);

    //take the info
    dir=info[0];
    input=info[1];
    output=info[2];
    strcpy(pathStudent,dir); //copy the path of the dir

    if((pDir = opendir(info[0]))==NULL){
        exit(1);
    }

    int i=0;
    while ((dp = readdir(pDir)) != NULL) {
        if (strcmp(dp->d_name, "..") == 0 || strcmp(dp->d_name, ".") == 0) {
            continue;
        }
        strcpy(tempPath, info[0]); //build recursive path
        strcat(tempPath, "/");
        strcat(tempPath, dp->d_name);

        st[i] = (Student *) malloc(sizeof(Student)); //create new student
        int thereIsCfile = HandleTheDir(dp,input,output,st[i],tempPath,pathStudent,i,dp->d_name); //recursive func
        if (thereIsCfile == 0) {
            strcpy(st[i]->studentName, dp->d_name);
            writeIntoTheStudentList(st[i], 0, "NO_C_FILE");
        }
        i++; //next student
    }
    writeToFileResults(st,i); //write all the data to csv file

   for (int j = 0; j < i; ++j) { //free the students
        free(st[j]);
    }
    unlink("a.out");
    unlink("outFile");
    closedir(pDir);
        return 0;

    }

/**
 * handle directory
 * @param pDirent
 * @param input
 * @param output
 * @param pStudent
 * @param pathTemp
 * @param student
 * @param i
 * @param sourceStudent
 * @return
 */
int HandleTheDir(struct dirent *pDirent, char *input,
             char *output, Student *pStudent, char pathTemp[160],
                 char student[160],int i,char sourceStudent[256]) {
    struct dirent *dp;
    DIR *pDir;
    char helper1[STUDENTS];
    if ((pDir = opendir(pathTemp)) == NULL) {
        exit(1);
    }

    while ((dp = readdir(pDir)) != NULL) {
        if (isCFile(dp)) {
            strcat(pathTemp, "/");
            strcat(pathTemp, dp->d_name);
            strcpy(pStudent->studentName,sourceStudent);
            Compile(pathTemp,input,output,pStudent); //run the c file we found
            closedir(pDir);
            return 1; //we find c file in the dir

        }else if(dp->d_type==DT_DIR) {
            if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0) {
                continue;
            }
            strcpy(helper1, pathTemp); //build recursive path
            strcat(helper1, "/");
            strcat(helper1, dp->d_name);
            int result = HandleTheDir(dp, input, output, pStudent, helper1, student, i, sourceStudent);
            if (result == 1) {
                closedir(pDir);
                return 1;
            } else {
                closedir(pDir);
                return 0; //if we dont find c file
            }
        }
    }
    closedir(pDir);
    return 0; //if we go over all the file and finish
}

/**
 * try to "gcc" the c file we found else compilation error
 * @param file = the c file to run
 * @param input = the input file
 * @param output = the output file to compare
 * @param pStudent = the student i check on
 */
void Compile(char *file, char *input, char *output, Student *pStudent) {
    char *lineArg[] = {"gcc", file, NULL};
    int suc1;
    pid_t val;
    val = fork();
    if (val == 0) { //child process
        suc1 = execvp("gcc", lineArg);
        checkError(suc1);
    }else if (val < 0) {    // Error forking
        checkError(val);
        return;
    } else if (val > 0) { //father process
        int status;
        waitpid(val, &status, 0);
        if(WEXITSTATUS(status) == 1) {
            writeIntoTheStudentList(pStudent, 0, "COMPILATION_ERROR");
            return;
        }
        runProg(input, output, pStudent);
    }
}

/**
 * chack if the file is c file according the extension
 * @param pDirent
 * @return true/false
 */
bool isCFile(struct dirent *pDirent) {
    const char *dot = strrchr(pDirent->d_name, '.');
    if (!dot || dot == pDirent->d_name) {
        return false;
    }
    if (strcmp((dot), ".c") == 0) {
        return true;
    }
    return false;
}

 /**
 * chech if the aloc work
 * @param num
 */
void checkError(int num) {
    if (num == -1 ){
        write(2, FAIL, ERROR_SIZE);
        exit(-1);
    }
}

/**
 * parse the path we get into 3 lines
 * @param configInfo- put in
 * @param path- the source txt
 * @param pDirent
 */
void parseConfigFile(char configInfo[3][160], char *path){
    int line = 0, len = 0;
    ssize_t buf;
    int fd = open(path, O_RDONLY);
    char tempChar[1];
    while ((buf = read(fd, tempChar, sizeof(tempChar))) != 0) {
            configInfo[line][len] = *tempChar;
            len++;
        while (tempChar[0] != '\n') {
            buf = read(fd, tempChar, sizeof(tempChar));
            configInfo[line][len] = *tempChar;
            len++;
        }
        configInfo[line][len - 1] = '\0';
        line++;
        len = 0;
    }

}

/**
 * try to run the prog with "a.out"
 * @param input
 * @param output
 * @param pStudent
 */
    void runProg(char *input, char *output, Student *pStudent) {
        int in, out1;
        char *lineArg1[] = {"./a.out",NULL};
        in=open(input,O_RDWR);  //the input file to get from
        out1 = open("outFile",  O_WRONLY | O_TRUNC |
                                O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);    //the output file to write in
        checkError(in);
        checkError(out1);

        if(dup2(in,0)==-1){ //input come from "in"
            write(2, FAIL, ERROR_SIZE);
            exit(-1);
        }
        if(dup2(out1,1)==-1){ //output write into "out1"
            write(2, FAIL, ERROR_SIZE);
            exit(-1);
        }
        int suc1;
        pid_t val;
        val = fork();
        if (val == 0) { //child process
            suc1 = execvp(lineArg1[0], lineArg1);
            checkError(suc1);
        }else if (val < 0) {    // Error forking
            fprintf(stderr, "Failed with fork\n");
            return;
        } else if (val > 0) { //father process
            sleep(5);
            pid_t pidProcess = waitpid(val, NULL, WNOHANG);
            if (!pidProcess) {
                writeIntoTheStudentList(pStudent, 0, "TIMEOUT");
                return;
            }
            runTheCompFile(output, out1, pStudent);
        }

    }

/**
 * try to run the comp file
 * @param output
 * @param out1
 * @param pStudent
 */
void runTheCompFile(char *output, int out1, Student *pStudent) {
    char *lineArg3[] = {"./comp.out", "outFile", output, NULL};
    int suc1;
    pid_t val;
    val = fork();
    if (val == 0) {         //child process
        suc1 = execvp(lineArg3[0], lineArg3);
        checkError(suc1);
    }else if (val < 0) {    // Error forking
        checkError(val);
        return;
    } else if (val > 0) { //father process
        int returned,status;
        waitpid(val, &status, 0);
        returned=WEXITSTATUS(status);
        switch (returned) {
            case 1:
                writeIntoTheStudentList(pStudent, 60, "BAD_OUTPUT");
                break;
            case 2:
                writeIntoTheStudentList(pStudent, 80, "SIMILAR_OUTPUT");
                break;
            case 3:
                writeIntoTheStudentList(pStudent, 100, "GREAT_JOB");
                break;
            default:
                break;
        }
    }
}

/**
 * write into the student list the name, grade and the reason
 * @param pStudent
 * @param grade
 * @param reasonGrade
 */
void writeIntoTheStudentList(Student *pStudent, int grade, char *reasonGrade) {
    pStudent->grade=grade;
    strcpy(pStudent->reasonGrade,reasonGrade);
}

/**
 * write into the "result" file all the data of the students
 * @param pStudent - specific student
 * @param numOfStudent - numner of students
 */
void writeToFileResults(Student *pStudent[160], int numOfStudent) {
    int results = open("results.csv", O_WRONLY | O_TRUNC |
                                      O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    checkError(results);

    dup2(results, 1); //change the output source

    for (int j = 0; j < numOfStudent; ++j)
        printf("%s,%d,%s\n", pStudent[j]->studentName, pStudent[j]->grade, pStudent[j]->reasonGrade);

    close(results);
}