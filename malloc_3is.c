#include "basic_include.h"

#define MAGIC_NUMBER 0x0123456789ABCDEFL

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next; /* pointe sur le prochain bloc libre */
    size_t bloc_size; /* taille du memory bloc en octets*/
    long magic_number; /* 0x0123456789ABCDEFL */
} HEADER;

HEADER * free_memory_struct_list = NULL; /* liste des blocs libres */

void * create_memory_struct(size_t size){
    HEADER * new_header;
    size_t memory_struct_size;
    void * memory_struct_start_addr;
    void * memory_block_start_addr;
    memory_struct_size = size + sizeof(HEADER);
    memory_struct_start_addr = sbrk(memory_struct_size);
    if (memory_struct_start_addr == (void *) -1) {
        return NULL;
    }
    new_header = (HEADER *) memory_struct_start_addr;
    new_header->ptr_next = NULL;
    new_header->bloc_size = size;
    new_header->magic_number = MAGIC_NUMBER;
    memory_block_start_addr = (void *) (new_header + 1);
    return memory_block_start_addr;
}

void * malloc_3is(size_t size){
    HEADER * free_memory_struct_ptr = free_memory_struct_list;
    if(free_memory_struct_ptr == NULL){
        return create_memory_struct(size);
    }
    return NULL;
}

void * test_multi_alloc(size_t size1, size_t size2, size_t size3){
    void * ptr1 = malloc_3is(size1);
    printf("ptr1 = %p\n", ptr1);
    void * ptr2 = malloc_3is(size2);
    printf("ptr2 = %p\n", ptr2);
    void * ptr3 = malloc_3is(size3);
    printf("ptr3 = %p\n", ptr3);
    return NULL;
}

int main(void){
    test_multi_alloc(20, 30, 40);
    return 0;
}

