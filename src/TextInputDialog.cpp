#include "TextInputDialog.h"
#include <windows.h>
#include <string>
#include "Window.h"
#define TID_OK 1
#define TID_OKCANCEL 0

TextInputDialog::TextInputDialog(std::string title, std::string prompt, int type, char* buffer, int bufLength)
{
    //char szClassName[30];
    //strcpy(&szClassName[0], "TextInputDialog");

    //int winHeight = 300;
    //int winWidth = 200;

    //this->nCmdShow = nCmdShow;
    /* The Window structure */
    //wc.hInstance = hThisInstance;
    //wc.lpszClassName = &szClassName[0];
    //wc.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    //wc.style = CS_DBLCLKS;                 /* Catch double-clicks */
    //wc.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    //wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    //wc.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    //wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    //wc.lpszMenuName = NULL;                 /* No menu */
    //wc.cbClsExtra = 0;                      /* No extra bytes after the window class */
    //wc.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    //wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

    /* Register the window class, and if it fails quit the program */
    //if (!RegisterClassExA (&wc))
    //{
        //std::cout << GetLastError() << std::endl;
      //  exit(-1);
    //}
    /* The class is registered, we can now create the window.*/
    //hDialog = CreateWindowEx (
    //       0,                   /* Extended possibilites for variation */
    //       &szClassName[0],         /* Classname */
    //       title,               /* Title Text */
    //       WS_SYSMENU,          // No resize
    //       CW_USEDEFAULT,       /* Windows decides the position */
    //       CW_USEDEFAULT,       /* where the window ends up on the screen */
    //       winWidth,  /* The programs width */
    //       winHeight,  /* and height in pixels */
    //       HWND_DESKTOP,        /* The window is a child-window to desktop */
    //       NULL,                /* No menu */
    //       wc.hInstance,     /* Program Instance handler */
    //       NULL                 /* No Window Creation data */
    //       );

    /* Make the window visible on the screen */
    //ShowWindow (hFrame, nCmdShow);

    //prompt = new Window(&hDialog, "STATIC", prompt.c_str(), WS_CHILD | WS_VISIBLE, 10, 10, 290, 40, 0);
    //ok =
    //if (type == TID_OKCANCEL)
    //{

    //}
}

TextInputDialog::~TextInputDialog()
{
    DestroyWindow(hDialog);
}
