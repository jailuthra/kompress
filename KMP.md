KMP File Format
===============

Kompress file header
--------------------

| byte 1 | byte 2 | byte 3           | byte 4             | huffcode map (1-n) | 
| ------ | ------ | ---------------- | ------------------ | ------------------ | 
| 0x05   | 0xA1   | padding bits (p) | no. of symbols (n) | huffcode map data  | 

The padding **p** here is the amount of unused bits in the last byte of bitstream.
The decoder needs this information to know when to stop reading bits.

The hexcode **0x05A1** is the indicator that the bitstream will follow
KMP format guidelines. It was chosen because it's similar to the author's
first name, and also because it is a palindrome (1441) in base 10.
Palindromes are cool.

### Huffcode map data format

| byte 1       | nibble (byte 2)       | nibble (byte 2)       | x bytes  |
| ------------ | --------------------- | --------------------- | -------- |
| symbol ascii | code length (x bytes) | padding bits (y bits) | huffcode |

The huffman bitcode for a symbol might not have a length that is divisible by 8,
thus a byte representation might need padding (**y**) in the last byte.

Kompress file bitstream
-----------------------

Each character *c* read from the original file is converted into its huffcode and
appended to the bitstream. Finally the bitstream is written to the KMP file, in units
of one byte. (Thus last byte may have a padding of 0-7 bits)
