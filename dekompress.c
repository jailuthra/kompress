#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct huffcode {
    int set;
    char code[30];
};

void read_header(FILE *fp, struct huffcode codetable[128])
{
    uint8_t byte, x, y;
    uint16_t twobyte;
    fread(&byte, sizeof(uint8_t), 1, fp);
    twobyte = byte;
    fread(&byte, sizeof(uint8_t), 1, fp);
    twobyte <<= 8;
    twobyte |= byte;
    if (!(twobyte == 0x05A1)) {
        fprintf(stderr, "File is not in KMP format\n");
        exit(EXIT_FAILURE);
    }
    fread(&byte, sizeof(uint8_t), 1, fp);
    printf("padding %d\n", byte);
    fread(&byte, sizeof(uint8_t), 1, fp);
    int n = byte, i, j, k, symb;
    char code[30];
    memset(code, 0, 30);
    for (i=0; i<n; i++) {
        fread(&byte, sizeof(uint8_t), 1, fp);
        symb = byte;
        fread(&byte, sizeof(uint8_t), 1, fp);
        x = byte >> 4; // no. of bytes
        y = byte & 0x0f; // padding
        memset(code, 0, 30);
        for (j=0; j<x; j++) {
            fread(&byte, sizeof(uint8_t), 1, fp);
            for (k=7; (j!=x-1 && k>=0) || (j==x-1 && k>=y); k--) {
                code[strlen(code)] = ((byte >> k) & 1) == 0 ? '0' : '1';
                code[strlen(code) + 1] = '\0';
            }
        }
        codetable[symb].set = 1;
        strcpy(codetable[symb].code, code);
    }
}

int main(int argc, char *argv[])
{
    FILE *kmp, *fp = stdout;
    int i;
    if (argc == 2) {
        if (!(kmp = fopen(argv[1], "rb"))) {
            fprintf(stderr, "Couldn't open file %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    } else {
        printf("USAGE: %s <infile.kmp>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    struct huffcode codetable[128];
    for (i=0; i<128; i++) {
        codetable[i].set = 0;
    }
    read_header(kmp, codetable);
    for (i=0; i<128; i++) {
        if (codetable[i].set) {
            printf("%c(%d) - %s\n", i, i, codetable[i].code);
        }
    }
    return 0;
}
