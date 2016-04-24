#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
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

void get_huffman_code(FILE *fp, uint8_t codetable[128]);
int create_freq_table(FILE *fp, struct treenode *ftable);
int node_compar(const void *a, const void *b);
void get_mintwo(struct queue *q1, struct queue *q2, struct treenode *tn[2]);

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
    uint8_t codetable[128];
    get_huffman_code(fp, codetable);
    rewind(fp);
    fclose(fp);
    return 0;
}

void update_codes(struct treenode *root, uint8_t codetable[128],
                  char code[20])
{
    if (root) {
        if (root->symbol != 0) {
            printf("%c(%d): %d - %s %d\n", root->symbol, root->symbol, root->freq, code, strtol(code, NULL, 2));
            codetable[(int) root->symbol] = strtol(code, NULL, 2);
        }
        int l = strlen(code);
        char lcode[20], rcode[20];
        strcpy(lcode, code);
        lcode[l] = '1';
        lcode[l+1] = '\0';
        strcpy(rcode, code);
        rcode[l] = '0';
        rcode[l+1] = '\0';
        update_codes(root->l, codetable, lcode);
        update_codes(root->r, codetable, rcode);
    }
}

void get_huffman_code(FILE *fp, uint8_t codetable[128])
{
    int i;
    /* Get table of frequencies (> 0) of symbols in text */
    struct treenode ftable[128];
    int ftsize = create_freq_table(fp, ftable);
    /* Sort frequency table by frequency */
    qsort(ftable, ftsize, sizeof(struct treenode), node_compar);
    /* Initialize queues */
    struct queue *q1 = initqueue(), *q2 = initqueue();
    /* Enqueue leaf nodes in q1 */
    for (i=0; i<ftsize; i++) {
        enqueue(q1, &ftable[i]);
    }
    printf("%d\n", q1->size);
    /* Form tree using both queues */
    while (q1->size > 1 || q2->size > 1) {
        struct treenode *tn[2];
        get_mintwo(q1, q2, tn);
        struct treenode *internal = malloc(sizeof(struct treenode)); 
        internal->symbol = 0;
        internal->freq = tn[0]->freq + tn[1]->freq;
        internal->l = tn[0];
        tn[0]->p = internal;
        internal->r = tn[1];
        tn[1]->p = internal;
        enqueue(q2, internal);
    }
    /* Set root of tree */
    struct treenode *root = front(q1) ? front(q1) : front(q2);
    char code[10] = "";
    update_codes(root, codetable, code);
    freequeue(q1);
    freequeue(q2);
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

/**
 * Dequeues and returns the two nodes with minumum frequencies from top of 
 * the two queues.
 *
 * @param[in] q1,q2 Pointers to the two queues
 * @param[out] tn Array of the minimum two nodes
 */
void get_mintwo(struct queue *q1, struct queue *q2, struct treenode *tn[2])
{
    struct treenode *temp1, *temp2;
    int i;
    /* tn[0] and tn[1] should be minimum from fronts of both queues */
    for (i=0; i<2; i++) {
        if (!front(q2)) {
            tn[i] = front(q1);
            dequeue(q1);
        } else if (!front(q1)) {
            tn[i] = front(q2);
            dequeue(q2);
        } else {
            temp1 = front(q1);
            temp2 = front(q2);
            if (temp1->freq < temp2->freq) {
                tn[i] = temp1;
                dequeue(q1);
            } else {
                tn[i] = temp2;
                dequeue(q2);
            }
        }
    }
}
