/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*         System and Device Programming - Lab 11 == Jacopo Nasi            *
*       Repository available on  https://github.com/Jacopx/SDPlab          *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>

/* Prototypes */
DWORD WINAPI sideThread(PVOID pThParam);
DWORD WINAPI carThread(PVOID pThParam);
DWORD WINAPI spawnThread(PVOID pThParam);

/* Global variables */
HANDLE *sBusy; // Semaphores

struct info
{
    HANDLE *sideSem;
    int travelTime;
    char side;
    int numberCars;
    int n;
    int wait;
};

INT _tmain(INT argc, LPTSTR argv[])
{
    if (argc < 7)
    {
        printf("Usage: soft.exe time_A_L2R time_A_R2L time_T_L2R time_T_R2L number_L2R number_R2L");
    }
    printf("argc: %d\n", argc);

    /*
	Notice that:
	- Cars running from left-to-right arrive at random time intervals,
	varying from 0 to time_A_L2R (integer value) seconds.
	- Cars running from right-to-left arrive at random time intervals,
	varying from 0 to time_A_R2L (integer value) seconds.
	- All cars traversing the bridge from left-to-right need time_T_L2R
	(integer value) seconds
	- All cars traversing the bridge from right-to-left need time_T_R2L
	(integer value) seconds
	- The program has to terminate only once exactly
	- number_L2R (integer value) cars have traversed the bridge from
	left-to-right
	- number_R2L (integer value) cars have traversed the bridge from
	right-to-left.
	*/

    int time_A_L2R = atoi(argv[1]);
    int time_A_R2L = atoi(argv[2]);
    int time_T_L2R = atoi(argv[3]);
    int time_T_R2L = atoi(argv[4]);
    int number_L2R = atoi(argv[5]);
    int number_R2L = atoi(argv[6]);

    //output = min + (rand() % static_cast<int>(max - min + 1))

    info *left = (info *)malloc(sizeof(info));
    left->sideSem = (HANDLE *)CreateSemaphore(NULL, 1, 1, NULL);
    left->travelTime = 1000 * time_T_L2R;
    left->side = 'L';
    left->numberCars = number_L2R;
    left->wait = time_A_L2R;
    left->n = 0;

    info *right = (info *)malloc(sizeof(info));
    right->sideSem = (HANDLE *)CreateSemaphore(NULL, 1, 1, NULL);
    right->travelTime = 1000 * time_T_R2L;
    right->side = 'R';
    right->numberCars = number_R2L;
    right->wait = time_T_R2L;
    right->n = 0;

    sBusy = (HANDLE *)CreateSemaphore(NULL, 1, 1, NULL);

    HANDLE *spawnHandle = (HANDLE *)malloc(2 * sizeof(HANDLE));

    spawnHandle[0] = CreateThread(0, 0, spawnThread, left, 0, NULL);  /* Left side */
    spawnHandle[1] = CreateThread(0, 0, spawnThread, right, 0, NULL); /* Right side */

    WaitForMultipleObjects(2, spawnHandle, TRUE, INFINITE); /* Waiting side thread */

    return 0;
}

/* Thread function for each side */
DWORD WINAPI carThread(PVOID pThParam) {
    info *inf = (info *)pThParam;
    SYSTEMTIME st;
    int i = 0;
    srand(GetCurrentThreadId());

    WaitForSingleObject(inf->sideSem, INFINITE);
    inf->n++;
    if (inf->n == 1)
    {
        WaitForSingleObject(sBusy, INFINITE);
    }
    ReleaseSemaphore(inf->sideSem, 1, NULL);

    /* START */
    GetLocalTime(&st);
    printf("\t%c - Start: \t%02d:%02d.%d\n", inf->side, st.wHour, st.wMinute, st.wSecond);

    Sleep(inf->travelTime); /* TRAVEL TIME */

    GetLocalTime(&st);
    printf("\t%c - Stop: \t%02d:%02d.%d\n", inf->side, st.wHour, st.wMinute, st.wSecond);
    /* STOP */

    WaitForSingleObject(inf->sideSem, INFINITE);
    inf->n--;
    if (inf->n == 0)
    {
        ReleaseSemaphore(sBusy, 1, NULL);
    }
    ReleaseSemaphore(inf->sideSem, 1, NULL);

    return 0;
}

DWORD WINAPI spawnThread(PVOID pThParam) {
    info *inf = (info *)pThParam;
    SYSTEMTIME st;
    int total = inf->numberCars;
    int i = 0;

    srand(GetCurrentThreadId());

    HANDLE *Handle = (HANDLE *)malloc(inf->numberCars * sizeof(HANDLE));

    while (i < inf->numberCars)
    {
        /* RUN SIDE-to-SIDE */
        Sleep((DWORD)1000 * (rand() % static_cast<int>(inf->wait + 1)));
        GetLocalTime(&st);

        printf("%c - Create: \t%02d:%02d.%d\n", inf->side, st.wHour, st.wMinute, st.wSecond);

        Handle[i++] = CreateThread(0, 0, carThread, inf, 0, NULL); /* Car */
    }

    WaitForMultipleObjects(inf->numberCars, Handle, TRUE, INFINITE);
    free(Handle);

    return 0;
}
