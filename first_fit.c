#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

typedef struct block {
    size_t          size;
    int             free;
    struct block    *prev;
    struct block    *next;
} Block;

Block*  BlockList =     NULL;
char*   MapAddr =       NULL;

size_t getMaxFreeChunkSize() {
    Block *tmp_block = NULL;
    size_t max_free_chunk_size = 0;
    char buf[100] = {'\0'};

    for (tmp_block = BlockList; tmp_block != NULL; tmp_block = tmp_block->next) {
        if (tmp_block->free == 1 && max_free_chunk_size < tmp_block->size) {
            max_free_chunk_size = tmp_block->size;
        }
    }

    sprintf(buf, "Max Free Chunk Size = %ld\n", max_free_chunk_size);
    write(0, buf, strlen(buf));

    return max_free_chunk_size;
}

char* malloc(size_t ask_size)
{
    char* TempMapAddr = NULL;

    if (MapAddr == NULL) {

        MapAddr = (char*) mmap(NULL, 20000, PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
        BlockList = (Block*) MapAddr;
        BlockList->size = 20000 - 32;
        BlockList->free = 1;
        BlockList->prev = NULL;
        BlockList->next = NULL;
    }

    if (ask_size > 0) {
        ask_size = ((ask_size + 32 - 1) / 32) * 32;
        Block* tmp_block_t = BlockList;

        for (; tmp_block_t != NULL; tmp_block_t = tmp_block_t->next) {
            if (tmp_block_t->free == 1 && tmp_block_t->size >= ask_size) {
                break;
            }
        }

        if (tmp_block_t->size > (ask_size + 32 + 32)) {
            Block* tmp_block_2 = (Block*) ((char*)tmp_block_t + 32 + ask_size);

            tmp_block_2->size = tmp_block_t->size - 32 - ask_size;
            tmp_block_2->free = 1;
            tmp_block_2->next = tmp_block_t->next;
            tmp_block_2->prev = tmp_block_t;

            tmp_block_t->next = tmp_block_2;
        }
        tmp_block_t->free = 0;
        tmp_block_t->size = ask_size;

        TempMapAddr = (char*) ((char*)tmp_block_t + 32);
    } else if (ask_size == 0) {
        getMaxFreeChunkSize();

        munmap(MapAddr, 20000);
        return NULL;
    }

    return TempMapAddr;
}

void free(char* ptr)
{
    Block* tmp_block_t = (Block*) (ptr - 32);
    Block* tmp_block_1 = tmp_block_t->prev;
    Block* tmp_block_3 = tmp_block_t->next;

    tmp_block_t->free = 1;

    if (tmp_block_1 != NULL && tmp_block_1->free == 1) {
        tmp_block_1->size += (tmp_block_t->size + 32);
        tmp_block_1->next = tmp_block_t->next;
        if (tmp_block_t->next != NULL) {
            tmp_block_t->next->prev = tmp_block_1;
        }
        tmp_block_t = tmp_block_1;
    }

    if (tmp_block_3 != NULL && tmp_block_3->free == 1) {
        tmp_block_t->size += (tmp_block_3->size + 32);
        tmp_block_t->next = tmp_block_3->next;
        if (tmp_block_3->next != NULL) {
            tmp_block_3->next->prev = tmp_block_t;
        }
    }

    return;
}
