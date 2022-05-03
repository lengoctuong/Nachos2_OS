#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
    char* name;
    int check;

    PrintString("Nhap ten file: ");
    ReadString(name, MAX_LENGTH);
    check = Create(name);

    if (check == -1)   // Truong hop khong the mo file -> dung program
    {
        PrintString("Khong the tao file!");
        Halt();
    }
    else{
        PrintString("Tao file thanh cong!");
    }

    Halt();
}