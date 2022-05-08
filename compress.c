#include <stdlib.h>
#include <string.h>
#include "common.h"

/**
 * Build a Huffman tree given a file
 * @param fileName
 * @return tree
 */
struct Tree *buildTree(char *fileName) {
    FILE *file = fopen(fileName, "r");
    if(file == NULL) {
        fprintf(stderr, "Could not open file %s \n", fileName);
        exit(1);
    }
    int count[256] = { 0 };
    int charCount = 0;
    int leafCount = 0;
    for(int c = getc(file); c != EOF; c = getc(file)) {
        charCount++;
        if(count[c] == 0)
            leafCount++;
        count[c]++;
    }

    printf("charCount= %d, leafCount= %d \n", charCount, leafCount);
    fclose(file);

    // Check if the file contains more than a single character
    if(leafCount == 1) {
        fprintf(stderr, "[Error] - It is impossible to build the Huffman tree with a single character.");
        exit(1);
    }

    Tree *t = malloc(sizeof(struct Tree));
    t->size = (leafCount * 2) - 1;
    t->table = malloc(sizeof(struct Node) * t->size);

    // Build the beginning of the table/tree
    int tableIndex = 0;
    for(int i = 0; i < 256; i++) {
        if(count[i] != 0) {
            t->table[tableIndex].character = i;
            t->table[tableIndex].occurrences = count[i];
            t->table[tableIndex].left = t->table[tableIndex].right = t->table[tableIndex].parent = -1;
            tableIndex++;
        }
    }

    // Check if the file is encoded in ASCII
    for(int i = 0; i < tableIndex; i++) {
        if(t->table[i].character < 0) {
            fprintf(stderr, "[Error] - This compressor is exclusively for ASCII encoded files while the file %s is not\n", fileName);
            exit(1);
        }
    }

    for(int i = 0; i < tableIndex; i++) {
        printf("id: %d, char: %c, occ: %d \n", t->table[i].character, t->table[i].character, t->table[i].occurrences);
    }

    // Creating tree starting from characters with less occurrences
    for(int i = tableIndex; i < t->size; i++) {
        int minNode1 = charCount, iMinNode1;
        int minNode2 = charCount, iMinNode2;
        for(int j = 0; j < i; j++) {
            if(t->table[j].parent == -1 && t->table[j].occurrences < minNode1) {
                minNode2 = minNode1;
                iMinNode2 = iMinNode1;
                minNode1 = t->table[j].occurrences;
                iMinNode1 = j;
            }
            else if(t->table[j].parent == -1 && t->table[j].occurrences < minNode2) {
                minNode2 = t->table[j].occurrences;
                iMinNode2 = j;
            }
        }
        // Creating the node
        t->table[i].left = iMinNode1;
        t->table[i].right = iMinNode2;
        t->table[i].occurrences = minNode1 + minNode2;
        t->table[i].parent = -1;
        t->table[i].character = NULL;
        // Setting parent for the two leaves
        t->table[iMinNode1].parent = i;
        t->table[iMinNode2].parent = i;

        printf("node: %d | leftChild: %c | rightChild: %c \n", i, t->table[iMinNode1].character, t->table[iMinNode2].character);
    }
    return t;
}

/**
 * Appends a character at the end of a string
 * @param string
 * @param char
 * @return string with character c at the end
 */
char *append(char *string, char c) {
    char *finalString = malloc(strlen(string) + 2);
    strcpy(finalString, string);
    finalString[strlen(string)] = c;
    finalString[strlen(string) + 1] = '\0';
    return finalString;
}

/**
 * Convert char to binary code and
 * setting each code into an array
 * @param tree
 * @param nodeIndex
 * @param binaryCode
 * @param codes
 */
void generateCodes(struct Tree *t, int nodeIndex, char *binaryCode, char **codes) {
    // Write binary code in the array if the node is a leaf
    if(t->table[nodeIndex].left == -1 && t->table[nodeIndex].right == -1) {
        printf("char: %c, code: %s\n", t->table[nodeIndex].character, binaryCode);
        codes[t->table[nodeIndex].character] = binaryCode;
        return;
    }
    // Iteration allowing to set a binary code for each node
    // Left son of each node appends '0', right son appends '1'
    generateCodes(t, t->table[nodeIndex].left, append(binaryCode, '0'), codes);
    generateCodes(t, t->table[nodeIndex].right, append(binaryCode, '1'), codes);
}

/**
 * Write the code of each char
 * at the top of the file
 * @param file
 * @param tree
 */
void writeHeaders(FILE *file, Tree *t) {
    fprintf(file, "%d\n", (t->size + 1) / 2); // Number of distinct characters
    for(int i = 0; i < t->size; i++) {
        if(t->table[i].left == -1 && t->table[i].right == -1)
            fprintf(file, "%c%d\n", t->table[i].character, t->table[i].occurrences);
    }
}

// Defined as global to allow us to
// write missing bits in the output file
static int bitCounter;

/**
 * Write byte in the output file
 * @param file
 * @param code
 */
void writeBit(FILE *file, int bit) {
    static char byte;
    byte = byte | bit;
    bitCounter++;
    if(bitCounter == 8) {
        bitCounter = 0;
        fwrite(&byte, sizeof(char), 1, file);
        byte = 0;
    }
    else {
        byte = byte << 1;
    }
}

/**
 * Convert chars in file with binary code
 * and write it in the output
 * @param inputFile
 * @param outputFile
 * @param codes
 * @param tree
 */
void encodeFile(char *inputFileName, char *outputFileName, char **codes, Tree *t) {
    FILE *input = fopen(inputFileName, "r");
    FILE *output = fopen(outputFileName, "wb");
    writeHeaders(output, t);
    // Convert each char of the input file with its
    // binary code and write it in the output file
    printf("Converting string to bits and writing in file\n");
    for(char c = fgetc(input); c != EOF; c = fgetc(input)) {
        for(int i = 0; i < strlen(codes[c]); i++) {
            writeBit(output, (codes[c][i] - '0'));
        }
    }
    // If the last byte is not complete (len(byte) < 8)
    // add bits at the end of it to complete it
    if(bitCounter != 0) {
        int k = bitCounter;
        while(k < 8) {
            writeBit(output, 0);
            k++;
        }
    }
    fclose(output);
    fclose(input);
}

/**
 * Return the size of a file in bytes
 * @param fileName
 * @return size
 */
long int getFileSize(char *fileName) {
    FILE *file = fopen(fileName, "r");
    long int size;
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    fclose(file);
    return size;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: %s <inputFile> <outputFile>\n", argv[0]);
        return 1;
    }
    printf("Starting...\n");
    Tree *t = buildTree(argv[1]);
    // Initialize the array which will contain the binary codes for each char
    char *codes[((t->size + 1) / 2)];
    for(int i = 0; i < (sizeof(codes) / sizeof(const char *)); i++) {
        if(t->table[i].character != NULL)
            codes[t->table[i].character] = "";
    }
    int rootIndex = getRootIndex(t);
    printf("Root node index: %d\n", rootIndex);
    generateCodes(t, rootIndex, "", codes);
    encodeFile(argv[1], argv[2], codes, t);

    float k = (getFileSize(argv[2]) * 100.00) / getFileSize(argv[1]);
    printf("Original size: %ld B, Compressed size: %ld B, Compression: %f \%\n", getFileSize(argv[1]), getFileSize(argv[2]), (100 - k));
    return 0;
}
