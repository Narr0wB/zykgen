
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
    if (n_of_active_threads == threadcount()) {
        return -2;
    }

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

threadid_t threadcount() {
    return (threadid_t)sysconf(_SC_NPROCESSORS_ONLN);
}

#endif // _WIN32