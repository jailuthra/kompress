/**
 * @file
 * Bitstream lib
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include "bitstream.h"

static int fpeek(FILE *stream)
{
    int c;
    c = fgetc(stream);
    ungetc(c, stream);
    return c;
}

struct bitstream * initbitstream(FILE *fp, enum bs_type type, ...)
{
    struct bitstream *bs = malloc(sizeof(struct bitstream));
    bs->fp = fp;
    bs->buf = 0;
    bs->bufoffset = 0;
    bs->type = type;
    bs->last = 0;
    va_list ap;
    va_start(ap, type);
    if (type == BS_READ) {
        bs->padding = va_arg(ap, int);
    }
    va_end(ap);
    return bs;
}

void write_bit(struct bitstream *bs, char c)
{
    assert(c == '0' || c == '1');
    assert(bs->type == BS_WRITE);
    assert(bs->bufoffset < 8);
    /* Write bit to buffer */
    bs->buf = (bs->buf << 1) | ((int) (c - '0'));
    bs->bufoffset += 1;
    /* If buffer full, write it to file */
    if (bs->bufoffset == 8) {
        fwrite(&bs->buf, sizeof(uint8_t), 1, bs->fp);
        bs->buf = 0;
        bs->bufoffset = 0;
    }
}

int read_bit(struct bitstream *bs)
{
    assert(bs->type == BS_READ);
    /* If buffer empty, read from file */
    if (bs->bufoffset == 0) {
        fread(&bs->buf, sizeof(uint8_t), 1, bs->fp);
        bs->bufoffset = 8;
        if (fpeek(bs->fp) == EOF) {
            bs->last = 1;
        }
    }
    /* Check if last byte */
    if (bs->last) {
        /* Don't read padding bits */
        if (bs->bufoffset <= bs->padding) {
            return -1;
        }
    }
    int bit = bs->buf >> (bs->bufoffset - 1);
    bs->buf &= ~(1 << (bs->bufoffset - 1));
    bs->bufoffset -= 1;
    assert(bit == 1 || bit == 0);
    return bit;
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
    if (bs->type == BS_WRITE) {
        int padding = 0;
        if (bs->bufoffset > 0) {
            padding = 8 - bs->bufoffset;
            bs->buf <<= padding;
            fwrite(&bs->buf, sizeof(uint8_t), 1, bs->fp);
        }
        free(bs);
        return padding;
    } else {
        free(bs);
        return 0;
    }
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
