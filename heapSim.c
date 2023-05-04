#include <stdio.h>
#include <stdlib.h>
#define PAGESIZE 4096


typedef struct chunkhead{
    unsigned int size;
    unsigned int info;
    unsigned char *next, *prev;
}chunkhead;

void* startofheap = NULL;
unsigned char myheap[1048576];

unsigned char* mymalloc(unsigned int size){
    int extraSize;
    int fullSize;
    int heapLoc;
    chunkhead* current;
    /*extraSize is set to the size after first page*/
    extraSize = size - PAGESIZE + sizeof(chunkhead);
    /*if extraSize is negative or zero, it should be set to zero.*/
    /*otherwise, it should be large enough to fill an integer number of pages.*/
    if(extraSize <= 0){
        extraSize = 0;
    }else{
        extraSize += (PAGESIZE - extraSize%PAGESIZE);
    }
    fullSize = PAGESIZE-sizeof(chunkhead)+extraSize;
    heapLoc = 0;
    current = &myheap[0];
    while(heapLoc < sizeof(myheap)){
        if(current->info == 0 && current->size >= size){
            /*the current chunk is available
            check if the chunk should be split, if so, split*/
            current->info = 1;
            /*if current chunk should be split*/
            if(current->size > fullSize){
                chunkhead* newChunk;
                newChunk = malloc(sizeof(chunkhead));
                newChunk->info = 0;
                newChunk->prev = current;
                current->next = newChunk;
                newChunk->size = current->size - fullSize - sizeof(chunkhead);
                current->size = fullSize;
            }
            setChunkHead(current, &myheap[heapLoc]);
            return &myheap[heapLoc + sizeof(chunkhead)];
        }else{
            if(current->next == NULL){
                return NULL;
            }
            current = current->next;
            heapLoc += (current->size + sizeof(chunkhead));
        }
    }
    return NULL;
}

void myfree(unsigned char* address){
    chunkhead* addr = (chunkhead*)address;
    chunkhead* chunk;
    chunkhead* tofree;
    chunk = (chunkhead*)(addr-1);
    chunk->info = 0;
    if(chunk->next != NULL && ((chunkhead*)chunk->next)->info == 0){
        chunk->size += (((chunkhead*)chunk->next)->size + sizeof(chunkhead));
        tofree = chunk->next;
        chunk->next = ((chunkhead*)chunk->next)->next;
        if(tofree != &myheap[0])
            free(tofree);
    }
    else if(chunk->prev != NULL && ((chunkhead*)chunk->prev)->info == 0){
        chunk->size += (((chunkhead*)chunk->prev)->size + sizeof(chunkhead));
        tofree = chunk->prev;
        chunk->prev = ((chunkhead*)chunk->prev)->prev;
        if(tofree != &myheap[0])
            free(tofree);
    }
}

void analyse(){
    int chunkCount;
    chunkhead* current;
    current = &myheap[0];
    chunkCount = 0;
    while(current != NULL){
        printf("Chunk #%d:\n", chunkCount);
        printf("Size = %d bytes\n", current->size);
        if(current->info == 0){
            printf("Free\n");
        }else{
            printf("Occupied\n");
        }
        printf("Next = %p\n", current->next);
        printf("Prev = %p\n\n", current->prev);
        current = current->next;
        chunkCount++;
    }
}

void main(){

    /*void* address = mymalloc(1024);
    void* address2 = mymalloc(3);
    analyse();
    myfree(address);
    myfree(address2);
    analyse();*/
}