
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

typedef unsigned long threadid_t;

#ifdef _WIN32
#include <windows.h>
#include <handleapi.h>
#include <tlhelp32.h>

typedef struct {
    threadid_t id;
    HANDLE handle;
} thread_t;

#else
#include <pthread.h>

typedef pthread_t thread_t;

typedef void* (LPTHREAD_START_ROUTINE) (void* args);

#endif // _WIN32

int create_thread(thread_t* thread, LPTHREAD_START_ROUTINE routine, void* args);

int threadcount();

// if thread is NULL then the function is applied to all active threads
int pause_thread(thread_t* thread);
int start_thread(thread_t* thread);
int join_thread(thread_t* thread);