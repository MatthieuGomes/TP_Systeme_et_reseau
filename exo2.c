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
    else printf("File size: %d bytes\n", file_size);

    return EXIT_SUCCESS;
}


