#ifndef TECXTINPUTDIALOG_H
#define TECXTINPUTDIALOG_H
#include <windows.h>
#include <string>
#include "Window.h"
#define TID_OK 1
#define TID_OKCANCEL 0


class TextInputDialog
{
    public:
        TextInputDialog(std::string title, std::string prompt, int type, char* buffer, int bufLength);
        virtual ~TextInputDialog();
    protected:
    private:
    WNDCLASSEX wc;
    HWND hDialog;
    char* buffer;
    int type;
    int bufLength;
    Window* prompt, *ok, *cancel, *field;
    int nCmdShow;
};

#endif // TECXTINPUTDIALOG_H
