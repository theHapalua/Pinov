#include <asm-generic/ioctls.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include "scrctrl.h"

int main(){

    //Getting Window Size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // Screen Character Buffer Definition
    char **charBuffer = (char **)calloc(w.ws_row+1,sizeof(char *));
    for(int i = 0;i < w.ws_row;i++){
        char *line;line = (char *)calloc(w.ws_col+1,sizeof(char));
        memset(line,' ',w.ws_col);
        line[w.ws_col] = '\0';
        charBuffer[i] = line;

    }
    charBuffer[w.ws_row] = NULL;

    //Foreground Color Buffer Definition
    int **FGColorBuffer;
    FGColorBuffer = (int **)calloc(w.ws_row + 1, sizeof(int *));
    for (int i = 0; i < w.ws_row; i++) {
        int *line = (int *)calloc(w.ws_col + 1, sizeof(int));
        for (int j = 0; j < w.ws_col; j++) {
            line[j] = 1;
        }
        line[w.ws_col] = 0;
        FGColorBuffer[i] = line;
    }
    FGColorBuffer[w.ws_row] = NULL;

    //Background Color Buffer Definition
    int **BGColorBuffer; BGColorBuffer = (int **)calloc(w.ws_row+1,sizeof(int *));
    for(int i=0;i < w.ws_row;i++){
        int *line = (int *)calloc(w.ws_col+1,sizeof(int *));
        for(int j = 0; j < w.ws_col;j++){
            line[j] = 1;
        }
        line[w.ws_col] = 0;
        BGColorBuffer[i] = line;
    }
    BGColorBuffer[w.ws_row] = NULL;


    //Turning echoing off in stdin
    struct termios term;
    tcgetattr(1, &term);
    term.c_lflag &= -ECHO;//Blocking the echo of normal characters
    term.c_lflag &= -ECHOCTL;//Blocking the echo of control characters
    term.c_lflag &= -ICANON;//Blocking Canonical Mode
    tcsetattr(1, TCSANOW, &term);

    runScreen(charBuffer,FGColorBuffer,BGColorBuffer);

    //Clearing Buffers
    free(charBuffer);
    free(FGColorBuffer);
    free(BGColorBuffer);

    //Turning echoing on in stdin
    term.c_lflag |= ECHO;
    term.c_lflag |= ECHOCTL;
    term.c_lflag |= ICANON;
    tcsetattr(1, TCSANOW, &term);

    return 0;
}

      
      
