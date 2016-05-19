/**
 * @file
 * Bitstream lib header
 */

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdint.h>
#include <stdio.h>

enum bs_type {
    BS_READ,
    BS_WRITE
};

struct bitstream {
    uint8_t buf;        ///< buffer
    int bufoffset;      ///< offset (used bits in buffer)
    FILE *fp;           ///< file pointer
    enum bs_type type;  ///< bitstream type {BS_READ, BS_WRITE}
    int padding;        ///< padding bits in last byte
    int last;           ///< is current byte the last one
};

/** Initialize a bitstream.
 * @param[in]   fp      Pointer to file
 * @param[in]   type    Type of bitstream {BS_READ, BS_WRITE}
 * @param[in]   padding Optional parameter, to be sent if type is BS_READ
 * @retval      bs      Pointer to bitstream context
 */
struct bitstream * initbitstream(FILE *fp, enum bs_type type, ...);

/** Close bitstream.
 * Frees the bitstream context, and returns the padding.
 * Note: It doesn't close the file stream
 * @param[in]   bs      Pointer to bitstream context
 * @retval      padding Number of bits padded with 0 in the last
 *                      byte written to the file
 *                      NB: If type is BS_READ, returns 0
 */
int closebitstream(struct bitstream *bs);

/** Write a bit character to the stream. **/
void write_bit(struct bitstream *bs, char c);

/** Write a bitstream string to the stream. **/
void write_bitstring(struct bitstream *bs, char *str);

/** Read a bit (int) from the bitstream **/
int read_bit(struct bitstream *bs);

#endif
