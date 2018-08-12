/**
 * Hadas Berger
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termio.h>
#include <signal.h>
#include <stdbool.h>

#define FAILURE -1
#define FAIL "Error in system call\n"
#define ERROR_SIZE 21
#define COMMAND "./draw.out"

/**
 * write error - in system call
 */
void checkError();
/**
 * read char from user without enter
 * @return char the user input
 */
char getch();

int main(){
    int childPID;
    int Pipe[2];
    /* Make our pipe */
    pipe(Pipe);

    /* Make a kid */
    if( (childPID = fork()) < 0 )
    {
        checkError();
    }
    /*child*/
    if( !childPID )
    {
        /* Force our stdin to be the read side of the pipe we made */
        dup2( Pipe[0], 0 );

        /* Execute our command */
        execlp( COMMAND, COMMAND, NULL );
        checkError();
    }
    /*father*/
    /*
     * Force our stdout to be the write end of the Pipe we formed.
     * Remeber dup2() make a *copy* of the file descriptor.
     */
    dup2( Pipe[1], 1 );

    char tav;
    while (1) {
        tav = getch();
        if (tav=='a' || tav=='d' ||tav=='w' ||tav=='s' ||tav=='q'){
            if(write(Pipe[1], &tav, 1) < 0) {
                checkError();
            }
            kill(childPID, SIGUSR2);
            if(tav=='q') {
                break;
            }
        }
    }
   // close( Pipe[0] );
    //close( Pipe[1] );
    return 0;
}

void checkError() {
    write(2, FAIL, ERROR_SIZE);
    exit(FAILURE);
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}


