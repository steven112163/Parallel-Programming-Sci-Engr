#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tree.h"
#define DATA_SIZE 40960

int main(int argc, char** argv){
    int numNodes = atoi(argv[1]);
    if(numNodes < 1){
        printf("\nNumber of nodes > 1");
        exit(1);
    }
    node* head = createNode();
    createTree(head, numNodes-1);
    serializeTree(head);

    return 0;
}

void createTree(node* curr, int numNodes){
    if(numNodes == 0) return;

    //int leftPercent = rand() % 100;
    int leftPercent = 50;
    int leftSize = (int)(((float) leftPercent / 100) * (float)numNodes);
    int rightSize = numNodes - leftSize;

    if(leftSize > 0){
        curr->left = createNode();
        createTree(curr->left, leftSize-1);
    }

    if(rightSize > 0){
        curr->right = createNode();
        createTree(curr->right, rightSize-1);
    }
}

void serializeTree(node* curr){
    if(curr == NULL){
        return;
    }
    printNode(curr);
    serializeTree(curr->left);
    serializeTree(curr->right);
}

void printNode(node* curr){
    char left = curr->left ? 'P': 'N';
    char right = curr->right ? 'P': 'N';

    printf("%c %c %s\n",left, right, curr->data);
}

node* createNode(){
    node* newNode = (node*) malloc(sizeof(node));
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->data = (char*) malloc(DATA_SIZE * sizeof(char));

    for(int n=0; n < DATA_SIZE - 1; n++){
        newNode->data[n] = (char)(rand() % 26 + 97);
    }
    newNode->data[DATA_SIZE - 1] = '\0';
    return newNode;
}