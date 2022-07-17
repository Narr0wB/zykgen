
#include "threads.h"

#ifdef _WIN32

static int n_of_active_threads;
static HANDLE* t_handles;

int threadcount() {
    static SYSTEM_INFO info;
    GetSystemInfo(&info);
    
    return info.dwNumberOfProcessors;
}

int create_thread(thread_t* thread, LPTHREAD_START_ROUTINE routine, void* args) {

    HANDLE hndl = CreateThread(
        NULL,
        0,
        routine,
        args,
        CREATE_SUSPENDED,
        &thread->id
    );

    if (hndl == NULL) {
        return -1;
    }

    thread->handle = hndl;
    n_of_active_threads++;
    
    t_handles = realloc(t_handles, n_of_active_threads * sizeof(HANDLE));
    t_handles[n_of_active_threads-1] = hndl;
    
    return 1;
}

int pause_thread(thread_t* thread) {
    if (thread == NULL) {
        HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (hThreadSnap == NULL) return -2;

        THREADENTRY32 te32;
        te32.dwSize = sizeof(THREADENTRY32);

        if (!Thread32First(hThreadSnap, &te32)) {
            CloseHandle(hThreadSnap);
            return -3;
        }

        do {
            if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId()) {
                HANDLE hndl = OpenThread(
                                    THREAD_ALL_ACCESS,
                                    FALSE,
                                    te32.th32ThreadID
                                );

                if (SuspendThread(hndl) < 0) return -4;
                CloseHandle(hndl);
            }
        } while (Thread32Next(hThreadSnap, &te32));

        CloseHandle(hThreadSnap);

        return 1;
    }

    return (int)SuspendThread(thread->handle);
}

int start_thread(thread_t* thread) {
    if (thread == NULL) {
        HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (hThreadSnap == NULL) return -2;

        THREADENTRY32 te32;
        te32.dwSize = sizeof(THREADENTRY32);

        if (!Thread32First(hThreadSnap, &te32)) {
            CloseHandle(hThreadSnap);
            return -3;
        }

        do {
            if (te32.th32OwnerProcessID == GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId()) {
                HANDLE hndl = OpenThread(
                                    THREAD_ALL_ACCESS,
                                    FALSE,
                                    te32.th32ThreadID
                                );

                if (ResumeThread(hndl) < 0) return -4;
                CloseHandle(hndl);
            }
        } while (Thread32Next(hThreadSnap, &te32));

        CloseHandle(hThreadSnap);

        return 1;
    }

    return ResumeThread(thread->handle);
}

int join_thread(thread_t* thread) {
    if (thread == NULL) {
        if (WaitForMultipleObjects(n_of_active_threads, t_handles, TRUE, INFINITE) == WAIT_FAILED) return -4;

        return 1;
    }

    if (WaitForSingleObject(thread->handle, INFINITE) == WAIT_FAILED) {
        return -1;
    }

    return 1;
}

#else

static thread_t* threads;
static int n_threads;

int threadcount() {
    return (int)sysconf(_SC_NPROCESSORS_ONLN);
}

int create_thread(thread_t* thread, LPTHREAD_START_ROUTINE routine, void* args) {
    n_threads++;

    threads = realloc(threads, n_threads * sizeof(thread_t));
    threads[n_threads-1] = *thread;

    return pthread_create(thread, NULL, thread, args);
}

int join_thread(thread_t* thread) { 
    if (thread == NULL) {
        for (int i = 0; i < n_threads; i++) {
            if (pthread_join(threads[i], NULL) != 0)
                return -1;
        }
    }
    else if (pthread_join(*thread, NULL) != 0) {
        return -1;
    }

    return 1;
}


#endif // _WIN32