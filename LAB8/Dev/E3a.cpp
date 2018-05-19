/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*        System and Device Programming - Lab 8.1a == Jacopo Nasi           *
*       Repository available on  https://github.com/Jacopx/SDPlab          *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

void read(HANDLE hIN);
void write(HANDLE hIN);

struct line {
	int row;
	long int id;
	char surnname[30];
	char name[30];
	int mark;
};

INT _tmain(INT argc, LPTSTR argv[])
{
	CHAR choice[4];
	HANDLE hIn;
	INT exit = 0;
	LARGE_INTEGER FilePos;
	PLARGE_INTEGER lpFilePos = 0;

	hIn = CreateFile(argv[1],GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
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
				SetFilePointerEx(hIn, FilePos, lpFilePos, 0);
				read(hIn);
				break;
			}

			case 'W': case 'w': {
				FilePos.QuadPart = (sizeof(line)*(atoi(&choice[2]) - 1));
				SetFilePointerEx(hIn, FilePos, lpFilePos, 0);
				write(hIn);
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

void read(HANDLE hIn) {
	DWORD nIn;

	line* row = (line *) malloc(sizeof(line));
	ReadFile(hIn, row, sizeof(line), &nIn, NULL);
	printf("%d %lu %s %s %d\n\n", row->row, row->id, row->surnname, row->name, row->mark);
	free(row);

	return;
}

void write(HANDLE hIn) {
	DWORD nIn;

	line* row = (line *)malloc(sizeof(line));
	scanf("%d %lu %s %s %d", &row->row, &row->id, &row->surnname, &row->name, &row->mark);
	WriteFile(hIn, row, sizeof(line), &nIn, NULL);
	free(row);

	return;
}
