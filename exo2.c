#include "basic_include.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>


int get_file_size(int file_descriptor){
    struct stat  stat_struct;
    if (fstat(file_descriptor, &stat_struct) == STD_FUNC_ERROR){
        perror("fstat");
        return STD_FUNC_ERROR;
    }
    return stat_struct.st_size;
}
int map_file_to_memory(char** mapped_file, int file_descriptor, int file_size){
    *mapped_file = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, file_descriptor, 0);
    if (mapped_file == MAP_FAILED){
        perror("mmap");
        return STD_FUNC_ERROR;
    }
    return 0;
}


int main(){
    char* file_path = "test.txt";
    int file_descriptor = open(file_path, O_RDWR);
    if (file_descriptor== STD_FUNC_ERROR){
        perror("open");
        close(file_descriptor);
        return EXIT_FAILURE;
    }
    int file_size = get_file_size(file_descriptor);
    if (file_size == STD_FUNC_ERROR){
        close(file_descriptor);
        return EXIT_FAILURE;
    }
    printf("File size: %d bytes\n", file_size);
    char *mapped_file;
    if (map_file_to_memory(&mapped_file, file_descriptor, file_size) == STD_FUNC_ERROR){
        close(file_descriptor);
        return EXIT_FAILURE;
    }
    printf("Mapped file content:\n%s\n", mapped_file);

    return EXIT_SUCCESS;
}


