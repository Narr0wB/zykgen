#include "threads.h"
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#define ERROR_HANDLE(__return_code) printf("%d\n", __return_code)

DWORD myfunc(void* args) {
    for(int i = 0; i < 20; i++) {
        printf("TH 1, n = %d\n", i);
        Sleep(500);
    }

    return 0;
}

DWORD myfunc2(void* args) {
    for(int i = 0; i < 15; i++) {
        printf("TH 2, n = %d\n", i);
        Sleep(500);
    }

    return 0;
}

int main(void) {
    thread_t id;
    thread_t id2;
    create_thread(&id2, myfunc2, NULL);
    create_thread(&id, myfunc, NULL);
    start_thread(NULL);
    Sleep(1000);
    pause_thread(NULL);
    Sleep(1000);
    start_thread(NULL);
    ERROR_HANDLE(join_thread(NULL));
    

    return 0;
}