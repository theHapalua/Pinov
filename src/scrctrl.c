#include <stdbool.h>
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

int col;
int row;
char **screenCharBuffer;//A buffer for chars that will be printed in screen
int **FGColorBuffer;
int **BGColorBuffer;
char *textBuffer;//A buffer for chars that will be printed in file
unsigned long int textBufferSize = 65536;
int currentCursorRow;
int currentCursorCol;
int screenStartColIndex = 1;
int screenStartRowIndex = 1;
bool checkval = false;

void mallocTextBuffer(){
    textBuffer = (char *)malloc(sizeof(char)*textBufferSize);
    memset(textBuffer, 0, textBufferSize);
    return;
}

void setTextBuffer(char newText[]){
    for(int i = 0;i < textBufferSize;i++){
        if(newText[i] == '\0'){
            break;
        }else{
            textBuffer[i] = newText[i];
        }
    }
}

void setCursor(int x, int y){
    cursorHome();
    for(int i = 1;i < x;i++){
        cursorRight();
    }
    for(int i = 1;i < y;i++){
        cursorDown();
    }
}

//GET LINE LENGTH
int getLineLengthOfTextBuffer(int y){
    int linecount = getLineCountOfTextBuffer();
    int currentLine = 0;
    int lineLength = 0;
    if(linecount == 0 || linecount < y || y == 0){
        return 0;
    }else{
        currentLine = 1;
        for(int i =0;;i++){
            if(currentLine < y){
                if(textBuffer[i] == '\n'){
                    currentLine++;
                }
                continue;
            }else if(currentLine == y){
                if(textBuffer[i] == '\n' || textBuffer[i] == 0){
                    lineLength++;
                    break;
                }else{
                    lineLength++;
                }
            }
        }
    }

    return lineLength;
}
//GET LINE COUNT
int getLineCountOfTextBuffer(){
    int linecount = 0;
    if(textBuffer[0] != 0){
        linecount++;
    }
    for(int i = 1;;i++){
        if(textBuffer[i] == 0){
            break;
        }else if(textBuffer[i] == '\n'){
            linecount++;
        }
    }
    return linecount;
}


int scrIndexToTextBufferIndex(int x, int y){
    int index = 0;
    int cury = 1;
    int curx = 1;
    while(1){
        if(cury == y && curx == x){
            break;
        }else if(textBuffer[index] == 0 && (cury != y || curx != x)){
            index = -1;
            break;
        }else{
            if(textBuffer[index] == '\n'){
                cury++;
                curx = 1;
            }else{
                curx++;
                //printf("%c\n",textBuffer[index]);
            }
            index++;
        }
    }
    return index;
}
//REWRITE THIS FUNCTION 
//startcol and startrow
void synchronizeCharBufferWithTextBuffer(){
    int x = 0;
    int y = 0;
    while(1){
        if(screenCharBuffer[y] == NULL){
            break;
        }else{
            if(screenCharBuffer[y][x] == '\0'){
                y++;
                x = 0;
            }else{
                screenCharBuffer[y][x] = ' ';
                x++;
            }
        }
    }
    x = 1;
    y = 1;
    bool lineOverflow = false;
    for(unsigned long int i = 0;i < textBufferSize;i++){
        if(lineOverflow){
            if(textBuffer[i] == '\n'){
                lineOverflow = false;
            }else{
                continue;
            }
        }

        if(textBuffer[i] == '\0'){
            break;
        }else if(textBuffer[i] == '\n'){
            y++;
            x = 1;
        }else{
            if(y >= screenStartRowIndex && x >= screenStartColIndex){
                if(screenCharBuffer[y-screenStartRowIndex] == NULL){
                    break;
                }else if(screenCharBuffer[y-screenStartRowIndex][x-screenStartColIndex] == '\0'){
                    lineOverflow = true;
                }else{
                    screenCharBuffer[y-screenStartRowIndex][x-screenStartColIndex] = textBuffer[i];
                    printf("%c",textBuffer[i]);
                    x++;
                }
            }else{
                x++;
            }
        }
    }

}
int getCharBufferRowLength(){
    int length = 0;
    while(1){
        if(screenCharBuffer[length++] == NULL){
            break;
        }
    }
    return length;
}

int getCharBufferColLength(){
    int length = 0;
    if(getCharBufferRowLength() >= 1){
        while(1){
            if(screenCharBuffer[0][length++] == '\0'){
                break;
            }
        }
    }
    return length;
}

void putCharBufferIndex(int startRow,int startCol,char text[]){
    //Text has to be null ended and has to fit into screenCharBuffer index
    char *outText = text;
    int currentCharRow = startRow;
    int currentCharCol = startCol;
    for(int i = 0;;i++){
        if(getCharBufferColLength() >= currentCharCol && getCharBufferRowLength() >= currentCharRow){
            if(outText[i] != '\0' && outText[i] != '\n'){//Forbidden Characters
                screenCharBuffer[currentCharRow][currentCharCol] = outText[i];
            }else{
                break;
            }
            currentCharCol++;
        }else{
            break;
        }
    }
}

void setCol(int val){
    col = val;
    return;
}

void setRow(int val){
    row = val;
    return ;
}
void mallocCharBuffer(int rowSize, int colSize){
    screenCharBuffer = (char **)malloc(sizeof(char*)*(rowSize+1));
    for(int i = 0;i < rowSize;i++){
        char* line = (char*)malloc(sizeof(char)*(colSize+1));
        memset(line,' ',col);
        line[colSize] = '\0';
        screenCharBuffer[i] = line;
    }
    screenCharBuffer[rowSize] = NULL;
}

void reallocCharBuffer(int rowSize, int colSize){
    //screenCharBuffer = (char**)realloc(screenCharBuffer,size);
}

void setCharBuffer(char **val){
    for(int y = 0;;y++){
        if(val[y] == NULL){
            fflush(stdout);
            break;
        }else{
            for(int x=0;;x++){
                if(screenCharBuffer[y][x] == '\0'){
                    break;
                }else{
                    screenCharBuffer[y][x] = val[y][x];
                }
            }
        }
    }
    
}

char getCharBuffer(int y, int x){
    return screenCharBuffer[y][x];
}

void mallocFGColorBuffer(int rowSize, int colSize){
    FGColorBuffer = (int **)malloc(sizeof(int*)*(rowSize+1));
    for(int i = 0;i < rowSize;i++){
        int* line = (int*)malloc(sizeof(int)*(colSize+1));
        line[colSize] = 0;
        FGColorBuffer[i] = line;
        free(line);
    }
    FGColorBuffer[rowSize] = NULL;
}

void reallocFGColorBuffer(int size){
    FGColorBuffer = (int **)realloc(FGColorBuffer,size);
}

void setFGColorBuffer(int **val){
    FGColorBuffer = val;
    for(int y = 0;;y++){
        if(FGColorBuffer[y] == NULL){
            break;
        }else{
            for(int x=0;;x++){
                if(FGColorBuffer[y][x] == 0){
                    break;
                }else{
                    FGColorBuffer[y][x] = val[y][x];
                    //printf("%d\n",val[y][x]);
                }
            }
        }
    }
}

void mallocBGColorBuffer(int rowSize, int colSize){
    BGColorBuffer = (int **)malloc(sizeof(int*)*(rowSize+1));
    for(int i = 0;i < rowSize;i++){
        int* line = (int*)malloc(sizeof(int)*(colSize+1));
        line[colSize] = 0;
        BGColorBuffer[i] = line;
        free(line);
    }
    BGColorBuffer[colSize] = NULL;
}

void reallocBGColorBuffer(int size){
    BGColorBuffer = (int **)realloc(BGColorBuffer,size);
}

void setBGColorBuffer(int **val){
    BGColorBuffer = val;
    for(int y = 0;;y++){
        if(BGColorBuffer[y] == NULL){
            break;
        }else{
            for(int x=0;;x++){
                if(BGColorBuffer[y][x] == 0){
                    break;
                }else{
                    BGColorBuffer[y][x] = val[y][x];
                }
            }
        }
    }
    
}

void addRows(){
    int colLength = 0;//without \0
    for(;;colLength++){
        if(screenCharBuffer[0][colLength] == '\0'){
            break;
        }
    }
    int nullIndex = 0;
    while(1){
        if(screenCharBuffer[nullIndex] == NULL){
            break;
        }else{
            nullIndex++;
        }
    }
    
    //Update Char Buffer
    //screenCharBuffer = realloc(screenCharBuffer,(nullIndex+rowCount+1)*sizeof(char*));//This Causes segfault
    /*for(int i = 0;i<rowCount;i++){
        char *line;line = (char *)calloc(colLength+1,sizeof(char));
        memset(line,' ',colLength);
        line[colLength] = '\0';
        screenCharBuffer[nullIndex+i] = line;
    }
    screenCharBuffer[nullIndex+rowCount] = NULL;

    //Update Foreground Color Buffer
    FGColorBuffer = (int**)realloc(FGColorBuffer,(nullIndex+rowCount+1)*sizeof(int*));
    for(int i = 0;i<rowCount;i++){
        int *line = (int *)calloc(colLength + 1, sizeof(int));
        for (int j = 0; j < colLength; j++) {
            line[j] = 1;
        }
        line[colLength] = 0;
        FGColorBuffer[nullIndex+i] = line;
    }
    FGColorBuffer[nullIndex+rowCount] = NULL;

    //Update Background Color Buffer
    BGColorBuffer = (int**)realloc(BGColorBuffer,(nullIndex+rowCount+1)*sizeof(int*));
    for(int i = 0;i<rowCount;i++){
        int *line = (int *)calloc(colLength + 1, sizeof(int));
        for (int j = 0; j < colLength; j++) {
            line[j] = 1;
        }
        line[colLength] = 0;
        BGColorBuffer[nullIndex+i] = line;
    }
    BGColorBuffer[nullIndex+rowCount] = NULL;*/

    return;
}

void addColumns(){

}

void resetColor(){
    printf("\x1b[0m");
    fflush(stdout);
}

void cursorUp(){
    if(currentCursorRow > 1){
        currentCursorRow--;
        printf("\x1b[A");
        fflush(stdout);
    }
}

void cursorDown(){
    if(currentCursorRow < row ){
        currentCursorRow++;
        printf("\x1b[B");
        fflush(stdout);
    }
    
}

void cursorRight(){
    if(currentCursorCol < col){
        currentCursorCol++;
        printf("\x1b[C");
        fflush(stdout);
    }
    
}

void cursorLeft(){
    if(currentCursorCol > 1){
        currentCursorCol--;
        printf("\x1b[D");
        fflush(stdout);
    }

}

void cursorHome(){
    currentCursorCol = 1;
    currentCursorRow = 1;
    printf("\x1b[H");
    fflush(stdout);
}

void cleanScreen(){
    printf("\x1b[2J");
    fflush(stdout);
}

void updateScreen(){
    cleanScreen();
    cursorHome();
    for(int y = 0;;y++){
        if(screenCharBuffer[y] == NULL){
            break;
        }else{
            for(int x = 0;;x++){
                if(screenCharBuffer[y][x] == '\0'){
                    if(screenCharBuffer[y+1] != NULL){
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
                    printf("%c",screenCharBuffer[y][x]);
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

void runScreen(){
    enableAltScreen();
    setTextBuffer("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed ac nisl quam. Vivamus dictum turpis.\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Sed ac nisl quam. Vivamus dictum turpis.\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Sed ac nisl quam. Vivamus dictum turpis.\nLorem\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Sed ac nisl quam. Vivamus dictum\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Sed ac nisl quam. Vivamus \n");
    //setTextBuffer("aaaaaaaaaaaa\naaa\naaaaaaaaaaaaaaaaaa\naaaaaa\n\naaaaaaa\naaaaaaaaaaaa\naaaa");
    synchronizeCharBufferWithTextBuffer();
    updateScreen();
    cursorHome();
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 100;
    int i = 1;
    int sa = 1;
    while(1){
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        if(w.ws_col != col){
            //add new columns to buffer
        }
        if(w.ws_row != row){
            //add new rows to buffer
            //addRows();
            //updateScreen();
            //i have to turn getting key part as another thread
        }
        char c;read(STDIN_FILENO,&c,1); //c = getchar();
        if(c == 0x1B){
            //esc is pressed
            int ready = select(STDIN_FILENO+1,&set,NULL,NULL,&timeout);
            if(ready == 0){
                //ESC
                break;
            }else{
                c = getchar();//[
                c = getchar();                
                if(c == 'A'){
                    //UP ARROW
                    if(currentCursorRow != 1){
                        while(currentCursorCol+screenStartColIndex-1 > getLineLengthOfTextBuffer(currentCursorRow-2+screenStartRowIndex)){
                            cursorLeft();
                            if(currentCursorCol == 1){
                                break;
                            }
                        }
                        while(currentCursorCol == 1 && getLineLengthOfTextBuffer(currentCursorRow+screenStartRowIndex-2)-screenStartColIndex-1<0){
                            checkval = true;
                            if(screenStartColIndex > 1){
                                screenStartColIndex--;
                                int x = currentCursorCol;
                                int y = currentCursorRow;
                                synchronizeCharBufferWithTextBuffer();
                                updateScreen();
                                setCursor(x, y);
                            }else{
                                break;
                            }
                        }
                        cursorUp();
                    }
                }else if(c == 'B'){
                    //DOWN ARROW
                    if(currentCursorRow != getLineCountOfTextBuffer()-screenStartRowIndex+1){
                        while(currentCursorCol+screenStartColIndex-1 > getLineLengthOfTextBuffer(currentCursorRow+screenStartRowIndex)){
                            cursorLeft();
                            if(currentCursorCol == 1){
                                break;
                            }
                        }
                        while(currentCursorCol == 1 && getLineLengthOfTextBuffer(currentCursorRow+screenStartRowIndex)-screenStartColIndex-1<0){
                            checkval = true;
                            if(screenStartColIndex > 1){
                                screenStartColIndex--;
                                int x = currentCursorCol;
                                int y = currentCursorRow;
                                synchronizeCharBufferWithTextBuffer();
                                updateScreen();
                                setCursor(x, y);
                            }else{
                                break;
                            }
                        }
                        cursorDown();
                    }
                    /*if(currentCursorRow+screenStartRowIndex-1 < getLineCountOfTextBuffer()-screenStartRowIndex+1){
                        cursorDown();
                    }*/
                }else if(c == 'C'){
                    //RIGHT ARROW
                    if(currentCursorCol < col){
                        if(currentCursorCol+screenStartColIndex-1 < getLineLengthOfTextBuffer(currentCursorRow+screenStartRowIndex-1)){
                            cursorRight();
                        }
                    }else if(currentCursorCol == col){
                        if(getLineLengthOfTextBuffer(currentCursorRow+screenStartRowIndex-1)-screenStartColIndex+1 > col){
                            screenStartColIndex++;
                            int x = currentCursorCol;
                            int y = currentCursorRow;
                            synchronizeCharBufferWithTextBuffer();
                            updateScreen();
                            setCursor(x, y);
                        }
                    }
                }else if(c == 'D'){
                    //LEFT ARROW
                    if(currentCursorCol > 1){
                        cursorLeft();
                    }else{
                        if(screenStartColIndex > 1){
                            screenStartColIndex--;
                            int x = currentCursorCol;
                            int y = currentCursorRow;
                            synchronizeCharBufferWithTextBuffer();
                            updateScreen();
                            setCursor(x, y);
                        }
                    }
                    
                }else if(c == '3'){
                    c = getchar();
                    if(c == '~'){
                        //Delete
                        printf("%s",BGCOLOR1);
                        printf("%s",FGCOLOR1);
                        printf(" ");
                        currentCursorCol++;
                        resetColor();
                        cursorLeft();
                    }
                }
            } 
        }else if(c == 127){
            //BACKSPACE
            cursorLeft();
            printf("%s",BGCOLOR1);
            printf("%s",FGCOLOR1);
            printf(" ");
            currentCursorCol++;
            resetColor();
            fflush(stdout);
            cursorLeft();

        }else if(c == 10){
            //ENTER
            cursorDown();
        }else {
            if(currentCursorCol == col){
                //You have to slide screen when you enter char
                printf("%s",BGCOLOR1);
                printf("%s",FGCOLOR1);
                printf("%c",c);
                resetColor();
                fflush(stdout);
                cursorLeft();
                cursorRight();
            }else{
                printf("%s",BGCOLOR1);
                printf("%s",FGCOLOR1);
                printf("%c",c);
                currentCursorCol++;
                resetColor();
                fflush(stdout);
            }
            

        }
    }
    disableAltScreen();
    //printf("%d,%d\n",currentCursorCol,col);
    if(checkval){
        printf("you are in \n");
    }
    return ;
}

void printIndex(int row, int col, char**buffer, char *text){


    return ;
}