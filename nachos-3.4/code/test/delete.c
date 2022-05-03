#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
    char* name, * chr;
    int check;

    PrintString("Nhap ten file: ");
    ReadString(name, MAX_LENGTH);
    check = -1;

    check = Remove(name, -1);
    if(check == -1){
        PrintString("Xoa file that bai.");
    }
    else{
        PrintString("Xoa fie thanh cong.");
    }
    Halt();
}