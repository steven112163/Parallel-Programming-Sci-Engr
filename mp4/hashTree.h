
struct node{
    struct node* left;
    struct node* right;
    unsigned char* data;
    unsigned char* hash;
    unsigned char* miningProof;
    int status;
} typedef node;

void readTree(node*);
int isThreaded(int, char**);
int isMasterSlave(int, char**);
node* createNode();
void getWallTime(double*);
unsigned char* hashTree(node*);
void* hashTreeParallel(void*);
unsigned char* solvePuzzle(unsigned char*);
unsigned char* hash(unsigned char*, unsigned char**, unsigned char*, unsigned char*);
void printHash(unsigned char*);
void encrypt(unsigned char *, int, unsigned char*, unsigned char *, unsigned char *);
