#include "GUI.h"
#include "Window.h"
#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <string>
#define startingDimensions 600
#include "RunWindow.h"

void GUI::getWindowDimenstions()
{
    GetWindowRect(hFrame, &windowArea);
    winHeight = windowArea.bottom - windowArea.top;
    winWidth = windowArea.right - windowArea.left;
}

//Initialise all variables and create the window class
GUI::GUI(HINSTANCE hThisInstance, int nCmdShow, WNDPROC WindowProcedure)
{
    strcpy(szClassName, "Ryan Shanks' York University Assembly Simulator");
    this->hThisInstance = hThisInstance;
    winHeight = startingDimensions;
    winWidth = startingDimensions;

    this->nCmdShow = nCmdShow;
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassExA (&wincl))
    {
        //std::cout << GetLastError() << std::endl;
        exit(-1);
    }
    /* The class is registered, we can now create the window.*/
    createMainWindowFrame();
    drawIDE();
    RunWindow::instance = new RunWindow(hThisInstance, &messages);
}
//create a window frame
void GUI::createMainWindowFrame()
{
    hFrame = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Turassic Enhanced LMC",       /* Title Text */
           WS_SYSMENU | WS_MINIMIZEBOX, /* default window + maximize it */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           winWidth,  /* The programs width */
           winHeight,  /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           wincl.hInstance,     /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hFrame, nCmdShow);
    //SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0); // maximize the window to start.
    commandStr[0] = "READ";
    commandStr[1] = "LOAD";
    commandStr[2] = "STORE";
    commandStr[3] = "ADD";
    commandStr[4] = "SUBTRACT";
    commandStr[5] = "BRANCH";
    commandStr[6] = "BRANCHZ";
    commandStr[7] = "BRANCHP";
    commandStr[8] = "PRINT";
    commandStr[9] = "STOP";
}

//drawing code for the IDE tab
void GUI::drawIDE()
{
    // Create the left bar which lists all the commands
    getWindowDimenstions(); // update the current window dimensions
    double winx = winWidth; // We need more precision if we want everything as a % of the window
    double winy = winHeight; // so we cast those integers to doubles.

    double distFromLeft = winx * 0.01;
    //heading for the left bar
    CreateWindow("static",
                 "Commands:",
                 WS_VISIBLE | WS_CHILD,
                 distFromLeft, winy * 0.01, 100, 20,
                 hFrame, NULL, NULL, NULL);
    //All the following command buttons will add the commands to the editor on the right
    double bHeight = 20;
    double bWidth = winx * 0.2;
    double yGap = winy * 0.087;

    commands[0] = new Window(&hFrame, "BUTTON", commandStr[0], distFromLeft, (int) winy * 0.1, (int)bWidth, (int)bHeight, 0);
    for (int i =1; i < 10; i++)
    {
        commands[i] = new Window(commands[i-1], "BUTTON", commandStr[i], 0, (int)yGap, i);
    }
    //Commands are now listed... Now make an edit box.
    edit = NULL;
    edit = new Editor(&hFrame, &wincl.hInstance, winx, winy, bHeight);
    Editor::instance = edit; // needed to allow the static callback function to access the object

    int xShift = 10;
    delLine = new Window(&hFrame, "BUTTON", "Del Line", (int) winx * 0.3, commands[9]->location.top, ((int)bWidth * 0.61), bHeight, 10);
    load = new Window(delLine, "BUTTON", "Save", xShift + ((int)bWidth * 0.61), 0, 11);
    save = new Window(load, "BUTTON", "Load", xShift + ((int)bWidth * 0.61), 0, 12);
    compile = new Window(save, "BUTTON", "Compile", xShift + ((int)bWidth * 0.61), 0, 13);
    help = new Window(compile, "BUTTON", "Help", xShift + bWidth * 0.61, 0, 14);
}
void GUI::handleButtonPress(WORD num)
{
    if (num < 10) // adding an instruction, pass to the editor
    {
        edit->addInstruction(commandStr[num]);
    }
    //buttons below the editor
    else if (num == 10) // delete line
    {
        edit->deleteLine();
    }
    else if (num == 11) // save
    {
        edit->saveFile();
    }
    else if (num == 12) // load
    {
        edit->loadFile();
    }
    else if (num == 13) // compile
    {
        if (edit->errorCheck())
        {
            RunWindow::instance->compile(edit->getLines());
            RunWindow::instance->setVisible(true);
        }
    }
    else if (num == 14)
    {
        ::MessageBox(hFrame,
                     "Turassic Enhanced LMC\nWritten by Ryan Shanks (2015)\n\nCommands Summary:\nREAD: Reads a number entered by the user and stores it in the register\nLOAD: Loads the memory specified by address into the register\nSTORE: Stores the contents of the register in the specified address\nADD: Adds the memory in the specified address to the register\nSUBTRACT: Subtract the number on the specified address from the register\nBRANCH: Jump to the address (line) specified and start executing from there\nBRANCHZ: Branch if the value in the register is zero, otherwise continue to next line\nBRANCHP: Branch if the value in the register is positive, otherwise continue to the next line\nPRINT: Output the contents of the specified address to the screen\nSTOP: Halt the program\n\nNotes:\n-The address of a line is the same as the line number\n-You can store positve values in 'address' without accompanying instructions and use them as variables in your program\n-You can add or subtract instructions (with or without addresses) and potentially have them run as different instructions\n-There is a hidden beep command, just for fun, but good luck figuring out how to use it"
                     , "Help / Readme: ", MB_OK);
    }
}
void GUI::saveFilePrompt()
{
    if (::MessageBox(hFrame, "Do you want to save?", "Save?", MB_YESNO) == IDYES)
    {
        edit->saveFile();
    }
}

GUI::~GUI()
{

    delete delLine;
    delete load;
    delete save;
    delete compile;
    delete help;
    for (int i =0; i < 10; i++)
    {
        delete commands[i];
    }
    delete edit;
    delete RunWindow::instance;
}
