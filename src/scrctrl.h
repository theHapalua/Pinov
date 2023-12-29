//SCREEN CONTROL HEADER FILE
#ifndef SCREEN_CONTROL
#define SCREEN_CONTROL

#define BGCOLOR1 "\x1b[48;2;176;146;106m"
#define BGCOLOR2 "\x1b[48;2;96;220;148m"

#define FGCOLOR1 "\x1b[38;2;220;220;220m"

void resetColor();
void cursorHome();
void cleanScreen();
void updateScreen(char **charBuffer, int **FGColorBuffer, int **BGColorBuffer);
void enableAltScreen();
void disableAltScreen();
void runScreen(char **charBuffer, int **FGColorBuffer, int **BGColorBuffer);

#endif