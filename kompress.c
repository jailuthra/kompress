#include<stdio.h>

int main(int argc, char *argv[])
{
    FILE *fptr;
    if (argc == 2) {
        if (!(fptr = fopen(argv[1], "r"))) {
            fprintf(stderr, "Couldn't open file %s\n", argv[1]);
        }
    } else {
        // STDIN mode
        fptr = stdin;
    }
    int freq[128], i;
    char c;
    for (i=0; i<127; i++) {
        freq[i] = 0;
    }
    while ((c = fgetc(fptr)) != EOF) {
        freq[(int) c] += 1;
    }
    for (i=0; i<127; i++) {
        if (freq[i] != 0) {
            printf("%c (%d): %d\n", (char) i, i, freq[i]);
        }
    }
    return 0;
}
