#include <windows.h>
#include "Window.h"
#include "Editor.h"
#include "RunWindow.h"

#ifndef GUI_H
#define GUI_H

class GUI
{
    public:
        void handleButtonPress(WORD num);
        RECT windowArea;
        int winHeight;
        int winWidth;
        int nCmdShow;
        char szClassName[256];
        HWND hFrame;               /* This is the handle for our window */
        MSG messages;            /* Here messages to the application are saved */
        WNDCLASSEX wincl;        /* Data structure for the windowclass */
        Window *load, *save, *compile, *delLine, *help;
        Window* commands[10];
        Editor* edit;
        GUI(HINSTANCE hThisInstance, int nCmdShow, WNDPROC WindowProcedure);
        virtual ~GUI();
        void createMainWindowFrame();
        void drawIDE();
        void saveFilePrompt();
    protected:
    private:
        void getWindowDimenstions();
        std::string commandStr[10];
        HINSTANCE hThisInstance;
};

#endif // GUI_H
