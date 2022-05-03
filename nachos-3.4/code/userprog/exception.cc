// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <cstdlib>
#include <ctime>

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MaxFileLength 32

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void IncreasePC()
{
    int counter = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, counter);
    counter = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, counter);
    machine->WriteRegister(NextPCReg, counter + 4);
}

char* User2System(int virtAddr,int limit)
{
    int i;// index
    int oneChar;
    char* kernelBuf = NULL;
    kernelBuf = new char[limit +1];//need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;
    
    memset(kernelBuf,0,limit+1);
    //printf("\n Filename u2s:");
    for (i = 0 ; i < limit ;i++)
    {
        machine->ReadMem(virtAddr+i,1,&oneChar);
        kernelBuf[i] = (char)oneChar;
        //printf("%c",kernelBuf[i]);
        if (oneChar == 0)
            break;
    }

    return kernelBuf;
}

int System2User(int virtAddr,int len,char* buffer)
{
    if (len < 0) return -1;
    if (len == 0)return len;
    int i = 0;
    int oneChar = 0 ;
    
    do {
        oneChar= (int) buffer[i];
        machine->WriteMem(virtAddr+i,1,oneChar);
        i ++;
    } while(i < len && oneChar != 0);
    
    return i;
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which)
    {
    case NoException:
        return;

    case PageFaultException:
        DEBUG('a', "\nNo valid translation found!\n");
        printf("\nNo valid translation found!\n");
   	    interrupt->Halt();
        break;

    case ReadOnlyException:
        DEBUG('a', "\nWrite attempted to page marked!\n");
        printf("\nWrite attempted to page marked!\n");
   	    interrupt->Halt();
        break;
    
    case BusErrorException:
        DEBUG('a', "\nTranslation resulted in an invalid physical address!\n");
        printf("\nTranslation resulted in an invalid physical address!\n");
   	    interrupt->Halt();
        break;
    
    case AddressErrorException:
        DEBUG('a', "\nUnaligned reference or one that was beyond the end of the address space!\n");
        printf("\nUnaligned reference or one that was beyond the end of the address space!\n");
   	    interrupt->Halt();
        break;

    case OverflowException:
        DEBUG('a', "\nInteger overflow in add or sub!\n");
        printf("\nInteger overflow in add or sub!\n");
   	    interrupt->Halt();
        break;

    case IllegalInstrException:
        DEBUG('a', "\nUnimplemented or reserved instr!\n");
        printf("\nUnimplemented or reserved instr!\n");
   	    interrupt->Halt();
        break;

    case NumExceptionTypes:
        DEBUG('a', "\nNumber exception types!\n");
        printf("\nNumber exception types!\n");
   	    interrupt->Halt();
        break;

    case SyscallException:
        switch (type)
        {
        case SC_Halt:
        {
            DEBUG('a', "\nShutdown, initiated by user program!\n");
            printf("\nShutdown, initiated by user program!\n");
            interrupt->Halt();
            return;
        }

        case SC_ReadNum:
        {
            // Input: K co
            // Output: Tra ve so nguyen doc duoc tu man hinh console.
            // Chuc nang: Doc so nguyen tu man hinh console.
            char* buffer;
            int MAX_BUFFER = 255;
            buffer = new char[MAX_BUFFER + 1];
            int numbytes = gSynchConsole->Read(buffer, MAX_BUFFER);// doc buffer toi da MAX_BUFFER ki tu, tra ve so ki tu doc dc
            int number = 0; // so luu ket qua tra ve cuoi cung
                
            /* Qua trinh chuyen doi tu buffer sang so nguyen int */
    
            // Xac dinh so am hay so duong                       
            bool isNegative = false; // Gia thiet la so duong.
            int firstNumIndex = 0;
            int lastNumIndex = 0;
            if(buffer[0] == '-')
            {
                isNegative = true;
                firstNumIndex = 1;
                lastNumIndex = 1;                                           
            }
            
            // Nhap chuoi rong
            if (buffer[0] == '\0')
            {
                DEBUG('a', "ERROR: The integer number is not valid!\n");
                printf("The integer number is not valid!\n");
                machine->WriteRegister(2, -2147483648);
                IncreasePC();
                delete buffer;
                return;
            }

            // Kiem tra tinh hop le cua so nguyen buffer
            for(int i = firstNumIndex; i < numbytes; i++)                   
            {
                if(buffer[i] == '.') /// 125.0000000 van la so
                {
                    int j = i + 1;
                    for(; j < numbytes; j++)
                    {
                        // So khong hop le
                        if(buffer[j] != '0')
                        {
                            DEBUG('a', "ERROR: The integer number is not valid!\n");
                            printf("The integer number is not valid!\n");
                            machine->WriteRegister(2, -2147483648);
                            IncreasePC();
                            delete buffer;
                            return;
                        }
                    }
                    // la so thoa cap nhat lastNumIndex
                    lastNumIndex = i - 1;               
                    break;                           
                }
                else if(buffer[i] < '0' || buffer[i] > '9')
                {
                    DEBUG('a', "ERROR: The integer number is not valid!\n");
                    printf("The integer number is not valid!\n");
                    machine->WriteRegister(2, -2147483648);
                    IncreasePC();
                    delete buffer;
                    return;
                }
                lastNumIndex = i;    
            }           
            
            // La so nguyen hop le, tien hanh chuyen chuoi ve so nguyen
            for(int i = firstNumIndex; i<= lastNumIndex; i++)
            {
                number = number * 10 + (int)(buffer[i] - 48); 
            }
            
            // neu la so am thi * -1;
            if(isNegative)
            {
                number = number * -1;
            }
            machine->WriteRegister(2, number);
            IncreasePC();
            delete buffer;
            return;     
        }

        case SC_PrintNum:
        {   
            // Input: mot so integer
            // Output: khong co 
            // Chuc nang: In so nguyen len man hinh console
            int number = machine->ReadRegister(4);
            if(number == 0)
            {
                gSynchConsole->Write("0", 1); // In ra man hinh so 0
                IncreasePC();
                return;    
            }
                    
            /*Qua trinh chuyen so thanh chuoi de in ra man hinh*/
            bool isNegative = false; // gia su la so duong
            int numberOfNum = 0; // Bien de luu so chu so cua number
            int firstNumIndex = 0; 
            // Nếu là số âm thì vị trí đầu tiên của mảng là "-" (0 index)
            if(number < 0)
            {
                isNegative = true;
                number = number * -1; // Nham chuyen so am thanh so duong de tinh so chu so
                firstNumIndex = 1; 
            }   
            
            int t_number = number; // bien tam cho number
            while(t_number)
            {
                numberOfNum++;
                t_number /= 10;
            }
    
            // Tao buffer chuoi de in ra man hinh
            char* buffer;
            int MAX_BUFFER = 255;
            buffer = new char[MAX_BUFFER + 1];
            for(int i = firstNumIndex + numberOfNum - 1; i >= firstNumIndex; i--)
            {
                buffer[i] = (char)((number % 10) + 48);
                number /= 10;
            }

            if(isNegative)
            {
                buffer[0] = '-';
                buffer[numberOfNum + 1] = 0;
                gSynchConsole->Write(buffer, numberOfNum + 1);
                delete buffer;
                IncreasePC();
                return;
            }
            buffer[numberOfNum] = 0;    
            gSynchConsole->Write(buffer, numberOfNum);
            delete buffer;
            IncreasePC();
            return;
        }

        case SC_ReadChar:
        {
            //Input: Khong co
            //Output: Duy nhất 1 ky tu (char)
            //Cong dung: Doc mot ky tu tu nguoi dung nhap

            int maxBytes;
            maxBytes = 255;
            char* buffer;
            buffer = new char[255];
            int numBytes;
            numBytes = gSynchConsole->Read(buffer, maxBytes);

            if(numBytes > 1) //Neu nhap nhiều hơn 1 ky tu thi khong hop le
            {
                DEBUG('a', "ERROR: Chi duoc nhap duy nhat 1 ky tu!\n");
                printf("Chi duoc nhap duy nhat 1 ky tu!\n");
                machine->WriteRegister(2, 0);
            }
            else if(numBytes == 0)  //Ky tu rong
            {
                DEBUG('a', "ERROR: Ky tu rong!\n");
                printf("Ky tu rong!\n");
                machine->WriteRegister(2, 0);
            }
            else    //Chuoi vua lay có 1 ky tu, lay ky tu o index = 0, return vào thành ghi R2
            {
                char c = buffer[0];
                machine->WriteRegister(2, c);
            }

            delete buffer;
            IncreasePC();
            break;
        }

        case SC_PrintChar:
        {
            // Input: Ki tu(char)
			// Output: Ki tu(char)
			// Cong dung: Xuat mot ki tu la tham so arg ra man hinh

            char c;
            c = (char)machine->ReadRegister(4); // Doc ki tu tu thanh ghi r4
			gSynchConsole->Write(&c, 1); // In ky tu tu bien c, 1 byte
			IncreasePC();
			break;
        }

        case SC_SRandomNum:
        {
            srand(time(NULL));
            IncreasePC();
            break;
        }

        case SC_RandomNum:
        {
            int res;
            res = rand();
            machine->WriteRegister(2, res);
            IncreasePC();
            break;
        }

        case SC_ReadString:
	    {
            // Input: Buffer(char*), do dai toi da cua chuoi nhap vao(int)
            // Output: Khong co
            // Cong dung: Doc vao mot chuoi voi tham so la buffer va do dai toi da

            int virtAddr, length;
            virtAddr = machine->ReadRegister(4); // Lay dia chi tham so buffer truyen vao tu thanh ghi so 4
            char* buffer;
            length = machine->ReadRegister(5); // Lay do dai toi da cua chuoi nhap vao tu thanh ghi so 5
            buffer = User2System(virtAddr, length); // Copy chuoi tu vung nho User Space sang System Space

            gSynchConsole->Read(buffer, length); // Goi ham Read cua SynchConsole de doc chuoi
            System2User(virtAddr, length, buffer); // Copy chuoi tu vung nho System Space sang vung nho User Space
            
            delete buffer; 
            IncreasePC(); // Tang Program Counter
            break;
	    }

	    case SC_PrintString:
        {
            // Input: Buffer(char*)
            // Output: Chuoi doc duoc tu buffer(char*)
            // Cong dung: Xuat mot chuoi la tham so buffer truyen vao ra man hinh

            int virtAddr;
            virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
            char* buffer;
            buffer = User2System(virtAddr, 255); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai 255 ki tu

            int length = 0;
            while (buffer[length] != 0) length++; // Dem do dai that cua chuoi
            gSynchConsole->Write(buffer, length + 1); // Goi ham Write cua SynchConsole de in chuoi
            
            delete buffer; 
            IncreasePC(); // Tang Program Counter 
            break;
        }

        case SC_Create: 
        { 
            int virtAddr; 
            char* filename; 
            DEBUG('a',"SC_Create call ...\n"); 
            DEBUG('a',"Reading virtual address of filename\n");
            // Lấy tham số tên tập tin từ thanh ghi r4 
            virtAddr = machine->ReadRegister(4); 
            DEBUG ('a',"Reading filename.\n"); 
            // MaxFileLength là = 32 
            filename = User2System(virtAddr,MaxFileLength + 1);
            if (strlen(filename) == 0) 
            { 
                printf("You don't input anything!\n"); 
                DEBUG('a',"You don't input anything!\n"); 
                machine->WriteRegister(2, -1); // trả về lỗi cho chương 
                // trình người dùng 
                IncreasePC();
                delete filename; 
                return;
            }  
            if (filename == NULL) 
            { 
                printf("Not enough memory in system!\n"); 
                DEBUG('a',"Not enough memory in system!\n"); 
                machine->WriteRegister(2, -1); // trả về lỗi cho chương 
                // trình người dùng 
                IncreasePC();
                delete filename; 
                return;
            } 
            DEBUG('a',"Finish reading filename.\n"); 
            //DEBUG('a',"\n File name : '"<<filename<<"'"); 
            // Create file with size = 0 
            // Dùng đối tượng fileSystem của lớp OpenFile để tạo file, 
            // việc tạo file này là sử dụng các thủ tục tạo file của hệ điều 
            // hành Linux, chúng ta không quản ly trực tiếp các block trên 
            // đĩa cứng cấp phát cho file, việc quản ly các block của file 
            // trên ổ đĩa là một đồ án khác 
            if (!fileSystem->Create(filename, 0)) 
            { 
                printf("Error create file '%s'\n",filename); 
                machine->WriteRegister(2, -1); 
                IncreasePC();
                delete filename; 
                return; 
            } 
            machine->WriteRegister(2, 0); // trả về cho chương trình 
            // người dùng thành công 
            IncreasePC();
            delete filename; 
            return; 
        }

		case SC_Open:
		{
			// Input: fileName: Ten chuoi name cua file, 
			// Output: Tra ve OpenFileID (int) neu thanh cong, -1 neu loi
			// Chuc nang: Mo file va tra ve ID cua file. 2 -> 9

			//OpenFileID Open(char *name)
			int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so *name* tu thanh ghi so  4
			char* filename;
			filename = User2System(virtAddr, MaxFileLength); 

			int freeSlot = fileSystem->FindFreeSlot();

            //Chi xu li khi con slot trong
			if (freeSlot != -1) 
			{
                // Gán giá trị openf[freeSlot] = Open(filename)
                // Nếu giá trị này khác NULL thì trả về thanh ghi OpenIDFile (freeSlot)
                if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename)) != NULL) //Mo file thanh cong
                {
                    machine->WriteRegister(2, freeSlot); //tra ve OpenFileID
                    IncreasePC();
				    delete[] filename;
				    return;
                }

			}
            // Nếu không thể open file thì trả về -1
			machine->WriteRegister(2, -1);
			IncreasePC();
			delete[] filename;
			return;
		}

		case SC_Write:
		{
			// Input: buffer(char*), so ky tu(int), id cua file(OpenFileID)
			// Output: -1 = Lõi. Còn lại = Thành công
			// Cong dung: Ghi vào file chuỗi buffer
			int virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
			int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
			int id = machine->ReadRegister(6); // Lay id cua file tu thanh ghi so 6
			char *buf;

            // Kiem tra size cua buffer, neu am thi tra ve -1
            if (charcount < 0)
            {
                printf("So ki tu ghi khong hop le!\n");
                machine->WriteRegister(2, -1);
                IncreasePC();
                return;
            }

			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 9)
			{
				printf("Khong the write - Vi id nam ngoai bang mo ta file!\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Kiem tra file co ton tai khong
			if (fileSystem->openf[id] == NULL)
			{
				printf("Khong the write vi file nay khong ton tai!\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			// Xet truong hop ghi file only read (type quy uoc la 1) hoac file stdin (type quy uoc la 2) thi tra ve -1
			if (id == 1 || id == 0)
			{
				printf("Khong the write file stdin hoac file only read!\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}
			buf = User2System(virtAddr, charcount);  // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount

			// Xử lí trường hợp strlen(buffer) < charcount
            // Thay vì ghi ra kí tự NULL thì ta chỉ write các giá trị char* != NULL
            charcount = 0;
            while (buf[charcount] != NULL)
                charcount++;
            if (id > 1)
			{
				if ((fileSystem->openf[id]->Write(buf, charcount)) > 0)
				{
					machine->WriteRegister(2, 0);
					delete buf;
					IncreasePC();
					return;
				}
			}
		}

        case SC_Read:
		{
            // int Read(char *buffer, int size, OpenFileId id);
            // tham so truyen vao bao gom 
            // virtAddr lay dia chi duoc luu vao thanh ghi so 4 va quan li boi tham so *buffer
            // charcount duoc lay dia chi tu thanh ghi so 5 quan li boi tham so size
            // id cua file(OpenFileID) duoc luu vao thanh ghi so 6 va quan li boi tham so id
			// Cong dung: Doc file voi tham so la buffer, so ky tu cho phep va id cua file
			int virtAddr = machine->ReadRegister(4); 
			int charcount = machine->ReadRegister(5); 
			int id = machine->ReadRegister(6); 
			int OldPos;
			int NewPos;
			char *buf;

			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 9)
			{
				printf("Khong the read vi id nam ngoai bang mo ta file!\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}

			// Kiem tra file co ton tai khong
			if (fileSystem->openf[id] == NULL)
			{
				printf("Khong the read vi file nay khong ton tai!\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}

            // truong hop size < 0
            if (charcount < 0) {
                printf("So ky tu can doc khong hop le!!!\n");
                machine->WriteRegister(2, -1);
				IncreasePC();
				return;
            }

            OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
			buf = User2System(virtAddr, charcount); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount

			// Xet truong hop ghi file only read (type quy uoc la 1) hoac file stdin (type quy uoc la 2) thi tra ve -1
			if (id == 1 || id == 0) {
				printf("Khong the read file stdin hoac file only read!\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}

            // Truong hop file co ton tai
            if (id > 1) {
                // Doc file co chua noi dung
				if ((fileSystem->openf[id]->Read(buf, charcount)) > 0) {
                    //printf("Doc file binh thuong.\n");

                    // So byte thuc su = NewPos - OldPos
                    NewPos = fileSystem->openf[id]->GetCurrentPos();
                    // Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su 
                    System2User(virtAddr, NewPos - OldPos, buf); 
                    machine->WriteRegister(2, NewPos - OldPos);
				}

                // Doc file rong
                else {
                    //printf("Doc file rong!\n");
                    machine->WriteRegister(2, -2);
			    }
			}
            
			delete buf;
			IncreasePC();
			return;
		}

        case SC_Seek:
		{
            // Prototype: int Seek(int pos, OpenFileId id);
			// Input: Vi tri con tro can den, id cua file
			// Output: -1  <-> loi, Vi tri thuc su cua con tro <-> thanh cong
			// Cong dung: Di chuyen con tro den vi tri thich hop trong file voi tham so la vi tri can chuyen va id cua file
			
            int pos = machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file tu reg 4
			int id = machine->ReadRegister(5);  // Lay id cua file tu reg 5
			
            // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id > 9)
			{
				printf("Khong the seek vi id nam ngoai bang mo ta file!\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}

			// Kiem tra file co dang open khong
			if (fileSystem->openf[id] == NULL)
			{
				printf("Khong the seek vi file nay chua open!\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}

			// Kiem tra co goi Seek tren file input va output khong
			if (id == 0 || id == 1)
			{
				printf("Khong the seek tren file console!\n");
				machine->WriteRegister(2, -1);
				IncreasePC();
				return;
			}

			// Neu pos = -1 thi gan pos = cuoi file, nguoc lai thi giu nguyen pos
			pos = (pos == -1) ? fileSystem->openf[id]->Length() : pos;

            // Kiem tra vi tri pos co hop le khong
			if (pos > fileSystem->openf[id]->Length() || pos < 0)
			{
				printf("Khong the seek file den vi tri nay!\n");
				machine->WriteRegister(2, -1);
			}
			else
			{
				fileSystem->openf[id]->Seek(pos);   // Di chuyen con tro den vi tri pos
				machine->WriteRegister(2, pos);
			}
            
			IncreasePC();
			return;
		}

        case SC_Remove: 
        { 
            int virtAddr, checkOpen; 
            char* filename;
           
            // Lay tham so ten tap tin tu thanh ghi r4 va r5
            virtAddr = machine->ReadRegister(4); 
            checkOpen = machine->ReadRegister(5);
            DEBUG('a',"Reading filename.\n");

            // MaxFileLength là = 32 
            // Kiem tra do dai cua ten file co bang 0 ki tu hay khong
            filename = User2System(virtAddr,MaxFileLength + 1);
            if (strlen(filename) == 0)
            { 
                printf("You don't input anything!\n"); 
                DEBUG('a',"You don't input anything!\n"); 
                machine->WriteRegister(2, -1); // trả về lỗi cho chương trình người dùng 
                IncreasePC();
                delete filename; 
                return;
            }

            // TH ten file == NULL
            if (filename == NULL) 
            { 
                printf("Not enough memory in system!\n");
                DEBUG('a',"Not enough memory in system\n");
                machine->WriteRegister(2, -1); // trả về lỗi cho chương trình người dùng 
                IncreasePC();
                delete filename; 
                return;
            } 
            DEBUG('a',"Finish reading filename.\n"); 
           
            // TH file dang mo 
            if (checkOpen != -1) {
                printf("This file is still opening, Try again later!!!\n"); 
                machine->WriteRegister(2, -1); 
                IncreasePC();
                delete filename; 
                return; 
            }

            // TH file khong ton tai
            if (!fileSystem->Remove(filename)) 
            { 
                printf("Error delete file '%s'\n", filename);
                machine->WriteRegister(2, -1); 
                IncreasePC();
                delete filename; 
                return; 
            } 

            // Xoa thanh cong
            printf("Deleted!!!\n"); 
            machine->WriteRegister(2, 0);
            IncreasePC();
            delete filename; 
            return; 
        }

		case SC_Close:
		{
            // Prototype: int Close(OpenFileId id);
			// Input: id cua file
			// Output: 0 <-> dong file thanh cong, -1 <-> dong file <-> that bai
            // Cong dung: Dong file voi id cua file

            // Lay id cua file tu reg 4
			int fid = machine->ReadRegister(4);

            // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (fid >= 0 && fid <= 9)
			{
                // Kiem tra file co dang open khong
				if (fileSystem->openf[fid])
				{
					delete fileSystem->openf[fid];  // Xoa vung nho luu tru file
					fileSystem->openf[fid] = NULL;  // Gan vung nho NULL

                    // Tra ve thanh cong
					machine->WriteRegister(2, 0);
                    IncreasePC();
					return;
				}
			}

            // Tra ve that bai
			machine->WriteRegister(2, -1);
            IncreasePC();
			return;
		}

        default:
            DEBUG('a', "\nUnexpected user mode exception!\n");
            printf("\nUnexpected user mode exception %d %d!\n", which, type);
            interrupt->Halt();
            break;
        }
        break;

    default:
        DEBUG('a', "\nUnexpected user mode exception!\n");
        printf("\nUnexpected user mode exception %d %d!\n", which, type);
        interrupt->Halt();
        break;
    }

    // if ((which == SyscallException) && (type == SC_Halt)) {
	// DEBUG('a', "Shutdown, initiated by user program.\n");
   	// interrupt->Halt();
    // } else {
	// printf("Unexpected user mode exception %d %d\n", which, type);
	// ASSERT(FALSE);
    // }
}
