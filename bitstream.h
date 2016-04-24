#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stdio.h>

struct bitstream {
    uint8_t buf;
    int bufoffset;
    FILE *fp;
    long offset;
};

/** Initialize a bitstream.
 * @param[in]   fp  Pointer to file in "wb" mode
 * @retval      bs  Pointer to bitstream context
 */
struct bitstream * initbitstream(FILE *fp);

/** Close bitstream.
 * Frees the bitstream context, and returns the padding.
 * Note: It doesn't close the file
 * @param[in]   bs      Pointer to bitstream context
 * @retval      padding Number of bits padded with 0 in the last
 *                      byte written to the file
 */
int closebitstream(struct bitstream *bs);

/** Write a bit character to the stream. **/
void write_bit(struct bitstream *bs, char c);

/** Write a bitstream string to the stream. **/
void write_bitstring(struct bitstream *bs, char *str);

#endif
