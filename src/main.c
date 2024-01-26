#include <asm-generic/ioctls.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <signal.h>
#include "config.h"
#include "scrctrl.h"

extern int col;
extern int row;
extern char **screenCharBuffer;
extern int **FGColorBuffer;
extern int **BGColorBuffer;

int main(){

	// Reading Configuration
	ConfigurationField* config_list = calloc(CONFIG_COUNT, sizeof(ConfigurationField));
	read_config(config_list);

    //signal(SIGWINCH,&resizeSignal);


    // Getting Window Size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    setCol(w.ws_col);
    setRow(w.ws_row);

    //Default Window Size
    if(col < DEFAULT_COLUMN){
        col = DEFAULT_COLUMN;
    }
    if(row < DEFAULT_ROW){
        row = DEFAULT_ROW;
    }
    resizeScreen(row, col);
    
    

    // Screen Character Buffer Definition
    mallocCharBuffer(row,col);
    char **tmpCharBuffer = (char **)calloc(row+1,sizeof(char *));
    for(int i = 0;i < row;i++){
        char *line;line = (char *)calloc(col+1,sizeof(char));
        memset(line,' ',col);
        line[col] = '\0';
        tmpCharBuffer[i] = line;

    }
    tmpCharBuffer[row] = NULL;
    setCharBuffer(tmpCharBuffer);
    free(tmpCharBuffer);

    // Foreground Color Buffer Definition
    mallocCharBuffer(row,col);
    int ** tmpFGColorBuffer = (int **)calloc(row + 1, sizeof(int *));
    for (int i = 0; i < row; i++) {
        int *line = (int *)calloc(col + 1, sizeof(int));
        for (int j = 0; j < col; j++) {
            line[j] = 1;
        }
        line[col] = 0;
        tmpFGColorBuffer[i] = line;
    }
    tmpFGColorBuffer[row] = NULL;
    setFGColorBuffer(tmpFGColorBuffer);
    //free(tmpFGColorBuffer);

    //Background Color Buffer Definition
    mallocBGColorBuffer(row,col);
    int **tmpBGColorBuffer = (int **)calloc(row+1,sizeof(int *));
    for(int i=0;i < row;i++){
        int *line = (int *)calloc(col+1,sizeof(int *));
        for(int j = 0; j < col;j++){
            line[j] = 1;
        }
        line[col] = 0;
        tmpBGColorBuffer[i] = line;
    }
    tmpBGColorBuffer[row] = NULL;
    setBGColorBuffer(tmpBGColorBuffer);
    //free(tmpBGColorBuffer);


    // Turning echoing off in stdin
    struct termios term;
    tcgetattr(1, &term);
    term.c_lflag &= -ECHO;// Blocking the echo of normal characters
    term.c_lflag &= -ECHOCTL;// Blocking the echo of control characters
    term.c_lflag &= -ICANON;// Blocking Canonical Mode
    tcsetattr(1, TCSANOW, &term);

    //addRows(FGColorBuffer, BGColorBuffer, 1);
    
    mallocTextBuffer();
    runScreen();
    //printf("%c\n",getCharBuffer(0, 0));
    int y = 0;
    int x = 0;
    //printf("%c : %d : %d",screenCharBuffer[y][x],FGColorBuffer[y][x],BGColorBuffer[y][x]);

    //Clearing Buffers

    //Turning echoing on in stdin
    term.c_lflag |= ECHO;
    term.c_lflag |= ECHOCTL;
    term.c_lflag |= ICANON;
    tcsetattr(1, TCSANOW, &term);

    system("clear");
    

    // Test stuff
    
    printf("Name: %s\n", get_config_field("BACKGROUND_COLOR", config_list).name);
	printf("Value (int): %d\n", get_config_field("BACKGROUND_COLOR", config_list).value.int_value);
	
	printf("Name: %s\n", get_config_field("LINE_WRAPPING", config_list).name);
	printf("Value (bool): %s\n", get_config_field("LINE_WRAPPING", config_list).value.bool_value ? "true" : "false");
    
    printf("Name: %s\n", get_config_field("CHAR_TEST", config_list).name);
	printf("Value (char): %c\n", get_config_field("CHAR_TEST", config_list).value.char_value);
    
    printf("Name: %s\n", get_config_field("STRING_TEST", config_list).name);
	printf("Value (string): %s\n", get_config_field("STRING_TEST", config_list).value.string_value);
    
    
    return 0;
}

      
    
