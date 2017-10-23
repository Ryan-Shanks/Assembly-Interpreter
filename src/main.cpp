/**
Program Name: Turassic Enhanced LMC
Language: C++
Required Dynamic Libraries: user32, kernel32, comctl32, Commdlg32 **You need to link these to build the project
    Located in project settings if youre using code::blocks**
Known Bugs: none
Purpose: To introduce students or other intrested parties to assembly programming, within a base 10 environment and only
    10 instructions (11 if you count the easter egg). This program lacks some of the limitations of the one produced by YorkU
    and also strays slightly from the design of the original LMC (namely it has 10x as much memory and can store larger numbers)
**/
#include <windows.h>
#include "GUI.h"
#include <iostream>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

GUI* window = NULL;

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    window = new GUI(hThisInstance, nCmdShow, WindowProcedure);

    /* Run the message loop. It will run until GetMessage() returns 0 */

    while (GetMessage (&window->messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&window->messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&window->messages);
    }

    delete window;
    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return window->messages.wParam;
}
/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_COMMAND:
            std::cout << (LOWORD(wParam)) << std::endl;
            window->handleButtonPress(LOWORD(wParam)); // handle the action within the GUI
            break;
        case WM_CLOSE:
            window->saveFilePrompt();
            return DefWindowProc(hwnd, message, wParam, lParam);
            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
