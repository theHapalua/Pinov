//SCREEN CONTROL HEADER FILE
#ifndef SCREEN_CONTROL
#define SCREEN_CONTROL

#define BGCOLOR1 "\x1b[48;2;26;11;12m"
#define BGCOLOR2 "\x1b[48;2;96;220;148m"

#define FGCOLOR1 "\x1b[38;2;226;165;40m"

#define DEFAULT_COLUMN 95
#define DEFAULT_ROW 27


void setCol(int val);
void setRow(int val);
void mallocCharBuffer(int rowSize, int colSize);
void reallocCharBuffer(int rowSize, int colSize);
void setCharBuffer(char **val);
void mallocFGColorBuffer(int rowSize, int colSize);
void reallocFGColorBuffer(int size);
void setFGColorBuffer(int **val);
void mallocBGColorBuffer(int rowSize, int colSize);
void reallocBGColorBuffer(int size);
void setBGColorBuffer(int **val);
void resetColor();
void cursorHome();
void cleanScreen();
void updateScreen();
void enableAltScreen();
void disableAltScreen();
void runScreen();
void resizeScreen(int row, int col);
void visibleCursor();
void invisibleCursor();
void cursorUp();
void cursorDown();
void cursorRight();
void cursorLeft();
void resizeSignal();
void printIndex(int row, int col, char** buffer, char *text);
void addRows();
void addColumns();
char getCharBuffer(int y, int x);
void mallocTextBuffer();
int getLineCountOfTextBuffer();

#endif