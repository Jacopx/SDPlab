/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*         System and Device Programming - Lab 9 == Jacopo Nasi             *
*       Repository available on  https://github.com/Jacopx/SDPlab          *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

#define TYPE_FILE 1
#define TYPE_DIR 2
#define TYPE_DOT 3

DWORD WINAPI ThreadFunction(PVOID pThParam);
DWORD FileType(LPWIN32_FIND_DATA pFileData);

struct dir {
	LPTSTR folder;
	int level;
};

INT _tmain(INT argc, LPTSTR argv[])
{
	INT i = 0;
	unsigned int myCounter = 0;

	if (argc <= 1) {
		printf("Usage: soft.exe dir1 dir2 dir3 ... dirN");
	}

	printf("argc: %d\n", argc);

	HANDLE* myHandle = (HANDLE*)malloc((argc - 1) * sizeof(HANDLE));
	DWORD* myThreadID = (DWORD*)malloc((argc - 1) * sizeof(DWORD));
	dir** dirStruct = (dir**)malloc((argc - 1) * sizeof(dir*));

	char base[256];
	GetCurrentDirectoryA(256, base);
	strcat(base, "\\");

	for (i = 0; i < argc - 1; ++i) {
		dirStruct[i] = (dir*)malloc(sizeof(dir));
		char *buf = (char *)calloc(1024, sizeof(char));

		strcpy(buf, base);
		strcat(buf, argv[i + 1]);
		strcat(buf, "\\");
		dirStruct[i]->folder = buf;
		dirStruct[i]->level = 1;

		printf("\nlevel=%d DIR : %s\n", 0, argv[i + 1]);
		myHandle[i] = CreateThread(0, 0, ThreadFunction, dirStruct[i], 0, &myThreadID[i]);
		Sleep(200);

		//free(buf);
	}

	WaitForMultipleObjects(argc - 1, myHandle, TRUE, INFINITE);

	return 0;
}

DWORD WINAPI ThreadFunction(PVOID pThParam) {
	dir* dirStruct = (dir*)pThParam;

	char *buf2 = (char *) calloc(1024, sizeof(char));
	HANDLE SearchHandle;
	WIN32_FIND_DATA FindData;
	DWORD FType, i;

	strcpy(buf2, dirStruct->folder);
	if (dirStruct->level != 1)
		strcat(buf2, "\\");
	strcat(buf2, "*");

	SearchHandle = FindFirstFile(buf2, &FindData);
	do {
		FType = FileType(&FindData);
		if (FType == TYPE_FILE) {
			//Printing
			printf("[%d]", GetCurrentThreadId());
			for (i = 0; i<dirStruct->level; i++)
				printf("\t");

			printf("level=%d FILE: %s\n", dirStruct->level, FindData.cFileName);
		}
		if (FType == TYPE_DIR) {
			//Printing
			printf("[%d]", GetCurrentThreadId());
			for (i = 0; i<dirStruct->level; i++)
				printf("\t");

			printf("level=%d DIR : %s\n", dirStruct->level, FindData.cFileName);

			dir* dirStructNew = (dir*)malloc(sizeof(dir));
			char *buf = (char *)calloc(1024, sizeof(char));
			strcpy(buf, dirStruct->folder);
			strcat(buf, "\\");
			strcat(buf, FindData.cFileName);

			dirStructNew->folder = buf;
			dirStructNew->level = dirStruct->level + 1;

			ThreadFunction(dirStructNew);

			free(dirStructNew);
			free(buf);

		}
	} while (FindNextFile(SearchHandle, &FindData));

	FindClose(SearchHandle);
	free(buf2);

	return(0);
}

static DWORD FileType(LPWIN32_FIND_DATA pFileData) {
	BOOL IsDir;
	DWORD FType;
	FType = TYPE_FILE;
	IsDir = (pFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	if (IsDir)
		if (lstrcmp(pFileData->cFileName, _T(".")) == 0 || lstrcmp(pFileData->cFileName, _T("..")) == 0)
			FType = TYPE_DOT;
		else
			FType = TYPE_DIR;

	return FType;
}
