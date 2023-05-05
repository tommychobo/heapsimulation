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

    //UPDATE THIS to account for the size the user wants 
    //not including the chunkhead

    if(size%PAGESIZE == 0){
        return size;
    }
    return ((size/PAGESIZE)+1)*PAGESIZE;
}

void splitChunk(chunkhead* lower, int size){
    if((lower->size)+PAGESIZE < size){
        return;
    }
    chunkhead* upper;
    upper = (chunkhead*)((char*)lower+size);
    lower->size = intNumPageSize(size);
    if(lower == lastchunk){
        brk(upper);
        lower->next = NULL;
    }
    upper->size = lower->size - intNumPageSize(size);
    upper->next = lower->next;
    lower->next = (void*)upper;
    upper->prev = (void*)lower;
    upper->info = 0;
}

void mergeChunk(chunkhead* lower){
    chunkhead* upper;
    upper = (chunkhead*)(lower->next);
    if(upper == lastchunk){
        lastchunk = lower;
    }
    lower->size += upper->size;
    lower->next = upper->next;
}

chunkhead* bestFit(int size){
    chunkhead* current;
    current = (chunkhead*)startofheap;
    while(current != NULL){
        if(current->info == 0 && current->size <= size){
            splitChunk(current, size);
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void* mymalloc(unsigned int size){
    chunkhead* current;
    size = intNumPageSize(size);
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
        lastchunk->next = (void*)current;
        current->prev = (void*)lastchunk;
        lastchunk = current;
    }
    return (void*)((char*)current + sizeof(chunkhead));
}

void myfree(void* address){
    /*address is the part of the chunk right after the chunkhead.
    set info to 0. If lastchunk, move prog break back.
    If next info is 0, merge. If prev not null & info is 0, merge.*/
    chunkhead* addr;
    addr = (chunkhead*)((char*)address-sizeof(chunkhead));
    addr->info = 0;
    if(addr = lastchunk){
        if(addr->prev == NULL){
            startofheap = NULL;
            lastchunk = NULL;
            brk(addr);
            return;
        }
        //move it back
        //set prev->next to null
        ((chunkhead*)addr->prev)->next = NULL;
        lastchunk = (chunkhead*)addr->prev;
        brk(addr);
        return;
    }
    if(addr->next != NULL && ((chunkhead*)addr->next)->info == 0){
        mergeChunk(addr);
    }
    if(addr->prev != NULL && ((chunkhead*)addr->prev)->info == 0){
        mergeChunk((chunkhead*)(addr->prev));
    }
}

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

    void* address = mymalloc(4000);
    analyse();
    void* address2 = mymalloc(8000);
    analyse();
    void* address3 = mymalloc(20202);
    analyse();
}