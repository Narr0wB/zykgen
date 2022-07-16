
#include <stdint.h>
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

int create_thread(thread_t* thread, LPTHREAD_START_ROUTINE routine, void* args);

#else
#include <pthread.h>

typedef void* (LPTHREAD_START_ROUTINE) (void* args);

int create_thread(thread_t* id, LPTHREAD_START_ROUTINE routine, void* args);

#endif // _WIN32

int threadcount();

// if threadid is 0 then the function is applied to all created threads
int pause_thread(thread_t* thread);
int start_thread(thread_t* thread);
int join_thread(thread_t* thread);