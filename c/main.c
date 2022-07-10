
#include <stdio.h>
#include <time.h>
#include "keygen.h"
#include "magic.h"

#define SN_MAX 99999999

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("usage: [-m|-n|-c] (optional)[-l <length>] (optional)[-f <filepath.txt>] (optional)[-g <# keys to gen>] [serial]\n");
        exit(0);
    }

    hashfunc func_list[3] = { mojhash, neghash, coshash };
    int func_idx = 0;


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
    int key_length = 16;
    int keys_to_gen = 1;
    const char* filepath = NULL;
    char* serial = NULL;

    timer = clock();
    for (int i = 1; i < argc; i++) {
        if (strncmp("-l", argv[i], 2) == 0 && strlen(argv[i]) == 2 && i + 1 < argc) {
            key_length = strtol(argv[i+1], NULL, 0);
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
            keys_to_gen = strtol(argv[i+1], NULL, 0);
            if (keys_to_gen == 0) keys_to_gen = 1;
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

    char* buf = calloc(key_length + 1, 1);
    char* sn_numbers;
    int i_sn_numbers = 0;

    sn_numbers = serial + 5;
    i_sn_numbers = strtol(sn_numbers, NULL, 0);
    sn_numbers[1] = '\0';

    for (int i = 0; i < keys_to_gen && i_sn_numbers + i < SN_MAX+1; i++) {

        sn_numbers[1] = '\0';

        sprintf(serial, "%s%07d", serial, i_sn_numbers + i);

        keygen(buf, key_length, serial, strlen(serial), func_list[func_idx]);
        fprintf(pfile, "\n%s", buf, serial);
    }
    clock_t timer2 = clock();

    if (keys_to_gen > 100) {
        printf("%d keys generated, time elapsed: %0.3fs", keys_to_gen, (float)(timer2 - timer) / CLOCKS_PER_SEC);
    }
    
    fclose(pfile);
    free(buf);
    return 0;
}