#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include "Ressoures_FS-20251021/tosfs.h"

#define path "Ressoures_FS-20251021/test_tosfs_files"
#define N (TOSFS_BLOCK_SIZE * 32)

typedef struct tosfs_superblock tosfs_superblock;
typedef struct tosfs_inode tosfs_inode;
typedef struct tosfs_dentry tosfs_dentry;

int checkMagicNumber(void * mMappedTestTosfs) {
    return ((tosfs_superblock *)mMappedTestTosfs)->magic == TOSFS_MAGIC;
}

void printSuperblockInfo(tosfs_superblock * superBlock) {
    printf("Magic number: 0x%x\n", superBlock->magic);
    printf("Block bitmap: 0x%x\n", superBlock->block_bitmap);
    printf("Inode bitmap: 0x%x\n", superBlock->inode_bitmap);
    printf("Block size: %u\n", superBlock->block_size);
    printf("Number of blocks: %u\n", superBlock->blocks);
    printf("Number of inodes: %u\n", superBlock->inodes);
    printf("Root inode: %u\n", superBlock->root_inode);
}

int getMaxNbInodesInInodeTable() {
    return TOSFS_BLOCK_SIZE / sizeof(tosfs_inode);
}

void printInodeTableInfo(tosfs_inode * inodeTable) {
    tosfs_inode * inode = inodeTable;
    for(int i = 0; i < getMaxNbInodesInInodeTable(); i++) {
        if (inode->inode != 0) {
            printf("Inode %u: Block No: %u, Uid: %u, Gid: %u, Mode: %u, Perm: %u, Size: %u bytes, NLink: %u\n",
                   inode->inode, inode->block_no, inode->uid, inode->gid, inode->mode, inode->perm, inode->size, inode->nlink);
        }
        inode = inode + 1;
    }
}

void printDentryInfo(tosfs_dentry * dentryTable) {
    tosfs_dentry * dentry = dentryTable;
    for(int i = 0; i < 32; i++) {
        if (dentry->inode != 0) {
            printf("Dentry %d: Inode: %u, Name: %s\n", i, dentry->inode, dentry->name);
        }
        dentry = dentry + 1;
    }
}

int main()
{
    int fd = open(path, O_RDONLY);

    printf("mapping file: %s\n", path);
    void * mMappedTestTosfs = mmap(NULL, N, PROT_READ, MAP_SHARED, fd, 0);
    if (mMappedTestTosfs == MAP_FAILED) {
        perror("Error mapping file");
        exit(EXIT_FAILURE);
    }
    printf("File mapped successfully.\n");
    if (!checkMagicNumber(mMappedTestTosfs)) {
        fprintf(stderr, "Invalid magic number. Not a TOSFS filesystem.\n");
        munmap(mMappedTestTosfs, N);
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("Valid TOSFS filesystem detected.\n");

    tosfs_superblock * superBlock = (tosfs_superblock *) mMappedTestTosfs;

    printSuperblockInfo(superBlock);

    tosfs_inode * inodeTable = (tosfs_inode *) ((void *)(mMappedTestTosfs + TOSFS_BLOCK_SIZE));
    printf("Inode table entries:\n");
    printInodeTableInfo(inodeTable);
    
    tosfs_dentry * rootDentry = (tosfs_dentry *) ((void *)(mMappedTestTosfs + (TOSFS_BLOCK_SIZE * TOSFS_ROOT_BLOCK)));

    printf("Root directory entries:\n");
    printDentryInfo(rootDentry);

    int returnCloseValue = close(fd);
    if (returnCloseValue == -1) {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }
    munmap(mMappedTestTosfs, N);
    return EXIT_SUCCESS;
}