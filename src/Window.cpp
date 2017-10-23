#include "Window.h"
#include <string>
#include <windows.h>
#include <iostream>

void Window::makeRect (LPRECT out, int x, int y, int width, int height)
{
    location.left = x;
    location.top = y;
    location.right = width + x;
    location.bottom = height + y;
}
Window::Window(HWND* parent, std::string type, std::string text, double x, double y, double width, double height, int menuNumber) /// Make a button from scratch with default styles
{
    this->parent = parent;
    this->text = text;
    this->type = type;
    this->attributes = WS_VISIBLE | WS_BORDER | WS_CHILD;
    thisWindow = CreateWindow(type.c_str(),
                              text.c_str(),
                              this->attributes,
                              (int)x, (int)y, (int)width, (int)height,
                              *parent,(HMENU) menuNumber, NULL, NULL);
    makeRect(&location, (int)x, (int)y, (int)width, (int)height);
}
Window::Window(HWND* parent, std::string type, std::string text, int attributes, double x, double y, double width, double height, int menuNumber) /// Make a button from scratch
{
    this->parent = parent;
    this->text = text;
    this->type = type;
    this->attributes = attributes;
    thisWindow = CreateWindow(type.c_str(),
                              text.c_str(),
                              this ->attributes,
                              (int)x, (int)y, (int)width, (int)height,
                              *parent,(HMENU) menuNumber, NULL, NULL);
    makeRect(&location, (int)x, (int)y, (int)width, (int)height);
}
Window::Window(Window* previous, std::string type, std::string text, double xDisplace, double yDisplace, int menuNumber) /// make a new button positioned relative to the previous button
{
    int x, y, width, height;
    x = previous->location.left + xDisplace;
    y = previous->location.top + yDisplace;
    width = previous->location.right - previous->location.left;
    height = previous->location.bottom - previous->location.top;
    this->attributes = previous -> attributes;

    parent = previous->parent;

    thisWindow = CreateWindow(type.c_str(),
                              text.c_str(),
                              this->attributes,
                              x, y, (int)width, (int)height,
                              *parent, (HMENU) menuNumber, NULL, NULL);
    makeRect(&location, x, y, width, height);
}
Window::~Window()
{
    DestroyWindow(thisWindow);
}
