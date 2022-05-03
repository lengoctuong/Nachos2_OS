#include "syscall.h"

int main()
{
    int temp;
    char c = 't';
    int r, i;
    char *buffer;


    // ========== Check: ReadNum, PrintNum ==========
    PrintString("Nhap mot so: ");
    temp = ReadNum();
    if (temp != -2147483648)
    {
        PrintString("So vua nhap la: \"");
        PrintNum(temp);
        PrintString("\"\n\n");
    }
    // ==================================================


    // ========== Check: ReadChar, PrintChar ==========
    PrintString("Nhap mot ki tu: ");
    c = ReadChar();
    if (c != 0)
    {
        PrintString("Ki tu vua nhap la: \"");
        PrintChar(c);
        PrintString("\"\n");
    }
    PrintChar('\n');
    // ==================================================


    // ========== Check: RandomNum ==========
    SRandomNum();
    PrintString("Random 10 so:\n");
    for (i = 0; i < 10; ++i)
    {
        PrintString(" - Random[");
        PrintNum(i);
        PrintString("] = ");

        PrintNum(RandomNum());
        PrintChar('\n');
    }
    PrintChar('\n');
    // ==================================================


    // ========== Check: ReadString, PrintString ==========
    PrintString("Nhap mot chuoi: ");
    ReadString(buffer, 100);
    
    PrintString("Chuoi vua nhap la: \"");
    PrintString(buffer);
    PrintString("\"\n");
    // ==================================================

    Halt();
}
