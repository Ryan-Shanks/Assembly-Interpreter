#include "RunWindow.h"
#include <string>
#include <sstream>
bool strint(std::string t, int* value)
{
    if (t.compare("") == 0)
    {
        value = 0;
        return false;
    }
    std::stringstream s1(t);
	int result1;
	result1 = s1 >> result1 ? result1 : 0;
	std::stringstream s2(t);
	int result2;
	result2 = s2 >> result2 ? result2 : 1;

	if (result1 == result2)
    {
        if (value != NULL)
            *value = result1;
        return true;
    }
    else return false;
}
bool strintNonNeg(std::string t, int* value)
{
    if (t.compare("")==0)
    {
        value = 0;
        return false;
    }
    std::stringstream s1(t);
	int result1;
	result1 = s1 >> result1 ? result1 : 0;
	std::stringstream s2(t);
	int result2;
	result2 = s2 >> result2 ? result2 : 1;

	if (result1 == result2 && result1 >= 0)
    {
        if (value != NULL)
            *value = result1;
        return true;
    }
    else return false;
}
