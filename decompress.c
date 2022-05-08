#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"

/**
 * Build tree given a list of characters
 * and their frequency in a file
 * @param count
 * @param leafCount
 * @param charCount
 * @return tree
 */
struct Tree *buildTree(const int *count, int leafCount, int charCount) {
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
    }
    return t;
}

/**
 * Read bits of the input file
 * @param file
 * @return 0 or 1
 */
unsigned int readBit(FILE *file) {
    static unsigned char byte;
    static int bitCount;
    if(bitCount == 0) {
        fread(&byte, sizeof(char), 1, file);
        if(feof(file))
            return 2;
        bitCount = 8;
    }
    unsigned int bit = byte >> 7; // First unread bit of the byte
    byte = byte << 1;
    bitCount--;
    return bit;
}

/**
 * Read the characters originally contained in the
 * file and their frequency in the file
 * (Inspired from the internet)
 * @param file
 * @param count
 */
void readHeaders(FILE *file, int *count) {
    // Counts the number of distinct characters contained in the original file
    char c;
    int charCount = 0;
    while((c = fgetc(file)) != '\n') {
        charCount = charCount * 10 + (c - '0');
    }
    printf("Distinct chars count: %d\n", charCount);
    for(int i = 0; i < charCount; i++) {
        char index = fgetc(file);
        int occ = 0;
        while ((c = fgetc(file)) != '\n') {
            occ = occ * 10 + (c - '0');
        }
        count[(int) index] = occ;
    }
}

/**
 * Converts and writes in the output the character
 * corresponding to a binary code by
 * following a path in the Huffman Tree
 * @param file
 * @param tree
 * @param rootNode
 */
void decode(FILE *file, FILE *outfile, struct Tree *t, struct Node rootNode) {
    printf("Decompressing file...\n");
    Node node = rootNode;
    unsigned int bit = readBit(file);
    while(bit < 2) {
        if (node.character != NULL) {
            fprintf(outfile, "%c", node.character);
            node = rootNode;
        }
        if (bit == 0)
            node = t->table[node.left];
        else
            node = t->table[node.right];
        bit = readBit(file);
    }
}

/**
 * Decode the file given in parameters
 * coded using Huffman method
 * @param fileName
 */
void decodeFile(char *fileName, char *outfileName) {
    FILE *file = fopen(fileName, "r");
    FILE *outfile = fopen(outfileName, "w+");
    int count[256] = { 0 };
    if(file == NULL) {
        fprintf(stderr, "Could not open file %s \n", fileName);
        exit(1);
    }
    readHeaders(file, count);
    int leafCount = 0;
    int charCount = 0;
    for(int i = 0; i < 256; i++) {
        if(count[i] != 0) {
            leafCount++;
            charCount += count[i];
        }
    }
    Tree *t = buildTree(count, leafCount, charCount);
    int rootIndex = getRootIndex(t);
    printf("Root node index: %d\n", rootIndex);
    Node rootNode = t->table[rootIndex];
    decode(file, outfile, t, rootNode);
    fclose(file);
    fclose(outfile);
}

int main(int argc, char *argv[]) {
    // If the command's syntax is wrong
    if(argc != 3) {
        printf("Usage: %s <fileName> <outfile>\n", argv[0]);
        return 1;
    }
    printf("Starting...\n");
    decodeFile(argv[1], argv[2]);
    return 0;
}
