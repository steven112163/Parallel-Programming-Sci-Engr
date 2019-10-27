CC=gcc
CFLAG= -std=c99 -Wall
CRYPTFLAG = -lcrypto -lssl -lpthread
DEBUGFLAG= -g

all: hashTree hashTree_debug tree
debug: hashTree_debug

hashTree: hashTree.c hashTree.h
	$(CC) $(CFLAG) $(OFLAG) hashTree.h hashTree.c -o hashTree $(CRYPTFLAG)

hashTree_debug: hashTree.c hashTree.h
	$(CC) $(CFLAG) $(DEBUGFLAG) hashTree.h hashTree.c -o hashTree_debug $(CRYPTFLAG)

tree: tree.c tree.h
	$(CC) $(CFLAG) tree.h tree.c -o tree

clean:
	rm -f hashTree tree hashTree_debug