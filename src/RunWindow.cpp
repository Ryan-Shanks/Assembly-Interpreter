#include <windows.h>
#include "RunWindow.h"
#include "GUI.h"
#include "Window.h"
#include "StringToNumber.h"
#include "LineOfText.h"
#include <iostream>
RunWindow* RunWindow::instance = NULL;

LRESULT CALLBACK RunWindow::RunWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_COMMAND)
    {
        switch (LOWORD(wParam))
        {
        case 1: // step button was pressed
            instance->step();
            break;
        case 2:
            instance->run();
            break;
        case 3: // submit
            {
                char input[30];
                int val;
                GetWindowText(instance->num->thisWindow, &input[0], 30);
                //std::cout << "text: " << input << std::endl;
                if (strint(input, &val))
                {
                    instance->reg = val;
                    EnableWindow(instance->prompt->thisWindow, FALSE);
                    EnableWindow(instance->num->thisWindow, FALSE);
                    EnableWindow(instance->submit->thisWindow, FALSE);
                    EnableWindow(instance->runB->thisWindow, TRUE);
                    EnableWindow(instance->stepB->thisWindow, TRUE);
                    instance->updateDisplay();
                    SetWindowText(instance->num->thisWindow, "");
                }
                else
                {
                    ::MessageBeep(MB_ICONERROR);
                    ::MessageBox(instance->hFrame, "Please enter an integer between -1,000,000 and 1,000,000", "Error", MB_OK);
                }
            }
        case 4://stop
           instance->stopProgram = true;
           break;
        }
    }
    else if (uMsg == WM_CLOSE)
    {
        instance->stopProgram = true;
        instance->setVisible(false);
    }
    else if (uMsg == WM_VSCROLL)
    {
        instance->processScrolling(wParam);
    }
    else
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
void RunWindow::setVisible(bool visible)
{
    if (visible)
        SetWindowPos(hFrame, HWND_DESKTOP,0,0, WIN_WIDTH, WIN_HEIGHT, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW | SWP_NOREPOSITION);
    else
        SetWindowPos(hFrame, HWND_DESKTOP,0,0,WIN_WIDTH, WIN_HEIGHT, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_HIDEWINDOW | SWP_NOREPOSITION);
}
RunWindow::RunWindow(HINSTANCE hThisInstance, MSG *messages)
{
    WNDCLASS test; // ignored
    if (GetClassInfo(hThisInstance, "RunWindowClass", &test) == 0) // check if the window class needs to be registered
    {
        strcpy(&winClassName[0], "RunWindowClass");

        /* The Window structure */
        wincl.hInstance = hThisInstance;
        wincl.lpszClassName = &winClassName[0];
        wincl.lpfnWndProc = RunWindow::RunWinProc;      /* This function is called by windows */
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
    }
    hFrame = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           &winClassName[0],         /* Classname */
           "Run Window",        /* Title Text */
           WS_SYSMENU | WS_MINIMIZEBOX,          // no resize allowed
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           WIN_WIDTH,          /* The programs width */
           WIN_HEIGHT,          /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           wincl.hInstance,     /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    strcpy(&regStr[0], "Register: ");
    stepB = new Window (&hFrame, "BUTTON", "Step", 10, 10, 175, 20, 1);
    runB = new Window (stepB, "BUTTON", "Run", 0, 40, 2);
    prompt = new Window (&hFrame, "STATIC", "Please enter a number: ", 10, 100, 175, 20, 0);
    num = new Window (&hFrame, "EDIT", "", 10, 120, 100, 20, 0);
    submit = new Window(&hFrame, "BUTTON", "Submit", 110, 120, 75, 20, 3);
    regVal = new Window(&hFrame, "STATIC", &regStr[0], 10, 150, 175, 20, 0);
    stop = new Window (regVal, "Button", "Stop", 0, 40, 4);

    numVisible = (WIN_HEIGHT / 20) - 2; // 20 is the height of each row
    initLines();
    EnableWindow(prompt->thisWindow, FALSE);
    EnableWindow(num->thisWindow, FALSE);
    EnableWindow(submit->thisWindow, FALSE);
    msgPtr = messages;

    scrollBar = CreateWindow("SCROLLBAR", (LPSTR)NULL,
                             WS_VISIBLE | WS_CHILD | SBS_VERT,
                             350, 10, 20, numVisible * 20,
                             hFrame, (HMENU)4, hThisInstance, NULL);
    SetScrollRange(scrollBar, SB_CTL, 0, numLines-numVisible +1, TRUE);
    EnableScrollBar(scrollBar, SB_CTL, ESB_ENABLE_BOTH);
    scrollPosition = 0;
}
void RunWindow::initLines()
{
    hNumbers.clear();
    hMemory.clear();
    char tmp[30];
    hNumbers.push_back(new Window(&hFrame, "STATIC", "Line #:", 200, 10, 50, 20, 0));
    hMemory.push_back(new Window(&hFrame, "STATIC", "Memory Contents:", 250, 10, 100, 20, 0));
    for (int i = 1; i < numVisible; i++)
    {
        itoa(i+1, tmp, 10);
        hNumbers.push_back(new Window(hNumbers[i-1], "STATIC", &tmp[0], 0, 20, 0));
        hMemory.push_back(new Window(hMemory[i-1],"STATIC", "", 0, 20, 0));
    }
}

void RunWindow::compile(std::vector<LineOfText> lines)
{
    currentLine = 0;
    memorySize = lines.size();
    memory = new int[memorySize];
    ZeroMemory(memory, memorySize * sizeof(int));
    for (int i =0; i < memorySize; i++)
    {
        if (strcmp(lines[i].getInstruction(), "") == 0){memory[i] = 0;}
        else if (strcmp(lines[i].getInstruction(), "READ") == 0){memory[i] = 1 * MAX_INT;}
        else if (strcmp(lines[i].getInstruction(), "LOAD") == 0){memory[i] = 2* MAX_INT;}
        else if (strcmp(lines[i].getInstruction(), "STORE") == 0){memory[i] = 3* MAX_INT;}
        else if (strcmp(lines[i].getInstruction(), "ADD") == 0){memory[i] = 4* MAX_INT;}
        else if (strcmp(lines[i].getInstruction(), "SUBTRACT") == 0){memory[i] = 5* MAX_INT;}
        else if (strcmp(lines[i].getInstruction(), "BRANCH") == 0){memory[i] = 6* MAX_INT;}
        else if (strcmp(lines[i].getInstruction(), "BRANCHZ") == 0){memory[i] = 7* MAX_INT;}
        else if (strcmp(lines[i].getInstruction(), "BRANCHP") == 0){memory[i] = 8* MAX_INT;}
        else if (strcmp(lines[i].getInstruction(), "PRINT") == 0){memory[i] = 9* MAX_INT;}
        else if (strcmp(lines[i].getInstruction(), "STOP") == 0){memory[i] = 10 * MAX_INT;} // same as stop, in either case program will halt
        memory[i] += lines[i].getAddress();
        updateDisplay();
    }
}
void RunWindow::updateDisplay()
{
    itoa(reg, &regStr[10], 10); // starting at index 9 because before that is 'register: '
    SetWindowText(regVal->thisWindow, &regStr[0]);
    char tmp[40];
    for (int i =1; i < numVisible; i ++)
    {
        if (i-1 + scrollPosition == currentLine)
        {
            strcpy(&tmp[0], "->");
            itoa(i + scrollPosition -1, &tmp[2], 10);
            SetWindowText(hNumbers[i]->thisWindow, &tmp[0]);
        }
        else
        {
            itoa(i + scrollPosition -1, &tmp[0], 10);
            SetWindowText(hNumbers[i]->thisWindow, &tmp[0]);
        }
        itoa(memory[i + scrollPosition -1], &tmp[0], 10);
        SetWindowText(hMemory[i]->thisWindow, &tmp[0]);
    }
}
void RunWindow::processScrolling(WPARAM wParam)
{
    if (LOWORD(wParam) != SB_ENDSCROLL) // dont process endscroll messages
    {
        switch (LOWORD(wParam)) // decide how much to adjust scrollPosition by
        {
        case SB_TOP:
                scrollPosition = 0;
                break;

        case SB_LINEUP:
                if (scrollPosition > 0)
                        scrollPosition--;
                break;

        case SB_THUMBPOSITION:
                scrollPosition = HIWORD(wParam);
                break;

        case SB_THUMBTRACK:
                scrollPosition = HIWORD(wParam);
                break;

        case SB_LINEDOWN:
                if (scrollPosition < numLines - numVisible)
                        scrollPosition++;
                break;

        case SB_BOTTOM:
                scrollPosition = numLines - numVisible;
                break;
        }
    }
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    si.nPos = scrollPosition;
    SetScrollInfo(scrollBar, SB_CTL, &si, TRUE); // update the scrollbar

    //Now we must process the text scrolling manually, because I cant figure out any more elegant way
    updateDisplay();
}
void RunWindow::run()
{
    int ins;
    stopProgram = false;
    do
    {
        if (PeekMessage(msgPtr, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(msgPtr);
            if (DispatchMessage(msgPtr) == -10)
            {
                break;
            }
        }
        ins = memory[currentLine] / MAX_INT;
        step();
    }while (((ins > 1 && ins < 11) || ins == 14) && !stopProgram);
}
void RunWindow::step() // executes the current instruction
{
    if (currentLine >= numLines)
    {
        currentLine --;
        STOP();
    }
    //get instruction
    int ins = memory[currentLine] / MAX_INT;
    //std::cout << "Step: " << currentLine << " Reg: " << reg << "Memory: " << memory[currentLine] << std::endl;
    switch(abs(ins))
    {
    case 1:
        READ();
        break;
    case 2:
        LOAD();
        break;
    case 3:
        STORE();
        break;
    case 4:
        ADD();
        break;
    case 5:
        SUBTRACT();
        break;
    case 6:
        BRANCH();
        break;
    case 7:
        BRANCHZ();
        break;
    case 8:
        BRANCHP();
        break;
    case 9:
        PRINT();
        break;
    case 14:
        BEEP();
        break;
    default:
        STOP();
    }
    currentLine ++;
    if (currentLine < scrollPosition)
    {
        scrollPosition = currentLine;
        processScrolling(SB_ENDSCROLL);
    }
    else if (currentLine > scrollPosition + numVisible -2)
    {
        scrollPosition = currentLine - numVisible + 2;
        processScrolling(SB_ENDSCROLL);
    }
    updateDisplay();
}
    void RunWindow::STOP()
    {
        if (memory[currentLine] / MAX_INT == 10)
        {
            ::MessageBeep(MB_ICONINFORMATION);
            ::MessageBox(hFrame, "Run completed successfully", "Info", MB_OK);
        }
        else
        {
            ::MessageBeep(MB_ICONERROR);
            ::MessageBox(hFrame,"Program terminated unexpectedly", "Error", MB_OK);
        }
        currentLine = -1;
        stopProgram = true;
    }
    void RunWindow::READ() // take integer from textbox and store in register
    {
        //std::cout << "Reading a line" << std::endl;
        ::MessageBeep(0xFFFFFFFF);
        EnableWindow(prompt->thisWindow, TRUE);
        EnableWindow(num->thisWindow, TRUE);
        EnableWindow(submit->thisWindow, TRUE);
        EnableWindow(stepB->thisWindow, FALSE);
        EnableWindow(runB->thisWindow, FALSE);
        SetFocus(num->thisWindow);
    }
    void RunWindow::LOAD() // copy addressed line into reg
    {
        if (validAddress(getPayload()))
            reg = memory[getPayload()];
        else
            STOP();
    }
    void RunWindow::STORE()
    {
        if (validAddress(memory[currentLine] % MAX_INT))
            memory[getPayload()] = reg;
        else
            STOP();
    }
    void RunWindow::ADD()
    {
        if (validAddress(getPayload()))
        {
            reg += memory[getPayload()];
        }
        else
        {
            STOP();
        }
    }
    void RunWindow::SUBTRACT()
    {
        if (validAddress(getPayload()))
        {
            reg -= memory[getPayload()];
        }
        else
        {
            STOP();
        }
    }
    void RunWindow::BRANCH()
    {
        if (validAddress(getPayload()))
            currentLine = getPayload() -1; // -1 because currentLine ++ at the end of step()
        else
            STOP();
    }
    void RunWindow::BRANCHZ() //jump to specified address when the register is zero
    {
        if (reg == 0)
        {
            BRANCH();
        }
    }
    void RunWindow::BRANCHP() // jump to specified address when the register is positive
    {
        if (reg > 0)
        {
            BRANCH();
        }
    }
    void RunWindow::PRINT() // print the number in the register
    {
        char out[30];
        itoa(reg, &out[0], 10);
        ::MessageBeep(0xFFFFFFFF); // simple beep
        ::MessageBox(hFrame, &out[0], "Program output:", MB_OK);
    }
    void RunWindow::BEEP()
    {
        if (getPayload() > 32 && getPayload() < 22000)
        {
            Beep(getPayload(), 1000);
        }
    }

    bool RunWindow::validAddress(int address) // check if address is within range of memory
    {
        return address >= 0 && address < memorySize;
    }
    int RunWindow::getPayload() // returns the current line with the command digit chopped off
    {
        return memory[currentLine] % MAX_INT;
    }

RunWindow::~RunWindow()
{
    for (int i =0; i < numVisible; i++)
    {
        delete hNumbers[i];
        delete hMemory[i];
    }
    DestroyWindow(hFrame);
    delete stepB;
    delete runB;
    delete prompt;
    delete num;
    delete submit;
    delete regVal;
    delete[] memory;
}
