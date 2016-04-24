#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "queue.h"

struct treenode {
    char symbol;
    int freq;
    struct treenode *l;
    struct treenode *r;
    struct treenode *p;
};

struct huffcode {
    char symbol;
    uint8_t code;
};

void get_huffman_code(FILE *fp);
int create_freq_table(FILE *fp, struct treenode *ftable);
int node_compar(const void *a, const void *b);

int main(int argc, char *argv[])
{
    FILE *fp;
    if (argc == 2) {
        if (!(fp = fopen(argv[1], "r"))) {
            fprintf(stderr, "Couldn't open file %s\n", argv[1]);
        }
    } else {
        /* STDIN mode */
        fp = stdin;
    }
    /* Huffman Code */
    get_huffman_code(fp);
    rewind(fp);
    fclose(fp);
    return 0;
}

void get_huffman_code(FILE *fp)
{
    /* Get table of frequencies (> 0) of symbols in text */
    struct treenode ftable[128];
    int ftsize = create_freq_table(fp, ftable);
    /* Sort frequency table by frequency */
    qsort(ftable, ftsize, sizeof(struct treenode), node_compar);
    /* Print sorted table */
    int i;
    for (i=0; i<ftsize; i++) {
        printf("%c (%d): %d\n", ftable[i].symbol, ftable[i].symbol, ftable[i].freq);
    }
}

/** Create a frequency table from the input stream.
 *
 * @param[in] fp Pointer to input stream
 * @param[out] ftable Frequency table
 * @retval ftsize Number of elements in ftable
 */
int create_freq_table(FILE *fp, struct treenode *ftable)
{
    int freq[128], i;
    char c;
    /* set initial frequencies to 0 */
    for (i=0; i<128; i++) {
        freq[i] = 0;
    }
    /* update frequencies of each ascii value from file */
    while ((c = fgetc(fp)) != EOF) {
        freq[(int) c] += 1;
    }
    /* update table */
    struct treenode *node;
    int top = 0;
    for (i=0; i<127; i++) {
        if (freq[i] != 0) {
            node = &ftable[top++];
            node->symbol = (char) i;
            node->freq = freq[i];
            node->l = node->r = NULL;
        }
    }
    return top;
}

int node_compar(const void *a, const void *b)
{
    const struct treenode *na = a, *nb = b;
    if (na->freq < nb->freq) {
        return -1;
    } else if (na->freq > nb->freq) {
        return 1;
    } else {
        return 0;
    }
}
