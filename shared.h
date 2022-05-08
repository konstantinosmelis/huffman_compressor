#include <stdio.h>

/**
 * Nodes contain information about:
 * - the index of their left and right son
 * - the index of their parent node
 * - the character they represent in the tree (cannot be left empty)
 * - the number of occurrences of their character in the file to be compressed
 */
typedef struct Node{
    int occurrences; // Occurrences of character in the file
    int left; // Left son index
    int right; // Right son index
    int parent; // Parent index
    char character; // Represented character
} Node;

/**
 * The tree contains all the created nodes
 * and has a very specific size:
 * size=(leafCount*2)-1
 */
typedef struct Tree{
    Node *table; // Array that will contain every node and its information
    int size; // Number of nodes contained in the table
} Tree;

/**
 * Returns the position of the root of the tree
 * which mean that it has no parents (-1)
 * in the binary tree
 * @param t
 * @return rootIndex
 */
int getRootIndex(struct Tree *t) {
    int rootIndex;
    for(int i = 0; i < t->size; i++) {
        if(t->table[i].parent == -1)
            rootIndex = i;
    }
    return rootIndex;
}
