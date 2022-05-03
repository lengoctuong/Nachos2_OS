#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
    char* buf, * chr;
    int id, numChr;

    PrintString("Nhap ten file: ");
    ReadString(buf, MAX_LENGTH);
    id = Open(buf);

    if (id == -1)   // Truong hop khong the mo file -> dung program
    {
        PrintString("Khong the mo file!");
        Halt();
    }
    else    // doc tung ki tu tu file, neu la ki tu trong thi dung program, nguoc lai in ra
        while (1)
        {
            chr = "\0";
            numChr = Read(chr, 1, id);
            
            if (numChr == -2)
                break;
            else
                PrintString(chr);
        }

    Close(id);
    Halt();
}