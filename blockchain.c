#include "libc.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "openssl/crypto.h"
#include "openssl/sha.h"

#define SHA256_DIGEST_LENGTH 32

struct block
{
	unsigned char prevHash[SHA256_DIGEST_LENGTH];
	int blockData;
	struct block *link;
}*head;

void addBlock(int);
void verifyChain();
void alterNthBlock(int,int);
void hackChain();
int hashCompare(unsigned char *, unsigned char *);
void hashPrinter();
unsigned char *toString(struct block);
void printBlock(struct block*);
void printAllBlocks();

// SHA256(src, size, dest)
// src and dest are of type unsigned char *

void addBlock(int data)
{
	if (!head)
	{
		head=malloc(sizeof(struct block));
		SHA256("",sizeof(""),head->prevHash);
		head->blockData=data;
		return;
	}
	struct block *currentBlock=head;
	while(currentBlock->link)
		currentBlock=currentBlock->link;
	struct block *newBlock=malloc(sizeof(struct block));
	currentBlock->link=newBlock;
	newBlock->blockData=data;
	SHA256(toString(*currentBlock),sizeof(*currentBlock),newBlock->prevHash);
}

void verifyChain()
{
	if(!head)
	{
		printf("Blockchain is empty! Try again after adding some blocks!\n");
		return;
	}
	struct block *curr=head->link,*prev=head;
	int count=1;
	while(curr)
	{
		printf("%d\t[%d]\t", count++, curr->blockData);
		hashPrinter(SHA256(toString(*prev),sizeof(*prev), NULL),SHA256_DIGEST_LENGTH);
		printf(" - ");
		hashPrinter(curr->prevHash,SHA256_DIGEST_LENGTH);
		if(hashCompare(SHA256(toString(*prev),sizeof(*prev),NULL),curr->prevHash))
			printf("Verified!\n");
		else
			printf("Verification Failed!\n");
		prev=curr;
		curr=curr->link;
	}
}

void alterNthBlock(int n, int newData)
{
	struct block *curr=head;
	int count=0;

	if (!curr)
	{
		printf("Nth block does not exist!\n");
		return;
	}
	while(count!=n)
	{
		if(!curr->link && count!=n)
		{
			printf("Nth block does not exist!\n");
			return;
		}
		else if (count==n)
			break;
		curr=curr->link;
		count++;
	}
	printf("Before: ");
	printBlock(curr);
	curr->blockData=newData;
	printf("\nAfter: ");
		printBlock(curr);
	printf("\n");
}

void hackChain()
{
	struct block *curr=head,*prev;
	if(!curr)
	{
		printf("BlockChain is empty!\n");
		return;
	}
	while (1)
	{
		prev=curr;
		curr=curr->link;
		if(!curr)
			return;
		if (!hashCompare(SHA256(toString(*prev),sizeof(*prev),NULL),curr->prevHash))
		{
			hashPrinter(
					SHA256(toString(*prev),sizeof(*prev),curr->prevHash),
					SHA256_DIGEST_LENGTH
			);
			printf("\n");
		}
	}
}

unsigned char* toString(struct block b)
{
	unsigned char *str=malloc(sizeof(unsigned char)*sizeof(b));
	memcpy(str,&b,sizeof(b));
	return(str);
}

void hashPrinter(unsigned char hash[], int length)
{
	for(int i=0;i<length;i++)
		printf("%02x",hash[i]);
}

int hashCompare(unsigned char *str1, unsigned char *str2)
{
	for (int i=0;i<SHA256_DIGEST_LENGTH;i++)
		if(str1[i]!=str2[i])
			return(0);
		return(1);
}

void printBlock(struct block *b)
{
	printf("%p]t",b);
	hashPrinter(b->prevHash,sizeof(b->prevHash));
	printf("\t[%d]\t",b->blockData);
	printf("%p\n",b->link);
}

void printAllBlocks()
{
	struct block *curr=head;
	int count=0;
	while(curr)
	{
		printBlock(curr);
		curr=curr->link;
	}
}

int main()
{
	int c,n,r;
	printf("1. addBlock\n2. add N random blocks\n3.alter Nth Block\n4. printAllBlocks\n5. verifyChain\n6. hackChain\n");
	while(1)
	{
		printf("Choice: ");
		scanf("%d",&c);
		switch(c)
		{
			case 1:
				printf("Enter data: ");
				scanf("%d",&n);
				addBlock(n);
				break;
			case 2:
				printf("How many blocks to enter?: ");
				scanf("%d", &n);
				for (int i=0;i<n;i++)
				{
					r=rand()%(n*10);
					printf("Entering: %d\n",r);
					addBlock(r);
				}
				break;
			case 3:
				printf("Which block to alter?: ");
				scanf("%d",&n);
				printf("Enter value: ");
				scanf("%d",&r);
				alterNthBlock(n,r);
				break;
			case 4:
				printAllBlocks();
				break;
			case 5:
				verifyChain();
				break;
			case 6:
				hackChain();
				break;
			default:
				printf("Wrong Choice!");
				break;
		}
	}
}

//gcc blockchain.c -o blockchain.o -lcrypto
//./blockchain.o
