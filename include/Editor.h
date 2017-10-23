#include <windows.h>
#ifndef EDITOR_H
#define EDITOR_H
#include "LineOfText.h"
#include "Window.h"
#include <string>
#include <vector>
#define numLines 1000
#define FILE_PATH_LENGTH 2048
#define FILE_TITLE_LENGTH 512

class Editor
{
    public:
        Editor(HWND* parent, HINSTANCE *hInstance, double winx, double winy, int bHeight);
        virtual ~Editor();
        void addInstruction(std::string cmd);
        void deleteLine();
        static Editor* instance;
        void saveFile();
        void loadFile();
        OPENFILENAME file;
        bool validInstruction(const char *instruction);
        std::vector <LineOfText> getLines();
        bool errorCheck();
        static WNDPROC stdEditProc;
        HWND editWindow;
    protected:
    private:
        std::vector<LineOfText> lines;
        Window* headings[3];
        void getTableContents();
        void setTableContents();
        int ax,ay; // the x and y of the last active cell
        double winx, winy, ex, ey;
        int numVisible;
        HWND* parent;
        void initLines();
        int scrollPosition;
        HWND scrollBar;
        void processScrolling(WPARAM wParam);
        SCROLLINFO si;
        int rowHeight;
        std::vector<Window*> hNumbers;
        std::vector<Window*> hInstruction;
        std::vector<Window*> hAddress;
        static LRESULT CALLBACK EditWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // handles messages relating to the table
        static LRESULT APIENTRY RSEditBoxProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void createTable();
        char filePath[FILE_PATH_LENGTH];
        char fileTitle[FILE_TITLE_LENGTH];
        void setFileTitle(std::string title);
};

#endif // EDITOR_H
