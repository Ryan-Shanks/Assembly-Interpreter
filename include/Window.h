#ifndef WINDOW_H
#define WINDOW_H
#include <windows.h>
#include <string>


class Window
{
    public:
        HWND* parent;

        RECT location;
        HWND thisWindow;

        Window(HWND* parent,std::string type, std::string text, double x, double y, double width, double height, int menuNumber);
        Window(HWND* parent,std::string type, std::string text, int attributes, double x, double y, double width, double height, int menuNumber);
        Window(Window* previous,std::string type, std::string text, double xDisplace, double yDisplace, int menuNumber);
        virtual ~Window();
    protected:
        int attributes;
    private:
    Window* previousWindow;
    void makeRect(LPRECT out, int x, int y, int width, int height);
    std::string text, type;
};

#endif // BUTTON_H
