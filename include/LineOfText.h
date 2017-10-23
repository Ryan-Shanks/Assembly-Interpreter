#ifndef LINEOFTEXT_H
#define LINEOFTEXT_H
#define MAX_INSTRUCTION_LENGTH 10

class LineOfText
{
    public:
        LineOfText(const char* instruction, short address); // create a new line with data
        LineOfText(); //create a new, empty line
        virtual ~LineOfText();
        void modifyInstruction(const char* ins);
        void modifyAddress(int address);
        void modifyAddress(const char* a);
        int getAddress(); // return the address to be used with the command
        const char* getInstruction();
        const char* getAddressStr();
    private:
        char instruction[MAX_INSTRUCTION_LENGTH];
        int address;
        char addressStr[12];
};

#endif // LINEOFTEXT_H
