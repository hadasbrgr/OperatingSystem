/**
 * hadas berger
 * 316590215
 */
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#define FAIL "‫‪Error‬‬ ‫‪in‬‬ ‫‪system‬‬ ‫‪call.‬‬\n"
#define SPACE ' '
#define NEW_LINE '\n'
#define ERROR_SIZE 25

void checkError(int num);
char changeToLower(unsigned char ch);
int checkCon(int ch2, char c1, char c2,int file);

int moreCheck(char c1, char c2, int ch1, int ch2, int file1, int file2);

int main(int argc, char* argv[]) {
    int flag = 0, ch1, ch2;
    int file1 = open(argv[1],O_RDONLY);
    checkError(file1);
    int file2 = open(argv[2],O_RDONLY);
    checkError(file2);

    char c1,c2;
    ch1= read(file1,&c1,1);
    checkError(ch1);

    ch2 = read(file2,&c2,1);
    checkError(ch2);

    if(ch1==0 && ch2==0)
        return 3; //if the files empty

    while ((ch1 != 0) && (ch2 != 0) && (c1 == c2)) {    //if the same length and equals chars
        ch1= read(file1,&c1,1);
        checkError(ch1);
        ch2 = read(file2,&c2,1);
        checkError(ch2);
    }

    if (c1 == c2 && ch1==0 && ch2==0) { //the files are exactly the same
        printf("good\n");
        close(file1);
        close(file2);
        return 3;
    }else{
        return moreCheck(c1,c2,ch1,ch2,file1,file2);
    }
}

int moreCheck(char c1, char c2, int ch1, int ch2, int file1, int file2) {
    int flag=0;
    while ((ch1 != 0) || (ch2 != 0)) {  //if it is not the end of the file
        if (c1 == SPACE || c1 == NEW_LINE || ch1==0) {  //if the char is \n or " " - skip over
            if(ch1!=0) {
                ch1 = read(file1, &c1, 1);
                checkError(ch1);
                continue;
            }
            else if(ch2!= SPACE || ch2!= NEW_LINE)
                flag=checkCon(ch2,c1,c2,file1);
        }
        if (c2 == SPACE || c2 == NEW_LINE || ch2==0) {
            if (ch2 != 0) {
                ch2 = read(file2, &c2, 1);
                checkError(ch2);
                continue;
            }else if(ch1!= SPACE || ch1!= NEW_LINE) {
                flag=checkCon(ch1,c2,c1,file2);

            }
        }
        if ((changeToLower(c1) != changeToLower(c2))){     //if the chars not the same
            flag=1;
            break;
        }
        ch1 = read(file1, &c1, 1);
        checkError(ch1);
        ch2 = read(file2, &c2, 1);
        checkError(ch2);

    }
    close(file1);
    close(file2);

    if (flag==0) {
        printf("almost the same file\n");
        return 2;
    }
    if(flag==1) {
        printf("not good!\n");
        return 1;
    }
}


/**
 * the method decide what to do if one tf the file end- check space and "\ n"
 * @param ch2
 * @param c1
 * @param c2
 * @param file
 * @return same or not
 */
int checkCon(int ch2, char c1, char c2, int file) {
    if(ch2!=0){
        while(ch2!=0){
            if(c2==SPACE || c2==NEW_LINE) {
                ch2 = read(file, &c1, 1);
            }else {
                return 1;
            }
        }
        return 0;
    }
}

/**
 * the method get char and change to lower char
 * @param ch
 * @return the new char
 */
char changeToLower(unsigned char ch) {
    if (ch <= 'Z' && ch >= 'A')
        ch = 'a' + (ch - 'A');
    return ch;
}
/**
 * chech if the aloc work
 * @param num
 */
void checkError(int num){
    if (num==-1) {
        write(2,FAIL,ERROR_SIZE);
        exit(-1);
    }
}


