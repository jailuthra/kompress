/**
 * @file
 * Kompress
 */

/* TODO check if file is empty */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "queue.h"
#include "bitstream.h"
#include "kmp.h"

void write_header(FILE *fp, uint8_t padding, struct huffcode codebook[ALPHLEN]);
void get_huffman_code(FILE *fp, struct huffcode codebook[ALPHLEN]);
int create_freq_table(FILE *fp, struct treenode *ftable);
int node_compar(const void *a, const void *b);
void get_mintwo(struct queue *q1, struct queue *q2, struct treenode *tn[2]);
void update_codes(struct treenode *root, struct huffcode codebook[ALPHLEN],
                  char code[MAX_CODELEN]);

int main(int argc, char *argv[])
{
    FILE *fp, *kmp;
    int i;
    if (argc == 3) {
        if (!(kmp = fopen(argv[1], "wb"))) {
            fprintf(stderr, "Couldn't open file %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        if (!(fp = fopen(argv[2], "r"))) {
            fprintf(stderr, "Couldn't open file %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    } else {
        printf("USAGE: %s <outfile.kmp> <infile.txt>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    /* Create Huffman codebook from file */
    struct huffcode codebook[ALPHLEN];
    for (i=0; i<ALPHLEN; i++) {
        codebook[i].set = 0;
    }
    get_huffman_code(fp, codebook);
    printf("CODEBOOK\n");
    for (i=0; i<ALPHLEN; i++) {
        printf("%d - %s\n", i, codebook[i].code);
    }
    /* Re-read input file, write each symbol's huffcode to temp file */
    rewind(fp);
    FILE *temp = tmpfile();
    struct bitstream *bs = initbitstream(temp, BS_WRITE);
    uint8_t byte;
    while (fread(&byte, sizeof(uint8_t), 1, fp) == 1) {
        if(!codebook[byte].set) {
            printf("byte: %d\n", byte);
            assert(0);
        }
        write_bitstring(bs, codebook[byte].code);
    }
    fclose(fp);
    /* Close bitstream, get padding bits in last byte */
    uint8_t padding = closebitstream(bs);
    /* Write header to the output file */
    write_header(kmp, padding, codebook);
    /* Copy bitstream from tempfile to output file */
    rewind(temp);
    while (fread(&byte, sizeof(uint8_t), 1, temp) == 1) {
        fwrite(&byte, sizeof(uint8_t), 1, kmp);
    }
    fclose(temp);
    fclose(kmp);
    return 0;
}

void write_header(FILE *fp, uint8_t padding, struct huffcode codebook[ALPHLEN])
{
    uint8_t byte;
    int n = 0, i, j, k;
    /* Write identifying hex code 0x05A1 */
    putc(0x05, fp);
    putc(0xA1, fp);
    /* Write padding byte */
    fwrite(&padding, sizeof(uint8_t), 1, fp);
    /* Write number of symbols */
    for (i=0; i<ALPHLEN; i++) {
        if (codebook[i].set) {
            n++;
        }
    }
    byte = n; fwrite(&byte, sizeof(uint8_t), 1, fp);
    printf("No. of bytes: %d\n", n);
    printf("Padding bits: %d\n", padding);
    /* Writing the code map */
    uint8_t x, y, l, buf, offset;
    char code[30];
    for (i=0; i<ALPHLEN; i++) {
        if (codebook[i].set) {
            /* Write ascii code */
            byte = i;
            fwrite(&byte, sizeof(uint8_t), 1, fp);
            /* Write huffcode info */
            strcpy(code, codebook[i].code);
            if (strlen(code) % 8 != 0) {
                x = (strlen(code) / 8) + 1; // no. of bytes
                y = 8 - (strlen(code) % 8); // padding bits
            } else {
                x = strlen(code) / 8; // no. of bytes
                y = 0; // padding bits
            }
            assert(x < 64 && y < 64);
            byte = x;
            byte = (byte << 4) | y;
            fwrite(&byte, sizeof(uint8_t), 1, fp);
            /* Write huffcode */
            l = strlen(code);
            for (j=0; j<y; j++) {
                code[l + j] = '0';
            }
            code[l + j] = '\0';
            assert(strlen(code) % 8 == 0);
            assert(strlen(code) / 8 == x);
            offset = 0;
            byte = 0;
            for (j=0; j<strlen(code); j++) {
                byte = (byte << 1) | ((int) (code[j] - '0'));
                offset++;
                if (offset == 8) {
                    fwrite(&byte, sizeof(uint8_t), 1, fp);
                    byte = 0;
                    offset = 0;
                }
            }
        }
    }
}

void update_codes(struct treenode *root, struct huffcode codebook[ALPHLEN],
                  char code[MAX_CODELEN])
{
    if (root) {
        if (!root->is_internal) {
            codebook[(int) root->symbol].set = 1;
            strcpy(codebook[(int) root->symbol].code, code);
        }
        int l = strlen(code);
        char lcode[MAX_CODELEN], rcode[MAX_CODELEN];
        strcpy(lcode, code);
        lcode[l] = '0';
        lcode[l+1] = '\0';
        strcpy(rcode, code);
        rcode[l] = '1';
        rcode[l+1] = '\0';
        update_codes(root->l, codebook, lcode);
        update_codes(root->r, codebook, rcode);
    }
}

/** Create a huffman codebook from the input stream.
 * @param[in] fp Pointer to input stream
 * @param[out] codebook Huffman codebook
 */
void get_huffman_code(FILE *fp, struct huffcode codebook[ALPHLEN])
{
    int i;
    /* Get table of frequencies (> 0) of symbols in text */
    struct treenode ftable[ALPHLEN];
    int ftsize = create_freq_table(fp, ftable);
    /* Sort frequency table by frequency */
    qsort(ftable, ftsize, sizeof(struct treenode), node_compar);
    /* Initialize queues */
    struct queue *q1 = initqueue(), *q2 = initqueue();
    /* Enqueue leaf nodes in q1 */
    for (i=0; i<ftsize; i++) {
        enqueue(q1, &ftable[i]);
    }
    /* Form tree using both queues */
    while (q1->size > 1 || q2->size > 1) {
        struct treenode *tn[2];
        get_mintwo(q1, q2, tn);
        struct treenode *internal = malloc(sizeof(struct treenode)); 
        internal->symbol = 0;
        internal->freq = tn[0]->freq + tn[1]->freq;
        internal->l = tn[0];
        internal->is_internal = 1;
        tn[0]->p = internal;
        internal->r = tn[1];
        tn[1]->p = internal;
        enqueue(q2, internal);
    }
    /* Set root of tree */
    struct treenode *root = front(q1) ? front(q1) : front(q2);
    char code[MAX_CODELEN] = "";
    update_codes(root, codebook, code);
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
    int freq[ALPHLEN], i;
    uint8_t byte;
    /* set initial frequencies to 0 */
    for (i=0; i<ALPHLEN; i++) {
        freq[i] = 0;
    }
    /* update frequencies of each ascii value from file */
    while (fread(&byte, sizeof(uint8_t), 1, fp) == 1) {
        freq[byte] += 1;
    }
    /* update table */
    struct treenode *node;
    int top = 0;
    for (i=0; i<ALPHLEN; i++) {
        if (freq[i] != 0) {
            node = &ftable[top++];
            node->symbol = (char) i;
            node->is_internal = 0;
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
            if (temp1->freq <= temp2->freq) {
                tn[i] = temp1;
                dequeue(q1);
            } else {
                tn[i] = temp2;
                dequeue(q2);
            }
        }
    }
}
