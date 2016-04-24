#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bitstream.h"

struct bitstream * initbitstream(FILE *fp)
{
    struct bitstream *bs = malloc(sizeof(struct bitstream));
    bs->fp = fp;
    bs->buf = 0;
    bs->offset = bs->bufoffset = 0;
    return bs;
}

void write_bit(struct bitstream *bs, char c)
{
    assert(c == '0' || c == '1');
    assert(bs->bufoffset < 8);
    printf("Writing %c to buffer\n", c);
    /* Write bit to buffer */
    bs->buf = (bs->buf << 1) | ((int) (c - '0'));
    bs->bufoffset += 1;
    /* If buffer full, write it to file */
    if (bs->bufoffset == 8) {
        fwrite(&bs->buf, sizeof(uint8_t), 1, bs->fp);
        printf("Writing buffer %x\n", bs->buf);
        bs->buf = 0;
        bs->bufoffset = 0;
        bs->offset += 8;
    }
}

void write_bitstring(struct bitstream *bs, char *str)
{
    while (*str) {
        write_bit(bs, *str);
        str++;
    }
}

int closebitstream(struct bitstream *bs)
{
    int padding = 0;
    if (bs->bufoffset > 0) {
        padding = 8 - bs->bufoffset;
        bs->buf <<= padding;
        fwrite(&bs->buf, sizeof(uint8_t), 1, bs->fp);
        printf("Writing buffer %x\n", bs->buf);
    }
    free(bs);
    return padding;
}

#ifdef RUN_MAIN
int main()
{
    FILE *fp = fopen("out.kmp", "wb");
    struct bitstream *bs = initbitstream(fp);
    write_bitstring(bs, "101100100010101001000100101001");
    printf("padding %d\n", closebitstream(bs));
    return 0;
}
#endif
