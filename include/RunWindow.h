#ifndef RUNWINDOW_H
#define RUNWINDOW_H
#define WIN_HEIGHT 600
#define WIN_WIDTH 385
#include "Window.h"
#include<vector>
#define MAX_INT 1000000

#include <windows.h>
#include "LineOfText.h"

class RunWindow
{
    public:
        RunWindow(HINSTANCE hThisInstance, MSG *messages);
        static LRESULT CALLBACK RunWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        HWND hFrame;
        static RunWindow *instance;
        void setVisible(bool visible);
        virtual ~RunWindow();
        void compile(std::vector<LineOfText> lines);
    protected:
    private:
    WNDCLASSEX wincl;
    char winClassName[20];
    Window* stepB, *runB, *prompt, *num, *submit, *regVal, *stop;
    HWND scrollBar;
    char regStr[30];
    std::vector<Window*> hNumbers;
    std::vector<Window*> hMemory;
    MSG* msgPtr;
    void processScrolling(WPARAM wParam);
    void initLines();
    int numVisible, scrollPosition;
    SCROLLINFO si;

    //for running the assembly program:
    int *memory;
    int memorySize;
    bool validAddress(int address);
    int reg; // register is keyword and thus was taken
    int currentLine;
    void step(); // performs the operation specified on currentLine of memory
    void run();
    void updateDisplay();
    bool stopProgram;
    //Operations
    void STOP();
    void READ();
    void LOAD();
    void STORE();
    void ADD();
    void SUBTRACT();
    void BRANCH();
    void BRANCHZ();
    void BRANCHP();
    void PRINT();
    void BEEP();

    int getPayload();
};

#endif // RUNWINDOW_H
