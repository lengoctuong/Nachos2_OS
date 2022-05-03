/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"
#define MAX_LENGTH 32
int main()
{
	int a;
    int b;
    int size, checkCreatFile1;
    char* filename1;
    char* filename2;
    char* buffer;

    PrintString("Nhap vao file nguon(o day la a.txt): ");
    ReadString(filename1, MAX_LENGTH);
    a = Open(filename1);

    PrintString("Nhap vao file dich(o day la b.txt): ");
    ReadString(filename2, MAX_LENGTH);
    
    checkCreatFile1 = Create(filename2);
    if (checkCreatFile1 == -1)
    {
        PrintString("Khong the tao file ");
        PrintString(filename2);
        PrintString("\n");
        Halt();
    }
    b = Open(filename2);
    
    if (a == -1 || b == -1)
    {
        PrintString("Khong the mo file!\n");
        Halt();
    }

    size = Seek(-1, a);
    Seek(0, a);
    Read(buffer, size, a);
    Write(buffer, size, b);

    PrintString("Copy thanh cong ^^\n");
    Close(a);
    Close(b);

    Halt();
}
