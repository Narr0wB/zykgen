
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include "keygen.h"
#include "magic.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("usage: [-m|-n|-c] (optional)[-l <length>] (optional)[-f <filepath.txt>] (optional)[-g <# keys to gen>] (optional)[-t <threads, default is max_threads>] [serial]\n");
        exit(0);
    }

    int func_idx = 2;

    switch (argv[1][1])
    {
    case 'm':
        func_idx = 0;
        break;
    
    case 'n':
        func_idx = 1;
        break;

    case 'c':
        func_idx = 2;
        break;
    
    default:
        break;
    }

    clock_t timer;
    FILE* pfile = stdout;
    int threads = 16;
    int key_length = 16;
    int keys_to_gen = 1;
    const char* filepath = NULL;
    char* serial = NULL;

    for (int i = 1; i < argc; i++) {
        if (strncmp("-l", argv[i], 2) == 0 && strlen(argv[i]) == 2 && i + 1 < argc) {
            key_length = strtol(argv[i+1], NULL, 10);
        }
        else if (strncmp("-f", argv[i], 2) == 0 && strlen(argv[i]) == 2 && i + 1 < argc) {
            filepath = argv[i+1];

            if (strstr(filepath, ".txt") == NULL) {
                printf("Could not open/create the file, make sure the file's extension is .txt\n");
                exit(0);
            }

            pfile = fopen(filepath, "a");

            if (pfile == NULL) {
                printf("Could not open/create the file, exiting...\n");
                exit(0);
            }
        }
        else if(strncmp("-g", argv[i], 2) == 0 && strlen(argv[i]) == 2 && i + 1 < argc) {
            keys_to_gen = strtol(argv[i+1], NULL, 10);
            if (keys_to_gen == 0) keys_to_gen = 1;
        }
        else if(strncmp("-t", argv[i], 2) == 0 && strlen(argv[i]) == 2 && i + 1 < argc) {
            threads = strtol(argv[i+1], NULL, 10);
            if (threads > 16) {
                printf("Invalid number of threads, using default settings (MAX_THREADS -> %d)\n", 16);
                threads = 16;
            }
        }
        
        if (strncmp("S", argv[i], 1) == 0 && strlen(argv[i]) == 13)
            serial = argv[i];
    }

    if (serial == NULL) {
        printf("Invalid serial number, exiting...\n");
        exit(0);
    }

    if (!filepath && keys_to_gen > 100) {
        printf("Buffer is too big for stdout, please use a txt file with the \"-f\" option\n");
        exit(0);
    }

    char** pbuffers = malloc(threads * sizeof(char*));

    timer = clock();

    int pthread_keys_generated = 0;
    
    pbuffers[0] = malloc( ( (keys_to_gen / threads) + (keys_to_gen % threads) ) * (key_length + 1));

    thread_t s_thread;
    thr_keygen_args* args = calloc(1, sizeof(thr_keygen_args));

    args->n_start = 0;
    args->func_idx = func_idx;
    args->key_length = key_length;
    args->keys_to_gen = (keys_to_gen / threads) + (keys_to_gen % threads);
    args->p_buffer = pbuffers[0];
    args->serial = strdup(serial);
    pthread_keys_generated = (keys_to_gen / threads) + (keys_to_gen % threads);

    create_thread(&s_thread, keygen_thread, (void*)args);
    start_thread(&s_thread);
    

    for (int i = 1; i < threads; i++) {
        pbuffers[i] = malloc( (keys_to_gen / threads) * (key_length + 1));

        thread_t thread;
        args = calloc(1, sizeof(thr_keygen_args));

        args->n_start = pthread_keys_generated;
        args->func_idx = func_idx;
        args->key_length = key_length;
        args->keys_to_gen = (keys_to_gen / threads);
        args->p_buffer = pbuffers[i];
        args->serial = strdup(serial);
        pthread_keys_generated += (keys_to_gen / threads);

        create_thread(&thread, keygen_thread, (void*)args);
        #ifdef _WIN32
            start_thread(&thread);
        #endif // _WIN32
    }

    join_thread(NULL);

    clock_t timer2 = clock();

    //printf("%d %s", threads, pbuffers[1]);
    fwrite(pbuffers[0], sizeof(char), ( (keys_to_gen / threads) + (keys_to_gen % threads) ) * (key_length + 1), pfile);
    for (int i = 1; i < threads; i++) {
        fwrite(pbuffers[i], sizeof(char), (keys_to_gen / threads) * (key_length + 1), pfile);
    }

    if (keys_to_gen > 100) {
        printf("%d keys generated, time elapsed: %0.2fs", keys_to_gen, (float)(timer2 - timer) / CLOCKS_PER_SEC);
    }
    
    fclose(pfile);
    return 0;
}