#include <asm-generic/ioctls.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include "scrctrl.h"

void resetColor(){
    printf("\x1b[0m");
}

void cursorHome(){
    printf("\x1b[H");
}

void cleanScreen(){
    printf("\x1b[2J");
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
                }
            }
        }
    }

    return ;
}

void enableAltScreen(){
    printf("\x1b[?1049h");
    return ;
}

void disableAltScreen(){
    printf("\x1b[?1049l");
    return ;
}

void runScreen(char **charBuffer, int **FGColorBuffer, int **BGColorBuffer){
    enableAltScreen();
    updateScreen(charBuffer, FGColorBuffer, BGColorBuffer);
    while(1){
        char c = getchar();
        if(c == 'q' || c == 'Q'){
            break;
        }else if(c == '\x1b'){
            //THE ESC OR A KEY WITH ESC IS PRESSED
            c = getchar();//[
            c = getchar();//A,B,C,D
            if(c == 'A'){
                printf("\x1b[1A");
            }else if(c == 'B'){
                printf("\x1b[B");
            }else if(c == 'C'){
                printf("\x1b[1C");
            }else if(c == 'D'){
                printf("\x1b[1D");
            }else{
                break;
            }
        }else if(c == 'c' || c == 'C'){
            //change Color
            for(int y = 0;;y++){
                if(BGColorBuffer[y] == NULL){
                    break;
                }
                for(int x = 0;;x++){
                    if(BGColorBuffer[y][x] == 0){
                        break;
                    }
                    if(x%2==0 || y%2==0){
                        BGColorBuffer[y][x] = 2;
                    }
                }
            }
            updateScreen(charBuffer,FGColorBuffer,BGColorBuffer);
        }
    }
    disableAltScreen();
    return ;
}