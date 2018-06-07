/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*         System and Device Programming - Lab 10 == Jacopo Nasi            *
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

HANDLE* sem, semCompare;
char** common;
int n;


DWORD WINAPI ThreadFunction(PVOID pThParam);
DWORD WINAPI Compare(PVOID pThParam);
DWORD FileType(LPWIN32_FIND_DATA pFileData);

struct dir {
	LPTSTR folder;
	int level;
	int num;
};

INT _tmain(INT argc, LPTSTR argv[])
{
	INT i = 0;
	unsigned int myCounter = 0;

	if (argc <= 1) {
		printf("Usage: soft.exe dir1 dir2 dir3 ... dirN");
	}

	n = (argc - 1);

	//printf("argc: %d\n", argc);

	HANDLE* myHandle = (HANDLE*)malloc((argc - 1) * sizeof(HANDLE));
	DWORD* myThreadID = (DWORD*)malloc((argc - 1) * sizeof(DWORD));
	dir** dirStruct = (dir**)malloc((argc - 1) * sizeof(dir*));

	common = (char**)malloc((argc - 1) * sizeof(char*));
	sem = (HANDLE*)malloc((argc - 1) * sizeof(HANDLE));

	semCompare = CreateSemaphore(NULL, 0, n, NULL);

	for (i = 0; i < argc - 1; ++i) {
		sem[i] = CreateSemaphore(NULL, 0, 1, NULL);
		common[i] = (char*) malloc(256 * sizeof(char));
	}

	DWORD comparingID;
	HANDLE comparing = (HANDLE) CreateThread(0, 0, Compare, NULL, 0, &comparingID);

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
		dirStruct[i]->num = i;

		//printf("\nlevel=%d DIR : %s\n", 0, argv[i + 1]);
		myHandle[i] = CreateThread(0, 0, ThreadFunction, dirStruct[i], 0, &myThreadID[i]);

		//free(buf);
	}

	WaitForMultipleObjects(argc - 1, myHandle, TRUE, INFINITE);
	printf("EQUAL DIRECTORY!\n\n");

	return 0;
}

DWORD WINAPI Compare(PVOID pThParam) {
	int i;
	char a;

	while (true) {
		for (i = 0; i < n; ++i) {
			WaitForSingleObject(semCompare, INFINITE);
		}

		// Compare common values
		for (i = 0; i < n; ++i) {
			if ( strcmp(common[0],common[i]) != 0) {
				printf("common[0]= %s != %s = common[%d]\n", common[0], common[i], i);
				printf("ERROR, DIFFERENT DIRECTORY!!!\n\n");
				exit(1);
			} else {
				//printf("common[0]= %s == %s = common[%d]\n", common[0], common[1], i);
			}
		}

		for (i = 0; i < n; ++i) {
			ReleaseSemaphore(sem[i], 1, NULL);
		}
	}

	return(0);
}

DWORD WINAPI ThreadFunction(PVOID pThParam) {
	dir* dirStruct = (dir*)pThParam;

	char *buf2 = (char *)calloc(1024, sizeof(char));
	HANDLE SearchHandle;
	WIN32_FIND_DATA FindData;
	DWORD FType, i;
	DWORD dwWaitResult;

	strcpy(buf2, dirStruct->folder);
	if (dirStruct->level != 1)
		strcat(buf2, "\\");
	strcat(buf2, "*");

	SearchHandle = FindFirstFile(buf2, &FindData);
	do {
		FType = FileType(&FindData);
		if (FType == TYPE_FILE) {
			// Saving filename in the common array
			strcpy(common[dirStruct->num], FindData.cFileName);

			ReleaseSemaphore(semCompare, 1, NULL);
			WaitForSingleObject(sem[dirStruct->num], INFINITE);

		}
		if (FType == TYPE_DIR) {
			// Saving dir in the common array
			strcpy(common[dirStruct->num], FindData.cFileName);

			ReleaseSemaphore(semCompare, 1, NULL);
			WaitForSingleObject(sem[dirStruct->num], INFINITE);

			dir * dirStructNew = (dir*)malloc(sizeof(dir));
			char *buf = (char *)calloc(1024, sizeof(char));
			strcpy(buf, dirStruct->folder);
			strcat(buf, "\\");
			strcat(buf, FindData.cFileName);

			dirStructNew->folder = buf;
			dirStructNew->level = dirStruct->level + 1;
			dirStructNew->num = dirStruct->num;

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
