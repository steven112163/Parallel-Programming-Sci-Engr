
struct node{
    struct node* left;
    struct node* right;
    char* data;
} typedef node;

node* createNode();
void serializeTree(node*);
void printNode(node*);
void createTree(node*, int);
