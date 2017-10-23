#include <windows.h>
#include <cstring>
#include <string>
#include <iostream>
#include <cstdlib>
#include "Editor.h"
#include "Window.h"
#include <fstream>
#include <sstream>
#include "StringToNumber.h"


Editor* Editor::instance = NULL;

WNDPROC Editor::stdEditProc = NULL;

LRESULT CALLBACK Editor::EditWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) // message handler, static
{
    if (instance != NULL)
    {
        if (uMsg == WM_COMMAND)
        {
            /*
            NUMBER FORMAT FOR LOWORD(wParam):
            0 is the edit window itself
            1 is the scrollbar, it should be handled by processScrolling()
            otherwise, right most digit is the column (1 for command, 2 for address), and digits to the left are the row.
            NOTE that the row is not the line number, but rather the number of rows from the top of the visible area, indexed from 1
            */
            if (LOWORD(wParam) > 10)
            {
                instance->ay = LOWORD(wParam) / 10 - 1;
                instance->ax = LOWORD(wParam) % 10;
                //std::cout << instance->ax << ", " << instance->ay << std::endl;
            }
        }
        else if (uMsg == WM_VSCROLL)
        {
            instance->processScrolling(wParam);
        }
        else if (uMsg == WM_KEYDOWN)
        {
            if (wParam == VK_TAB) // tab key was pressed
            {
                if (instance->ax == 1) // current active cell is an instruction cell
                {
                    instance->ax = 2; // move over to the address
                    SetFocus(instance->hAddress[instance->ay]->thisWindow);
                    SendMessage(instance->hAddress[instance->ay]->thisWindow, EM_SETSEL, 0, -1);
                }
                else if (instance->ax == 2 && instance->ay >= instance->numVisible-1)
                {
                    instance->processScrolling(SB_LINEDOWN);
                    instance->ax = 1;
                    SetFocus(instance->hInstruction[instance->ay]->thisWindow);
                    SendMessage(instance->hInstruction[instance->ay]->thisWindow, EM_SETSEL, 0, -1);
                }
                else
                {
                    instance->ay ++;
                    instance->ax = 1;
                            SetFocus(instance->hInstruction[instance->ay]->thisWindow);
                    SendMessage(instance->hInstruction[instance->ay]->thisWindow, EM_SETSEL, 0, -1);
                }
            }
            else if (wParam == VK_SPACE)
            {
                if (instance->ax == 1)
                {
                    instance->ax =2;
                    SetFocus(instance->hAddress[instance->ay]->thisWindow);
                    SendMessage(instance->hAddress[instance->ay]->thisWindow, EM_SETSEL, 0, -1);
                }
            }
            else if (wParam == VK_RETURN || wParam == VK_DOWN)
            {
                //std::cout << "Down" << std::endl;
                if (instance->ay >= instance->numVisible - 1)
                {
                    if (instance->scrollPosition < numLines - instance->numVisible)
                    {
                        instance->processScrolling(SB_LINEDOWN);
                        instance->ay --;
                        //std::cout << "scrolling" << std::endl;
                    }
                    else
                    {
                        ::MessageBeep(0xFFFFFFFF);
                        return 0;
                    }
                }
                instance->ay++;
                if (instance->ax == 1)
                {
                    SetFocus(instance->hInstruction[instance->ay]->thisWindow);
                }
                else
                    SetFocus(instance->hAddress[instance->ay]->thisWindow);
            }
            else if (wParam == VK_UP)
            {
                if (instance->ay == 0)
                {
                    if (instance->scrollPosition > 0)
                    {
                        instance->processScrolling(SB_LINEUP);
                    }
                    else
                    {
                        ::MessageBeep(0xFFFFFFFF);
                    }
                }
                else
                {
                    instance->ay--;
                    if (instance->ax == 1)
                        SetFocus(instance->hInstruction[instance->ay]->thisWindow);
                    else
                    SetFocus(instance->hAddress[instance->ay]->thisWindow);
                }
            }
            else if (wParam == VK_RIGHT)
            {
                if (instance->ax == 1)
                {
                    instance->ax = 2;
                    SetFocus(instance->hAddress[instance->ay]->thisWindow);
                }
                else
                {
                    ::MessageBeep(0xFFFFFFFF);
                }
            }
            else if (wParam == VK_LEFT)
            {
                if (instance->ax == 2)
                {
                    instance->ax = 1;
                    SetFocus(instance->hInstruction[instance->ay]->thisWindow);
                }
                else
                {
                    ::MessageBeep(0xFFFFFFFF);
                }
            }
            return 0;
        }
        else if (uMsg == WM_MOUSEWHEEL)
        {
            wParam -= LOWORD(wParam);
            instance->getTableContents();
            int scroll = - GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            if (instance->scrollPosition + scroll > 0 && instance->scrollPosition + scroll < numLines - instance->numVisible)
            {
                instance->scrollPosition+= scroll;
            }
            else if (instance->scrollPosition + scroll <= 0)
            {
                instance->scrollPosition = 0;
            }
            else if(instance->scrollPosition + scroll >= numLines - instance->numVisible)
            {
                instance->scrollPosition = numLines - instance->numVisible;
            }
            instance->processScrolling(SB_ENDSCROLL);
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK Editor::RSEditBoxProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_KEYDOWN)
    {
        WORD key = LOWORD(wParam);
        if (key == VK_TAB || key == VK_RETURN || key == VK_UP || key == VK_DOWN || key == VK_RIGHT || key == VK_LEFT || key == VK_SPACE)
        {
            SendMessage(GetParent(hwnd), WM_KEYDOWN, wParam, lParam);
            return 0;
        }
    }
    else if (msg == WM_CHAR) // discard tab and other navigation keystrokes to supress beep
    {
        WORD key = LOWORD(wParam);
        if (key == VK_TAB || key == VK_RETURN || key == VK_UP || key == VK_DOWN || key == VK_RIGHT || key == VK_LEFT || key == VK_SPACE)
        {
            return 0;
        }
    }
    return CallWindowProc(Editor::stdEditProc, hwnd, msg, wParam, lParam);
}
void Editor::createTable()
{
    ax = 0;
    ay = 0;
    scrollPosition = 0;
    numVisible = (int) ((winy * 0.8) / rowHeight) - 1; // figure out how many lines we need to draw
    headings[0] = new Window(&editWindow, "STATIC", "#:", 0, 0, ex * 0.1, rowHeight, 0);
    headings[1] = new Window(&editWindow, "STATIC", "Instruction:", ex * 0.1, 0, ex * 0.425, rowHeight, 0);
    headings[2] = new Window(&editWindow, "STATIC", "Address:", ex * 0.525, 0, ex * 0.425, rowHeight, 0);
    //clear the window* vectors
    hNumbers.clear(); hInstruction.clear(); hAddress.clear();
    // used to store windows while we modify their procedures. Shouldnt be nessessary but Im trying to find the bug
    Window *numbers, *instruction, *address;
    //Create first row of windows
    numbers = new Window(&editWindow, "STATIC", "0", 0, rowHeight, ex * 0.1, rowHeight, 10);
    instruction = new Window(&editWindow, "EDIT", lines[0].getInstruction(), WS_BORDER | WS_VISIBLE | WS_CHILD | ES_UPPERCASE, ex * 0.1, rowHeight , ex * 0.425, rowHeight, 11);
    address = new Window(&editWindow, "EDIT", lines[0].getAddressStr(), WS_BORDER | WS_VISIBLE | WS_CHILD | ES_NUMBER, ex * 0.525, rowHeight, ex * 0.425, rowHeight, 12);

    //set custom edit procedures
    #ifdef STRICT //something microsoft suggested... It seems strict is defined by default though
        Editor::stdEditProc = (WNDPROC)SetWindowLongPtr(instruction->thisWindow, GWLP_WNDPROC, (LONG)RSEditBoxProc);
    #else
        Editor::stdEditProc = (FARPROC)SetWindowLongPtr(instruction->thisWindow, GWLP_WNDPROC, (LONG)RSEditBoxProc);
    #endif // STRICT
    SetWindowLongPtr(address->thisWindow, GWLP_WNDPROC, (LONG)RSEditBoxProc);
    SendMessage(instruction->thisWindow, EM_LIMITTEXT, (WPARAM) 8, 0); // never needs to enter more than 8 characters
    SendMessage(address->thisWindow, EM_LIMITTEXT, (WPARAM) 7, 0); // limit the size of the number the user can enter
    //store in vectors... nothing gets into the vectors without having its custom edit proc other than numbers, no custom proc for them
    hNumbers.push_back(numbers);
    hInstruction.push_back(instruction);
    hAddress.push_back(address);
    //create all the line objects...
    for (int i =1; i < numVisible; i++)
    {
        //prepare text for line number windows
        char tmp[6];
        itoa(i, &tmp[0], 10);
        // create each row
        numbers = new Window(hNumbers[i-1], "STATIC", tmp, 0, rowHeight, 10 * (i+1));
        instruction = new Window(hInstruction[i-1], "EDIT", lines[i].getInstruction(), 0, rowHeight,10*(i+1)+1);
        address = new Window(hAddress[i-1], "EDIT", lines[i].getAddressStr(), 0, rowHeight, 10*(i+1)+2);
        SendMessage(instruction->thisWindow, EM_LIMITTEXT, (WPARAM) 8, 0);
        SendMessage(address->thisWindow, EM_LIMITTEXT, (WPARAM) 7, 0);
        //set the custom wndproc
        SetWindowLongPtr(instruction->thisWindow, GWLP_WNDPROC, (LONG)RSEditBoxProc);
        SetWindowLongPtr(address->thisWindow, GWLP_WNDPROC, (LONG)RSEditBoxProc);

        //add to vectors
        hNumbers.push_back(numbers);
        hInstruction.push_back(instruction);
        hAddress.push_back(address);
    }
}
void Editor::initLines()
{
    lines.clear();
    lines.reserve(numLines);
    for (int i=0; i < numLines; i++)
    {
        lines.push_back(LineOfText());
    }
}

void Editor::processScrolling(WPARAM wParam)
{
    int prev_ax = ax;
    int prev_ay = ay;
    if (LOWORD(wParam) != SB_ENDSCROLL) // dont process endscroll messages
    {
        getTableContents(); // overwrite lines in memory with data in table
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
    si.fMask  = SIF_POS;
    si.nPos   = scrollPosition;
    SetScrollInfo(scrollBar, SB_CTL, &si, TRUE); // update the scrollbar

    //Now we must process the text scrolling manually, because I cant figure out any more elegant way
    setTableContents(); // write data tp the table that has been stored in memory
    ax = prev_ax;
    ay = prev_ay;
}

Editor::Editor(HWND* parent,HINSTANCE *hInstance, double winx, double winy, int bHeight)
{
    rowHeight = bHeight;
    this->parent = parent;
    this->winx = winx;
    this->winy = winy;
    ex = winx * 0.675; // the width of the edit window
    ey = winy * 0.8; // the height of the edit window
    editWindow = CreateWindow("STATIC", (LPSTR)NULL,
                              WS_VISIBLE | WS_CHILD | WS_BORDER,
                              (int) winx * 0.3, rowHeight, (int)ex, (int) ey,
                              *parent, NULL, NULL, NULL);
    scrollBar = CreateWindow("SCROLLBAR", (LPSTR)NULL,
                             WS_VISIBLE | WS_CHILD | SBS_VERT,
                             (int) ex * 0.95, 0, (int) ex * 0.05, (int) ey,
                             editWindow, (HMENU)1, *hInstance, NULL);
    // now that all the main windows are created we must create a table of smaller windows on top of them.

    initLines();
    createTable();
    SetWindowLongPtr(editWindow, GWLP_WNDPROC, (LONG) &Editor::EditWinProc);

    scrollPosition = 0;
    SetScrollRange(scrollBar, SB_CTL, 0, numLines-numVisible, TRUE);
    EnableScrollBar(scrollBar, SB_CTL, ESB_ENABLE_BOTH);

    GetScrollInfo(scrollBar, SB_CTL, &si);

    ZeroMemory(filePath, sizeof(filePath));
    ZeroMemory(fileTitle, sizeof(fileTitle));
    ZeroMemory(&file, sizeof(file));
    file.lStructSize = sizeof(file);
    file.hwndOwner = *parent;
    file.hInstance = NULL;
    file.lpstrFile = &filePath[0];
    file.lpstrFile[0] = 0;
    file.nMaxFile = FILE_PATH_LENGTH;
    file.lpstrFilter = "Turassic Enhanced Assembly\0*.tea\0All\0*.*\0";
    file.nFilterIndex = 1;
    file.lpstrFileTitle = &fileTitle[0];
    file.nMaxFileTitle = FILE_TITLE_LENGTH;
    file.lpstrInitialDir = NULL;
    file.Flags = OFN_PATHMUSTEXIST;
}
void Editor::getTableContents()
{
    char tmp[MAX_INSTRUCTION_LENGTH];
    for (int i =0; i < numVisible; i++)
    {
        GetWindowText(hInstruction[i]->thisWindow, &tmp[0], MAX_INSTRUCTION_LENGTH);
        lines[i + scrollPosition].modifyInstruction(&tmp[0]);
        GetWindowText(hAddress[i]->thisWindow, &tmp[0], MAX_INSTRUCTION_LENGTH);
        lines[i + scrollPosition].modifyAddress(&tmp[0]);
    }
}
void Editor::setTableContents()
{
    char tmp[8];
    for (int i =0; i < numVisible; i++)
    {
        itoa(i + scrollPosition, &tmp[0], 10);
        SetWindowText(hNumbers[i]->thisWindow, &tmp[0]);
        SetWindowText(hInstruction[i]->thisWindow, lines[i+scrollPosition].getInstruction());
        SetWindowText(hAddress[i]->thisWindow, lines[i+scrollPosition].getAddressStr());
    }
}
void Editor::setFileTitle(std::string title)
{
    std::string tmp = "Turassic Enhanced LMC";
    if (title.compare("") != 0)
    {
        tmp += " - " + title;
    }
    SetWindowText(*parent, tmp.c_str());
}

void Editor::saveFile()
{
    getTableContents();
    if (file.lpstrFile[0] == 0)
    {
        if (GetSaveFileName(&file) != 0 )
        {
            if (strcmp(&file.lpstrFile[strlen(file.lpstrFile) - 4], ".tea") != 0) // ensure file ends in .tea
            {
                //ensure buffers are long enough to add the extension
                if (strlen(file.lpstrFile) < FILE_PATH_LENGTH - 4)
                {
                    strcpy(&file.lpstrFile[strlen(file.lpstrFile)], ".tea");
                }
                if (strlen(file.lpstrFileTitle) < FILE_TITLE_LENGTH - 4)
                {
                    strcpy(&file.lpstrFileTitle[strlen(file.lpstrFileTitle)], ".tea");
                }
            }
            write:
            std::ofstream fout;
            std::cout << "saving file|" << file.lpstrFile <<"|" << std::endl;
            fout.open(file.lpstrFile);
            if (fout.good()) // cant open or write the file
            {
                getTableContents();
                for (int i =0; i < numLines; i++)
                {
                    if (strcmp(lines[i].getInstruction(), "") != 0 || lines[i].getAddress() != 0) // if comamnd isnt empty or address isnt 0
                    {
                        if (strcmp(lines[i].getInstruction(), "") == 0)
                        {
                            fout << i << " - " << lines[i].getAddress() << std::endl;
                        }
                        else
                        {
                            fout << i << " " << lines[i].getInstruction() << " " << lines[i].getAddress() << std::endl;
                        }
                    }
                }
                setFileTitle(file.lpstrFileTitle);
            }
            else
            {
                ::MessageBeep(MB_ICONERROR);
                ::MessageBox(*parent, "Error", "Cannot write file. Please try another directory.", MB_OK);
            }
            fout.close();
        }
        else
        {
            std::cout <<"Error " << CommDlgExtendedError() << std::endl;
        }
    }
    else
    {
        goto write; // just this once
    }
}
bool Editor::validInstruction(const char *instruction)
{
    return (strcmp(instruction, "") == 0
            || strcmp(instruction, "READ")== 0
            || strcmp(instruction, "LOAD")== 0
            || strcmp(instruction, "STORE")== 0
            || strcmp(instruction, "BRANCH")== 0
            || strcmp(instruction, "BRANCHZ")== 0
            || strcmp(instruction, "BRANCHP")== 0
            || strcmp(instruction, "PRINT")== 0
            || strcmp(instruction, "ADD")== 0
            || strcmp(instruction, "SUBTRACT")== 0
            || strcmp(instruction, "STOP")== 0);
    return true;
}
void Editor::loadFile()
{
    if (GetOpenFileName(&file) != 0)
    {
        std::ifstream fin;
        fin.open(file.lpstrFile);
        std::string test, lineStr, commandStr, addrStr;
        int lineNumber = 0;
        int address;
        if (fin.good())
        {
            while (!fin.eof())
            {

                std::getline(fin, test);
                std::stringstream ss(test);
                ss >> lineStr;
                if (strintNonNeg(lineStr, &lineNumber) && test.compare("") != 0) // line number is valid and test isnt a blank line
                {
                    if (lineNumber > numLines)
                    {
                        ::MessageBeep(MB_ICONERROR);
                        ::MessageBox(*parent, "File is corrupt; program supports a maximum of numLines lines. Some lines may not be read.", "Error", MB_ICONERROR);
                    }
                    else
                    {
                        //std::cout << "Line: " << lineNumber << " Command: ";
                        ss >> commandStr; // get the command, "-" is a placeholder for no command
                        //std::cout << commandStr << " address: ";
                        if (commandStr.compare("-") != 0)
                        {
                            lines[lineNumber].modifyInstruction(commandStr.c_str());
                        }
                        ss >> addrStr; // read the address
                        //std::cout << addrStr << std::endl;
                        if (strintNonNeg(addrStr, &address))
                        {
                            lines[lineNumber].modifyAddress(address);
                        }
                    }
                }
                else if (lineNumber > numLines || lineNumber < 0)// line number is invalid, user probably attempted to edit the file outside of application
                {
                    ::MessageBeep(MB_ICONERROR);
                    ::MessageBox(*parent, "File contains corrupt line. Some lines may be missing.", "Error", MB_OK);
                }
            }
            setFileTitle(file.lpstrFileTitle);
        }
        else
        {
            ::MessageBeep(MB_ICONERROR);
            ::MessageBox(*parent, "Cannot read file", "Error", MB_OK);
        }
        fin.close();
    }
    else
    {
        std::cout << CommDlgExtendedError() << std::endl;
    }
    setTableContents();
}

void Editor::addInstruction(std::string cmd) //The instruction is placed in the active line's instruction cell. @param instructionNum the number of the button that was pressed, between 0 and 9
{
    //if a cell in Address has focus, edit the next line, otherwise edit the current line
    if (ax == 2)
    {
        ay++; // move down a row
        if (ay > numVisible-1)
        {
            processScrolling(SB_LINEDOWN);
            ay--;
        }
        SetWindowText(hInstruction[ay]->thisWindow, cmd.c_str());
        SetWindowText(hAddress[ay]->thisWindow, ""); // overwrite the address which is about to be re-entered
        SetFocus(hAddress[ay]->thisWindow); // put keyboard focus on the address field
    }
    else
    {
        SetWindowText(hInstruction[ay]->thisWindow, cmd.c_str());
        SetWindowText(hAddress[ay]->thisWindow, ""); // overwrite the address which is about to be re-entered
        SetFocus(hAddress[ay]->thisWindow);
        ax = 2;
    }
}
void Editor::deleteLine()
{
    SetWindowText(hInstruction[ay]->thisWindow, "");
    SetWindowText(hAddress[ay]->thisWindow, "0");
}
bool Editor::errorCheck()
{
    getTableContents();
    bool good = true;
    std::stringstream badLines;
    badLines.clear();
    badLines << "The following lines contain errors:\n";
    for (int i =0; i < numLines; i++)
    {
        if (!validInstruction(lines[i].getInstruction()))
        {
            good = false;
            badLines << i << ", ";
        }
    }
    if (!good) // prompt and dont compile
    {
        ::MessageBeep(MB_ICONERROR);
        ::MessageBox(editWindow, badLines.str().c_str(), "Compilation failed", MB_OK);
    }
    return good;
}
std::vector<LineOfText> Editor::getLines()
{
    getTableContents();
    return lines;
}
Editor::~Editor()
{
    DestroyWindow(scrollBar);
    DestroyWindow(editWindow);
    delete headings[0]; delete headings[1]; delete headings[2];
    for (unsigned int i =0; i < hNumbers.size(); i++){delete hNumbers[i];}
    for (unsigned int i =0; i < hAddress.size(); i++){delete hAddress[i];}
    for (unsigned int i =0; i < hInstruction.size(); i++){delete hInstruction[i];}
}
