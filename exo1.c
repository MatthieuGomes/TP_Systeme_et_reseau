#include "basic_include.h"  
#include <sys/mman.h>

int var_in_data = 42; 
int var_in_bss; 

void get_var_in_stack(int arg_in_stack) {
    int var_local_in_stack = 100; 
    printf("Address of argument (stack): %p\n", &arg_in_stack);
    printf("Address of local variable (stack): %p\n", &var_local_in_stack);
}
void get_var_in_heap() {
    int *var_in_heap = malloc(sizeof(int)); 
    *var_in_heap = 100;
    printf("Address of variable in heap: %p\n", &var_in_heap);
    free(var_in_heap);
}

void function_in_text() {
    printf("This function is in the text segment.\n");
}

void pmap_check(int pid) {
    char pid_str[10];
    printf("Current process ID: %d\n", pid);
    snprintf(pid_str, sizeof(pid_str), "%d", pid);
    char *pmap_args[] = {"pmap", "-X",pid_str, NULL};
    execvp("pmap", pmap_args);
}
int main() {
    printf("Address of a variable in data segment: %p\n", &var_in_data);
    printf("Address of a variable in BSS segment: %p\n", &var_in_bss);
    printf("Address of main function (text segment): %p\n", main);
    printf("Address of a function of libC (libC): %p\n", printf);
    get_var_in_stack(10);
    get_var_in_heap();
    void *mmap_addr = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    printf("Address of mmaped memory: %p\n", mmap_addr);
    pmap_check(getpid());

    return 0;
}