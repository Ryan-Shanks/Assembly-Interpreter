#include "LineOfText.h"
#include <windows.h>

LineOfText::LineOfText(const char* instruction, short address) /// read up to the null character
{
    strcpy(&this->instruction[0], instruction);
    this->address = address;
}

LineOfText::LineOfText()
{
    for (int i =0; i < MAX_INSTRUCTION_LENGTH; i++)
    {
        instruction[i] = 0;
    }
    address = 0;
}
void LineOfText::modifyAddress(const char* a)
{
    address = atoi(a);
    if (abs(address) >= 1000000)
    {
        address = 0;
    }
}

void LineOfText::modifyInstruction(const char* ins)
{
    if (strlen(ins) < MAX_INSTRUCTION_LENGTH)
        strcpy(&instruction[0], ins);
}

void LineOfText::modifyAddress(int address)
{
    this->address = address;
}

int LineOfText::getAddress()
{
    return address;
}
const char* LineOfText::getAddressStr()
{
    if (address >= 0)
    {
        itoa(address, &addressStr[0], 10);
    }
    else
    {
        strcpy(addressStr, "");
    }
    return &addressStr[0];
}

const char* LineOfText::getInstruction()
{
    return (&instruction[0]);
}

LineOfText::~LineOfText()
{

}
