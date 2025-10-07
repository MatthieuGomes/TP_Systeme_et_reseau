#include "basic_include.h"

#define MAGIC_NUMBER 0x0123456789ABCDEFL

typedef struct HEADER_TAG
{
    struct HEADER_TAG *ptr_next; /* pointe sur le prochain bloc libre */
    size_t bloc_size;            /* taille du memory bloc en octets*/
    long magic_number;           /* 0x0123456789ABCDEFL */
} HEADER;

void header_to_string(HEADER *header_ptr){
    if(header_ptr == NULL){
        printf("header_ptr = NULL\n");
        return;
    }
    printf("header_ptr = %p\n", header_ptr);
    printf("header_ptr->ptr_next = %p\n", header_ptr->ptr_next);
    printf("header_ptr->bloc_size = %zu\n", header_ptr->bloc_size);
    printf("header_ptr->magic_number = %lx\n", header_ptr->magic_number);
    return;
}

HEADER *free_memory_struct_list = NULL; /* liste des blocs libres */

void print_free_memory_struct_list(void)
{
    HEADER *current = free_memory_struct_list;
    printf("#### free_memory_struct_list ####\n");
    if (current == NULL)
    {
        printf("free_memory_struct_list = NULL\n");
    }
    while (current != NULL)
    {
        printf("----\n");
        header_to_string(current);
        printf("----\n");
        current = current->ptr_next;
    }
    printf("###############################\n");
    return;
}

void *create_memory_struct(size_t size)
{
    HEADER *new_header;
    size_t memory_struct_size;
    void *memory_struct_start_addr;
    void *memory_block_start_addr;
    memory_struct_size = size + sizeof(HEADER);
    memory_struct_start_addr = sbrk(memory_struct_size);
    if (memory_struct_start_addr == (void *)-1)
    {
        return NULL;
    }
    new_header = (HEADER *)memory_struct_start_addr;
    new_header->ptr_next = NULL;
    new_header->bloc_size = size;
    new_header->magic_number = MAGIC_NUMBER;
    memory_block_start_addr = (void *)(new_header + 1);
    return memory_block_start_addr;
}

void *malloc_3is(size_t size)
{
    HEADER *free_memory_struct_ptr = free_memory_struct_list;
    if (free_memory_struct_ptr == NULL)
    {
        return create_memory_struct(size);
    }
    return NULL;
}

void free_3is(void *memory_block_ptr_to_free)
{
    HEADER *header_ptr_to_free;
    if (memory_block_ptr_to_free == NULL)
    {
        return;
    }
    header_ptr_to_free = (HEADER *)memory_block_ptr_to_free - 1;
    if (free_memory_struct_list == NULL)
    {
        free_memory_struct_list = header_ptr_to_free;
        header_ptr_to_free->ptr_next = NULL;
        return;
    }
    header_ptr_to_free->ptr_next = free_memory_struct_list;
    free_memory_struct_list = header_ptr_to_free;
    return;
}


void test_multi_alloc(size_t size1, size_t size2, size_t size3)
{
    printf("#### Test multi alloc ####\n");
    void *ptr1 = malloc_3is(size1);
    printf("ptr1 = %p\n", ptr1);
    void *ptr2 = malloc_3is(size2);
    printf("ptr2 = %p\n", ptr2);
    void *ptr3 = malloc_3is(size3);
    printf("ptr3 = %p\n", ptr3);
    printf("#### End of test ####\n");
    return;
}

void test_free(size_t size)
{
    printf("#### Test free ####\n");
    void *ptr_to_free = malloc_3is(size);
    printf("ptr_to_free = %p\n", ptr_to_free);
    print_free_memory_struct_list();
    free_3is(ptr_to_free);
    print_free_memory_struct_list();
    printf("#### End of test ####\n");
    return;
}

int main(void)
{
    test_multi_alloc(20, 30, 40);
    test_free(50);
    return 0;
}
