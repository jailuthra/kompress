/**
 * @file
 * Dekompress
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "bitstream.h"
#include "kmp.h"

int read_header(FILE *fp, struct huffcode codebook[ALPHLEN])
{
    uint8_t byte, x, y;
    uint16_t magic;
    fread(&byte, sizeof(uint8_t), 1, fp);
    magic = byte;
    fread(&byte, sizeof(uint8_t), 1, fp);
    magic <<= 8;
    magic |= byte;
    if (!(magic == 0x05A1)) {
        fprintf(stderr, "File is not in KMP format\n");
        exit(EXIT_FAILURE);
    }
    fread(&byte, sizeof(uint8_t), 1, fp);
    int padding = byte;
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
        codebook[symb].set = 1;
        strcpy(codebook[symb].code, code);
    }
    return padding;
}

void update_tree(struct treenode *root, uint8_t sym, char *code)
{
    struct treenode *ptr = root;
    int i;
    for (i=0; code[i]; i++) {
        if (code[i] == '0') {
            if (!ptr->l) {
                ptr->l = malloc(sizeof(struct treenode));
                ptr->l->symbol = 0;
                ptr->l->l = ptr->l->r = NULL;
            }
            ptr = ptr->l;
        } else {
            if (!ptr->r) {
                ptr->r = malloc(sizeof(struct treenode));
                ptr->r->symbol = 0;
                ptr->r->l = ptr->r->r = NULL;
            }
            ptr = ptr->r;
        }
    }
    ptr->symbol = sym;
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
    struct huffcode codebook[ALPHLEN];
    for (i=0; i<ALPHLEN; i++) {
        codebook[i].set = 0;
    }
    int padding = read_header(kmp, codebook);
    struct treenode *root = malloc(sizeof(struct treenode));
    root->symbol = 0;
    root->l = root->r = NULL;
    for (i=0; i<ALPHLEN; i++) {
        if (codebook[i].set) {
            update_tree(root, i, codebook[i].code);
        }
    }
    struct treenode *ptr = root;
    struct bitstream *bs = initbitstream(kmp, BS_READ, padding);
    int start = 1;
    int b;
    while ((b = read_bit(bs)) != -1) {
        if (start) {
            ptr = root;
            start = 0;
        }
        if (b == 0) {
            ptr = ptr->l;
        } else if (b == 1) {
            ptr = ptr->r;
        }
        if (ptr->symbol) {
            start = 1;
            printf("%c", ptr->symbol);
        }
    }
    closebitstream(bs);
    fclose(kmp);
    return 0;
}
