#include "syscall.h"

int main()
{
    int i;

    PrintString("----------- ASCII TABLE -------------\n\n");
    PrintString("       [Dec]   ~~~~~   [Chr]\n");
    for (i = 32; i < 127; ++i)
    {
        PrintChar('\t');
        PrintNum(i);
        PrintString("\t<->\t-");
        PrintChar(i);
        PrintString("-\n");
    }

    Halt();
}