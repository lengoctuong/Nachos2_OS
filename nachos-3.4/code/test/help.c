#include "syscall.h"

int main()
{
    PrintString("========== GROUP 8 ==========\n");
    PrintString("   20127383 Le Ngoc Tuong\n");
    PrintString("   20127588 Nguyen Tan Phat\n");
    PrintString("   20127663 Huynh Vi Khang\n");
    PrintString("=============================\n\n");

    PrintString("           - HELP -              \n");
    PrintString("Print ASCII Table run: ./userprog/nachos -rs 1023 -x ./test/ascii\n");
    PrintString("Sorting Program run: ./userprog/nachos -rs 1023 -x ./test/bubble_sort\n");
    Halt();
}
