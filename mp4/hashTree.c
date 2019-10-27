#include <stdio.h>
#include <openssl/sha.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <sys/time.h>
#include "hashTree.h"

#define DATA_SIZE 40960
#define EN_DATA_SIZE DATA_SIZE + 16

/* A 256 bit key */
unsigned char *AES_KEY = (unsigned char *)"01234567890123456789012345678901";
/* A 128 bit IV */
unsigned char *IV = (unsigned char *)"0123456789012345";

pthread_mutex_t lock;

int main(int argc, char** argv){
    node* head = createNode();
    readTree(head);
    unsigned char* treeHash = NULL;
    int threadCount = isThreaded(argc, argv);

    double start, end = 0;
    getWallTime(&start);
    srand(time(0));

    if(threadCount == -1){
        treeHash = hashTree(head);
    }
    else{
        if(isMasterSlave(argc, argv) == -1){
            //TODO: Parallelize hash tree
            pthread_t threads[threadCount];
            int half = (int)threadCount / 2;
            for(int i = 0; i < half; i++)
                pthread_create(&threads[i], NULL, hashTreeParallel, head->left);
            for(int i = half; i < threadCount; i++)
                pthread_create(&threads[i], NULL, hashTreeParallel, head->right);
            
            for(int i = 0; i < threadCount; i++)
                pthread_join(threads[i], NULL);
            
            treeHash = hash(head->data, &head->miningProof, head->left->hash, head->right->hash);
        }
        else{
            //TODO: Master Slave hash tree(4 credit students only)
        }
    }
    getWallTime(&end);
    printHash(treeHash);
    double timeTaken = end - start;
    printf("\n%lfs\n", timeTaken);
    return 0;
}

void readTree(node* curr){
    char left, right = 'N';
    scanf("%c %c %s\n", &left, &right, curr->data);
    if(left == 'P'){
        curr->left = createNode();
        readTree(curr->left);
    }
    if(right == 'P'){
        curr->right = createNode();
        readTree(curr->right);
    }
}

void getWallTime(double *wcTime){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    *wcTime = (double) (tp.tv_sec + tp.tv_usec / 1000000.0);
}

int isThreaded(int argc, char** argv){
    int threadCount = -1;
    for(int n=1; n < argc; n++){
        if(strcmp(argv[n], "-t") == 0){
            threadCount = atoi(argv[n+1]);
            break;
        }
    }
    return threadCount;
}

int isMasterSlave(int argc, char** argv){
    int isMaster = -1;
    for(int n=1; n < argc; n++){
        if(strcmp(argv[n], "-m") == 0){
            isMaster = 1;
            break;
        }
    }
    return isMaster;
}

node* createNode(){
    node* newNode = (node*) malloc(sizeof(node));
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->hash = NULL;
    newNode->data = (unsigned char*) malloc(DATA_SIZE * sizeof(unsigned char));
    newNode->miningProof = NULL;
    newNode->status = 0;
    return newNode;
}

unsigned char* hashTree(node* curr) {
    //TODO: Single threaded hash tree
    if(!curr->left && !curr->right) {
        curr->hash = hash(curr->data, &curr->miningProof, NULL, NULL);
        return curr->hash;
    } else if(!curr->left) {
        unsigned char* rightHash = hashTree(curr->right);
        curr->hash = hash(curr->data, &curr->miningProof, NULL, rightHash);
        return curr->hash;
    } else if(!curr->right) {
        unsigned char* leftHash = hashTree(curr->left);
        curr->hash = hash(curr->data, &curr->miningProof, leftHash, NULL);
        return curr->hash;
    }
    
    unsigned char* rightHash = hashTree(curr->right);
    unsigned char* leftHash = hashTree(curr->left);
    curr->hash = hash(curr->data, &curr->miningProof, leftHash, rightHash);
    return curr->hash;
}

void* hashTreeParallel(void* node_curr){
    //TODO: Parallel hash tree
    node* curr = (node*) node_curr;
    if(curr->status == 1)
        return curr->hash;
    
    if(!curr->left && !curr->right) {
        pthread_mutex_lock(&lock);
        curr->hash = hash(curr->data, &curr->miningProof, NULL, NULL);
        curr->status = 1;
        pthread_mutex_unlock(&lock); 
        return curr->hash;
    } else if(!curr->left) {
        unsigned char* rightHash = hashTree(curr->right);
        pthread_mutex_lock(&lock);
        curr->hash = hash(curr->data, &curr->miningProof, NULL, rightHash);
        curr->status = 1;
        pthread_mutex_unlock(&lock); 
        return curr->hash;
    } else if(!curr->right) {
        unsigned char* leftHash = hashTree(curr->left);
        pthread_mutex_lock(&lock);
        curr->hash = hash(curr->data, &curr->miningProof, leftHash, NULL);
        curr->status = 1;
        pthread_mutex_unlock(&lock); 
        return curr->hash;
    }
    
    short rightOrLeft = rand() % 2;
    unsigned char* rightHash; 
    unsigned char* leftHash;
    if(rightOrLeft == 1) {
        rightHash = hashTree(curr->right);
        leftHash = hashTree(curr->left);
    } else {
        leftHash = hashTree(curr->left);
        rightHash = hashTree(curr->right);
    }
    pthread_mutex_lock(&lock);
    curr->hash = hash(curr->data, &curr->miningProof, leftHash, rightHash);
    curr->status = 1;
    pthread_mutex_unlock(&lock);
    return curr->hash;
}

unsigned char* hash(unsigned char* data, unsigned char** miningProof, unsigned char* lHash, unsigned char* rHash){
    unsigned char encryptedData[EN_DATA_SIZE];
    /* Encrypt the plaintext */
    encrypt (data, DATA_SIZE, AES_KEY, IV, encryptedData);

    unsigned char* hashResult = (unsigned char*) malloc(SHA256_DIGEST_LENGTH * sizeof(unsigned char));
    unsigned char branchData[SHA256_DIGEST_LENGTH + SHA256_DIGEST_LENGTH + EN_DATA_SIZE];
    int index = 0;
    if(lHash != NULL){
        memcpy(branchData + index, lHash, SHA256_DIGEST_LENGTH);
        index += SHA256_DIGEST_LENGTH;
    }

    if(rHash != NULL){
        memcpy(branchData + index, rHash, SHA256_DIGEST_LENGTH);
        index += SHA256_DIGEST_LENGTH;
    }
    memcpy(branchData + index, encryptedData, EN_DATA_SIZE);
    index += EN_DATA_SIZE;
    (*miningProof) = solvePuzzle(encryptedData);
    return SHA256(branchData, index, hashResult);
}

unsigned char* solvePuzzle(unsigned char *data){
    unsigned char nouncedEncryptedData[EN_DATA_SIZE+1];
    memcpy(nouncedEncryptedData+1, data, EN_DATA_SIZE);
    unsigned char* hashResult = (unsigned char*) malloc(SHA256_DIGEST_LENGTH * sizeof(unsigned char));

    for(int n=0; n < 256; n++){
        unsigned char nounce = (unsigned char)n;
        nouncedEncryptedData[0] = nounce;
        SHA256(nouncedEncryptedData, EN_DATA_SIZE+1, hashResult);
    }
    return hashResult;
}

void encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();;
    int len;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    EVP_CIPHER_CTX_free(ctx);
}

void printHash(unsigned char* hash){
    for(int n = 0; n < SHA256_DIGEST_LENGTH; n++) {
        printf("%02x", (unsigned char) hash[n]);
    }
}