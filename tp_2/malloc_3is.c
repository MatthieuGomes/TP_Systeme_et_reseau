#include "basic_include.h"

#define MAGIC_NUMBER 0x0123456789ABCDEF

typedef struct HEADER_TAG
{
    struct HEADER_TAG *ptr_next; /* pointe sur le prochain bloc libre */
    size_t bloc_size;            /* taille du memory bloc en octets*/
    long magic_number;           /* 0x0123456789ABCDEFL */
} HEADER;

void print_header(HEADER *header_ptr)
{
    if (header_ptr == NULL)
    {
        printf("    header_ptr = NULL\n");
        return;
    }
    printf("    {\n");
    printf("        header_ptr = %p\n", header_ptr);
    printf("        header_ptr->ptr_next = %p\n", header_ptr->ptr_next);
    printf("        header_ptr->bloc_size = %zu\n", header_ptr->bloc_size);
    printf("        header_ptr->magic_number = %lx\n", header_ptr->magic_number);
    printf("    }\n");
    return;
}

HEADER *free_memory_struct_list = NULL; /* liste des blocs libres */
int overflow_count = 0;

void print_free_memory_struct_list(void)
{
    HEADER *current = free_memory_struct_list;
    printf("    #### start free_memory_struct_list ####\n");
    if (current == NULL)
    {
        printf("    free_memory_struct_list = NULL\n");
    }
    while (current != NULL)
    {
        printf("    ----\n");
        print_header(current);
        printf("    ----\n");
        current = current->ptr_next;
    }
    printf("    #### end free_memory_struct_list ####\n");
    return;
}

void *create_memory_struct(size_t size)
{
    HEADER *new_header = NULL;
    size_t memory_struct_size = 0;
    void *memory_struct_start_addr = NULL;
    void *memory_block_start_addr = NULL;
    long *magic_ptr = NULL;
    memory_struct_size = size + sizeof(HEADER) + sizeof(MAGIC_NUMBER);
    memory_struct_start_addr = sbrk(memory_struct_size);
    if (memory_struct_start_addr == (void *)-1)
    {
        return NULL;
    }
    new_header = (HEADER *)memory_struct_start_addr;
    new_header->ptr_next = NULL;
    new_header->bloc_size = size;
    new_header->magic_number = (long)MAGIC_NUMBER;
    memory_block_start_addr = (void *)(memory_struct_start_addr + sizeof(HEADER));

    magic_ptr = (long *)(memory_block_start_addr + size);
    *magic_ptr = (long)MAGIC_NUMBER;
    return memory_block_start_addr;
}

void check_overflow(void *memory_block_ptr)
{
    HEADER *header_ptr;
    long *magic_ptr;
    long saved_magic_number;
    long current_magic_number;
    long memory_block_size;

    if (memory_block_ptr == NULL)
    {
        return;
    }
    header_ptr = (HEADER *)(memory_block_ptr - sizeof(HEADER));
    saved_magic_number = header_ptr->magic_number;
    memory_block_size = header_ptr->bloc_size;
    magic_ptr = (long *)((void *)memory_block_ptr + memory_block_size);
    current_magic_number = *magic_ptr;
    if (current_magic_number != saved_magic_number)
    {
        overflow_count = overflow_count + 1;
    }
    return;
}

void *malloc_3is(size_t size)
{
    HEADER *current_free_memory_struct_ptr;
    HEADER *next_free_memory_struct_ptr;
    current_free_memory_struct_ptr = free_memory_struct_list;

    if (current_free_memory_struct_ptr == NULL)
    {
        return create_memory_struct(size);
    }
    next_free_memory_struct_ptr = current_free_memory_struct_ptr->ptr_next;
    while (next_free_memory_struct_ptr != NULL)
    {
        if (next_free_memory_struct_ptr->bloc_size >= size)
        {
            current_free_memory_struct_ptr->ptr_next = next_free_memory_struct_ptr->ptr_next;
            next_free_memory_struct_ptr->ptr_next = NULL;
            return next_free_memory_struct_ptr;
        }
        current_free_memory_struct_ptr = current_free_memory_struct_ptr->ptr_next;
        next_free_memory_struct_ptr = current_free_memory_struct_ptr->ptr_next;
    }
    return create_memory_struct(size);
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
    printf("    ptr1 = %p\n", ptr1);
    print_header((HEADER *)(ptr1 - sizeof(HEADER)));
    void *ptr2 = malloc_3is(size2);
    printf("    ptr2 = %p\n", ptr2);
    print_header((HEADER *)(ptr2 - sizeof(HEADER)));
    void *ptr3 = malloc_3is(size3);
    printf("    ptr3 = %p\n", ptr3);
    print_header((HEADER *)(ptr3 - sizeof(HEADER))); 
    printf("#### End of multi alloc test ####\n");
    return;
}

void test_free(size_t size)
{
    printf("#### Test free ####\n");
    void *ptr_to_free = malloc_3is(size);
    printf("    ptr_to_free = %p\n", ptr_to_free);
    print_header((HEADER *)(ptr_to_free - sizeof(HEADER)));
    printf("    free_memory_struct_list before free:\n");
    print_free_memory_struct_list();
    free_3is(ptr_to_free);
    printf("    free_memory_struct_list after free:\n");
    print_free_memory_struct_list();
    free_memory_struct_list = NULL; // reset for other tests
    printf("#### End of free test ####\n");
    return;
}

void test_overflow(size_t size)
{
    printf("#### Test overflow ####\n");
    char *ptr = (char *)malloc_3is(size);
    printf("    ptr = %p\n", ptr);
    print_header((HEADER *)(ptr - sizeof(HEADER)));
    printf("    filling memory block with %zu bytes\n", size);
    for (int c = 0; c < (int)size; c++)
    {
        ptr[c] = 'A';
    }
    check_overflow(ptr);
    printf("    overflow_count = %d\n", overflow_count);
    printf("    overflowing memory block with +1 byte\n");
    ptr[size] = 'A';
    check_overflow(ptr);
    printf("    overflow_count = %d\n", overflow_count);
    printf("#### End of overflow test ####\n");
}

void test_reuse_allocated_memory(size_t too_big, size_t small_enough)
{
    printf("#### Test reuse allocated memory ####\n");
    void *ptr1 = malloc_3is(too_big);
    printf("    ptr1 = %p\n", ptr1);
    print_header((HEADER *)(ptr1 - sizeof(HEADER)));
    free_3is(ptr1);
    printf("    free_memory_struct_list after free:\n");
    print_free_memory_struct_list();
    void *ptr2 = malloc_3is(too_big);
    printf("    ptr2 = %p\n", ptr2);
    print_header((HEADER *)(ptr2 - sizeof(HEADER)));
    if (ptr1 == ptr2)
    {
        printf("    ptr2 is equal to ptr1, memory reused\n");
    }
    else
    {
        printf("    ptr2 is different from ptr1, memory not reused\n");
    }
    printf("#### End of reuse allocated memory test ####\n");
    return;
}

int main(void)
{
    test_multi_alloc(20, 30, 40);
    printf("\n");
    test_free(50);
    printf("\n");
    test_overflow(2);

    return 0;
}
