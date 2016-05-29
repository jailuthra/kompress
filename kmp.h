/**
 * @file
 * KMP Header
 */

#ifndef KMP_H
#define KMP_H

#define ALPHLEN 256
#define MAX_CODELEN 40

struct treenode {
    uint8_t symbol;
    int freq;
    int is_internal;
    struct treenode *l;
    struct treenode *r;
    struct treenode *p;
};

struct huffcode {
    int set;
    char code[MAX_CODELEN];
};

#endif
