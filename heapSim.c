#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PAGESIZE 4096


typedef struct chunkhead{
    unsigned int size;
    unsigned int info;
    void *next, *prev;
    void *largerEmpty;
}chunkhead;

void* startofheap = NULL;
chunkhead* lastchunk = NULL;

int intNumPageSize(int size){
    if(size%PAGESIZE == 0){
        return size;
    }
    return ((size/PAGESIZE)+1)*PAGESIZE;
}

void splitChunk(chunkhead* lower, int size){
    /*only run if you've already checked that lower->size is greater than size*/
    chunkhead* upper;
    upper = (chunkhead*)((char*)lower+size);
    upper->size = lower->size - size;
    lower->size = size;
    upper->next = lower->next;
    lower->next = (void*)upper;
    upper->prev = (void*)lower;
    upper->info = 0;
}

chunkhead* bestFit(int size){
    chunkhead* current;
    current = (chunkhead*)startofheap;
    while(current != NULL){
        if(current->info == 0 && current->size <= size){

        }
    }
}

void* mymalloc(unsigned int size){
    chunkhead* current;
    size = nearestChunkSize(size);
    if(size == 0){
        return NULL;
    }
    if(startofheap == NULL){
        /*base case, allocate space*/
        startofheap = sbrk(size);
        current = (chunkhead*)startofheap;
        lastchunk = current;
        current->size = size;
        current->info = 1;
        current->prev = NULL;
        current->next = NULL;
        current->largerEmpty = NULL;
        return (void*)((char*)(startofheap+sizeof(chunkhead)));
    }
    /*search for best empty fit for size. If not found, allocate more space at the end.*/
    current = bestFit(size);
    if(current == NULL){
        current = sbrk(size);
        current->size = size;
        current->info = 1;
        current->next = NULL;
        current->prev = lastchunk;
        lastchunk = current;
    }
    return (void*)((char*)current + sizeof(chunkhead));
}

/*void myfree(unsigned char* address){
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
}*/

void analyse(){
    int chunkCount;
    chunkhead* current;
    current = (chunkhead*)startofheap;
    chunkCount = 0;
    printf("\n--------------------------------------\n");
    printf("Program break location: %p\n", sbrk(0));
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

    void* address = mymalloc(1024);
    void* address2 = mymalloc(3);
    analyse();
    //myfree(address);
    //myfree(address2);
    analyse();
}