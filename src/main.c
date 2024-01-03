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

int col;
int row;

int main(){

	// Reading Configuration
	ConfigurationField* config_list = calloc(CONFIG_COUNT, sizeof(ConfigurationField));
	read_config(config_list);

    signal(SIGWINCH,&resizeSignal);


    // Getting Window Size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    col = w.ws_col;
    row = w.ws_row;

    // Default Window Size
    if(col < DEFAULT_COLUMN){
        col = DEFAULT_COLUMN;
    }
    if(row < DEFAULT_ROW){
        row = DEFAULT_ROW;
    }
    resizeScreen(row, col);

    // Screen Character Buffer Definition
    char **charBuffer = (char **)calloc(row+1,sizeof(char *));
    for(int i = 0;i < row;i++){
        char *line;line = (char *)calloc(col+1,sizeof(char));
        memset(line,' ',col);
        line[col] = '\0';
        charBuffer[i] = line;

    }
    charBuffer[row] = NULL;

    // Foreground Color Buffer Definition
    int **FGColorBuffer;
    FGColorBuffer = (int **)calloc(row + 1, sizeof(int *));
    for (int i = 0; i < row; i++) {
        int *line = (int *)calloc(col + 1, sizeof(int));
        for (int j = 0; j < col; j++) {
            line[j] = 1;
        }
        line[col] = 0;
        FGColorBuffer[i] = line;
    }
    FGColorBuffer[row] = NULL;

    // Background Color Buffer Definition
    int **BGColorBuffer; BGColorBuffer = (int **)calloc(row+1,sizeof(int *));
    for(int i=0;i < row;i++){
        int *line = (int *)calloc(col+1,sizeof(int *));
        for(int j = 0; j < col;j++){
            line[j] = 1;
        }
        line[col] = 0;
        BGColorBuffer[i] = line;
    }
    BGColorBuffer[row] = NULL;


    // Turning echoing off in stdin
    struct termios term;
    tcgetattr(1, &term);
    term.c_lflag &= -ECHO;// Blocking the echo of normal characters
    term.c_lflag &= -ECHOCTL;// Blocking the echo of control characters
    term.c_lflag &= -ICANON;// Blocking Canonical Mode
    tcsetattr(1, TCSANOW, &term);

    runScreen(charBuffer,FGColorBuffer,BGColorBuffer);

    // Clearing Buffers
    free(charBuffer);
    free(FGColorBuffer);
    free(BGColorBuffer);
    
	
    // Turning echoing on in stdin
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

      
    
