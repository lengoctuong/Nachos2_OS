#include "syscall.h"

int main()
{
    int a[100], n, i, j, t, c;

    // Nhap so pt cua mang
    PrintString("Nhap so pt mang [1; 100]: ");
    n = ReadNum();
    while (n <= 0 || n > 100)
    {
        PrintString("\t- So pt mang khong hop le, nhap lai so pt mang [1; 100]: ");
        n = ReadNum();
    }

    // Nhap mang
    for (i = 0; i < n; ++i)
    {
        PrintString(" + Array[");
        PrintNum(i);
        PrintString("] = ");
        a[i] = ReadNum();
    }

    // Chon cach sap xep
    PrintString("Chon cach sap xep [0] Tang - [1] Giam: ");
    c = ReadNum();
    while (c != 0 && c != 1)
    {
        PrintString("\t- Lua chon khong hop le, vui long chon lai: ");
        c = ReadNum();
    }

    // Bubble sort
    for (i = 0; i < n - 1; ++i)
        for (j = n - 1; j > i; --j)
            if (a[j] < a[j - 1] && c == 0 || a[j] > a[j - 1] && c == 1)
            {
                t = a[j];
                a[j] = a[j - 1];
                a[j - 1] = t;
            }

    // In mang sau khi sap xep
    PrintString("Mang sau khi sap xep la:\t");
    for (i = 0; i < n; ++i)
    {
        PrintNum(a[i]);
        PrintChar('\t');
    }

    PrintString("\n");
    Halt();
}