/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*        System and Device Programming - Lab 8.1c == Jacopo Nasi           *
*       Repository available on  https://github.com/Jacopx/SDPlab          *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

void read(HANDLE hIN, OVERLAPPED ov, LARGE_INTEGER FileReserved);
void write(HANDLE hIN, OVERLAPPED ov, LARGE_INTEGER FileReserved);

struct line {
	int row;
	long int id;
	char surnname[30];
	char name[30];
	int mark;
};

OVERLAPPED ov = { 0, 0, 0, 0, NULL };

INT _tmain(INT argc, LPTSTR argv[])
{
	CHAR choice[4];
	HANDLE hIn;
	INT exit = 0;
	LARGE_INTEGER FilePos, FileReserved;
	PLARGE_INTEGER lpFilePos = 0;

	hIn = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Cannot open input file. Error: %x\n", GetLastError());
		return 2;
	}

	while (exit == 0) {

		printf("User input: ");
		fgets(choice, 4, stdin);

		switch (choice[0])
		{
			case 'R': case 'r': {
				FilePos.QuadPart = (sizeof(line)*(atoi(&choice[2])-1));
				FileReserved.QuadPart = (sizeof(line)*(atoi(&choice[2])));
				ov.Offset = FilePos.LowPart;
				ov.OffsetHigh = FilePos.HighPart;
				ov.hEvent = (HANDLE) 0;
				read(hIn, ov, FileReserved);
				break;
			}

			case 'W': case 'w': {
				FilePos.QuadPart = (sizeof(line)*(atoi(&choice[2]) - 1));
				ov.Offset = FilePos.LowPart;
				ov.OffsetHigh = FilePos.HighPart;
				write(hIn, ov, FileReserved);
				break;
			}

			case 'E': case 'e': {
				exit = 1;
				break;
			}
		}
	}

	CloseHandle(hIn);
  return 0;
}

void read(HANDLE hIn, OVERLAPPED ov, LARGE_INTEGER FileReserved) {
	DWORD nIn;

	line* row = (line *) malloc(sizeof(line));
	LockFileEx(hIn, LOCKFILE_EXCLUSIVE_LOCK, 0, FileReserved.LowPart, FileReserved.HighPart, &ov);
	ReadFile(hIn, row, sizeof(line), &nIn, &ov);
	UnlockFileEx(hIn, 0, FileReserved.LowPart, FileReserved.HighPart, &ov);
	printf("%d %lu %s %s %d\n\n", row->row, row->id, row->surnname, row->name, row->mark);
	free(row);

	return;
}

void write(HANDLE hIn, OVERLAPPED ov, LARGE_INTEGER FileReserved) {
	DWORD nIn;

	line* row = (line *)malloc(sizeof(line));
	scanf("%d %lu %s %s %d", &row->row, &row->id, &row->surnname, &row->name, &row->mark);
	LockFileEx(hIn, LOCKFILE_EXCLUSIVE_LOCK, 0, FileReserved.LowPart, FileReserved.HighPart, &ov);
	WriteFile(hIn, row, sizeof(line), &nIn, &ov);
	UnlockFileEx(hIn, 0, FileReserved.LowPart, FileReserved.HighPart, &ov);
	free(row);

	return;
}
