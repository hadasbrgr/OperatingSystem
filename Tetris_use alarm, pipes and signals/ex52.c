/**
 * Hadas Berger
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <termio.h>
#include <memory.h>
#include <signal.h>

#define FAILURE -1
#define FAIL "Error in system call\n"
#define ERROR_SIZE 21
#define SizeBoard 20

/**
 * enum and structs
 */
enum state {STANDING, LYING};
typedef struct Point {
    int xRow;
    int yCol;
} Point;
typedef struct Player {
    enum state state;
    Point help;
} Player;

/**
 *move the player down according ALARM signal every second
 */
void moveEverySec();

/**
 * print the Tetris board game- after update player position
 */
void printBoard();

/**
 * write error - in system call
 */
void checkError();

/**
 * get from the Pipe char and move the player.
 * In addition, send signal to SIGUSR2
 */
void stepToDo();

/**
 * Moves the player to the left and checks that it does not cross the bound
 */
void moveLeft();

/**
 * Moves the player to the right and checks that it does not cross the bound
 */
void moveRight();

/**
 * Moves the player down and checks that it does not cross the bound
 */
void moveDown();

/**
 * Change the player state and save no limit
 */
void oppositeShape();

/**
 * default values to the start of the game
 */
void initializeGame();

/**
 * print the board game
 * @param board
 */
void print(char board[20][20]);

/**
 * check if the player on the end of the board. if True- initialize is place on top again
 */
void initializePlayerPlace();

void moveEverySec(){
    alarm(1);
    signal(SIGALRM,moveEverySec);
    moveDown();
    printBoard();
}

void stepToDo() {
    char tav;
    scanf("%c", &tav);
    switch (tav) {
        case 'a':
             moveLeft();
            break;
        case 'd':
            moveRight();
            break;
        case 's':
            moveDown();
            break;
        case 'w':
            oppositeShape();
            break;
        case 'q':
            exit(0);
    }
    printBoard();
    signal(SIGUSR2,stepToDo);

}

Player player;
int main() {
    /*initialize the player*/
    initializeGame();
    /*move every sec*/
    signal(SIGALRM, moveEverySec);
    alarm(1);
    printBoard(); //TODO delete?
    /*do move*/
    signal(SIGUSR2, stepToDo);
    while (1) {
        pause();
    }
}

void initializeGame() {
    player.state=LYING;
    player.help.xRow=1;
    /*middle*/
    player.help.yCol=SizeBoard/2;
}

void moveDown() {
    if (player.state == LYING) {
        if (player.help.xRow < SizeBoard - 1) {
            player.help.xRow += 1;
        }
    }
    /*standing*/
    if (player.state == STANDING) {
        if (player.help.xRow < SizeBoard - 2)
            player.help.xRow += 1;
    }
}

void oppositeShape() {
    if (player.state == LYING) {
        player.state = STANDING;
    }
    /*standing*/
    else if (player.state == STANDING) {
        /*check the limits*/
        if ((player.help.yCol >= 2) && (player.help.yCol <= SizeBoard - 3))
            player.state = LYING;
    }
}

void moveLeft() {
    if (player.state == LYING) {
        if ((player.help.yCol >=3)) {
            player.help.yCol-=1;
        }
    /*standing*/
    } else {
        if ((player.help.yCol > 1)) {
            player.help.yCol-=1;
        }
    }
}

void moveRight() {
    if (player.state == LYING) {
        if ((player.help.yCol <=SizeBoard-4)) {
            player.help.yCol+=1;
        }
    /*standing*/
    } else {
        if ((player.help.yCol < SizeBoard-2)) {
            player.help.yCol+=1;
        }
    }
}

void printBoard(){
    /*clean the board*/
    if(system("clear")==FAILURE)
        checkError();

    /*create game board*/
    char board[SizeBoard][SizeBoard];
    for (int i = 0; i < SizeBoard; ++i) {
        for (int j = 0; j <SizeBoard ; ++j) {
            board[i][j]=' ';
            board[i][0]='*';
            board[i][SizeBoard-1]='*';
            board[SizeBoard-1][j]='*';
        }
    }

    /*draw the player*/
    board[player.help.xRow][player.help.yCol]='-';
    if(player.state==STANDING){
        board[player.help.xRow-1][player.help.yCol] = '-';
        board[player.help.xRow+1][player.help.yCol] = '-';
    } else if(player.state==LYING){
        board[player.help.xRow][player.help.yCol+1] = '-';
        board[player.help.xRow][player.help.yCol-1] = '-';
    }

    /*print the board*/
    print(board);

    /*initialize player place*/
    initializePlayerPlace();
}

void initializePlayerPlace() {
    if(player.state==LYING){
        /*if we at the end of the board*/
        if(player.help.xRow>=SizeBoard-1) {
            //initializeGame()
            player.help.xRow = 1;
            player.help.yCol = SizeBoard / 2;
            printBoard();
        }
    } else if(player.state==STANDING){
        if(player.help.xRow>=SizeBoard-2){
            //initializeGame()
            player.help.xRow = 1;
            player.help.yCol = SizeBoard / 2;
            player.state=LYING;
            printBoard();
        }
    }
}

void print(char board[20][20]) {
    for (int l = 0; l <SizeBoard ; ++l) {
        for (int t = 0; t < SizeBoard; ++t) {
            printf("%c",board[l][t]);
        }
        printf("\n");
    }
}

void checkError() {
    write(2, FAIL, ERROR_SIZE);
    exit(FAILURE);
}