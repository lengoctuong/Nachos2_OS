#include "syscall.h"
#define MAX_LENGTH 32

int main()
{
    char* buf, * chr;
    int id1, id2, numChr, success = 1;

    PrintString("Nhap ten file nguon 1: ");
    ReadString(buf, MAX_LENGTH);
    id1 = Open(buf);    
    
    PrintString("Nhap ten file nguon 2: ");
    ReadString(buf, MAX_LENGTH);
    id2 = Open(buf);

    if (id1 == -1 || id2 == -1) // Truong hop khong the mo 1 trong 2 file -> dung program
    {
        PrintString("Khong the mo file!");
        Halt();
    }
    else    // nhay den cuoi file 1, lan luot doc tung ki tu tu file 2
            // neu la ki tu trong thi dung program, nguoc lai ghi vao file 1
    {
        Seek(-1, id1);
        while (1)
        {
            chr = "\0";
            numChr = Read(chr, 1, id2);
            
            if (numChr == -2)
                break;
            else
                Write(chr, 1, id1);
        }
    }

    PrintString("Noi hai file thanh cong.\n");
    Close(id1);
    Close(id2);

    Halt();
}