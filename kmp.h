#ifndef KMP_H
#define KMP_H

#define ALPHLEN 256

struct treenode {
    char symbol;
    int freq;
    struct treenode *l;
    struct treenode *r;
    struct treenode *p;
};

struct huffcode {
    int set;
    char code[30];
};

#endif
