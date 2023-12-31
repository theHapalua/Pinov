#include <asm-generic/ioctls.h>
#include <bits/types/struct_timeval.h>
#include <sys/select.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include "scrctrl.h"

extern int col;
extern int row;

void resizeSignal(){
    resizeScreen(row,col);
    return ;
}

void resetColor(){
    printf("\x1b[0m");
    fflush(stdout);
}

void cursorUp(){
    printf("\x1b[A");
    fflush(stdout);
}

void cursorDown(){
    printf("\x1b[B");
    fflush(stdout);
}

void cursorRight(){
    printf("\x1b[C");
    fflush(stdout);
}

void cursorLeft(){
    printf("\x1b[D");
    fflush(stdout);
}

void cursorHome(){
    printf("\x1b[H");
    fflush(stdout);
}

void cleanScreen(){
    printf("\x1b[2J");
    fflush(stdout);
}

void updateScreen(char **charBuffer, int **FGColorBuffer, int **BGColorBuffer){
    cleanScreen();
    cursorHome();

    for(int y = 0;;y++){
        if(charBuffer[y] == NULL){
            break;
        }else{
            for(int x = 0;;x++){
                if(charBuffer[y][x] == '\0'){
                    if(charBuffer[y+1] != NULL){
                        printf("\n");
                    }
                    break;
                }else{
                    if(FGColorBuffer[y][x] == 1){
                        printf("%s",FGCOLOR1);
                    }
                    if(BGColorBuffer[y][x] == 1){
                        printf("%s",BGCOLOR1);
                    }else if(BGColorBuffer[y][x] == 2){
                        printf("%s",BGCOLOR2);
                    }
                    printf("%c",charBuffer[y][x]);
                    resetColor();
                    fflush(stdout);
                }
            }
        }
    }

    return ;
}

void enableAltScreen(){
    printf("\x1b[?1049h");
    fflush(stdout);
    return ;
    
}

void disableAltScreen(){
    printf("\x1b[?1049l");
    fflush(stdout);
    return ;
}

void visibleCursor(){
    printf("\x1b[?25h");
    fflush(stdout);
}

void invisibleCursor(){
    printf("\x1b[?25l");
    fflush(stdout);
}

void resizeScreen(int row, int col){
    printf("\x1b[8;%d;%dt",row,col);
    fflush(stdout);
    return ;
}

void runScreen(char **charBuffer, int **FGColorBuffer, int **BGColorBuffer){
    enableAltScreen();
    updateScreen(charBuffer, FGColorBuffer, BGColorBuffer);
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 100;
    while(1){
        char c;read(STDIN_FILENO,&c,1); //c = getchar();
        if(c == 0x1B){
            //esc is pressed
            int ready = select(STDIN_FILENO+1,&set,NULL,NULL,&timeout);
            if(ready == 0){
                break;
            }else{
                c = getchar();
                c = getchar();
                if(c == 'A'){
                    cursorUp();
                }else if(c == 'B'){
                    cursorDown();
                }else if(c == 'C'){
                    cursorRight();
                }else if(c == 'D'){
                    cursorLeft();
                }
            }
            
        }
    }
    disableAltScreen();
    return ;
}
