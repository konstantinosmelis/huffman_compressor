# Huffman compression and decompression
A minimal compression program using Huffman's algorithm, written in C.

## Theory
Let's say we have the following string:

`AAABCBDBDDDDDDEEEFFGEGGGEEEEEECCAAAAA`

| Character | Frequency |
|:---------:|:---------:|
|     A     |     8     |
|     B     |     3     |
|     C     |     3     |
|     D     |     7     |
|     E     |     10    |
|     F     |     2     |
|     G     |     4     |

The above data will generate a binary tree starting from the characters having the lowest frequency, and construct till we use all the characters.

| Character | Frequency | Huffman Code |
|:---------:|:---------:|:------------:|
|     A     |     8     |      01      |
|     B     |     3     |      1101    |
|     C     |     3     |      000     |
|     D     |     7     |      111     |
|     E     |     10    |      10      |
|     F     |     2     |      1100    |
|     G     |     4     |      001     |

We can replace then each character of the string by it's binary code.

## Build

```bash
make all
```

### Compression
```bash
gcc -c compress.c -o obj/compress.o
gcc obj/compress.o -o bin/compress
```

### Decompression
```bash
gcc -c decompress.c -o obj/decompress.o
gcc obj/decompress.o -o bin/decompress
```