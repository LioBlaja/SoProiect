#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snapshot.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#define shm_name "/my_shared_memory"
#define filesCount 100
#define delimitingLine "-----------------------------------------------------------------------------------------------------------------\n"

int initiateFilesMemory(MetaDataFile_T ** adress){
    int shm_fd;

    // Create or open the shared memory object
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory object
    ftruncate(shm_fd, sizeof(MetaDataFile_T) * filesCount);

    // Map the shared memory segment into process address space
    *adress = (MetaDataFile_T *)mmap(NULL, sizeof(MetaDataFile_T) * filesCount, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (*adress == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    close(shm_fd);
    return 0;
}

int main(int argc, char** argv){

    MetaDataFile_T *files = NULL;
    int counter = 0;
    initiateFilesMemory(&files);
    
    const char *shm_name1 = "/my_shared";
    int shm_fd1;
    int *shared_var1;

    // Create or open the shared memory object
    shm_fd1 = shm_open(shm_name1, O_CREAT | O_RDWR, 0666);
    if (shm_fd1 == -1) {
        perror("shm_open1");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory object
    ftruncate(shm_fd1, sizeof(int));

    // Map the shared memory segment into process address space
    shared_var1 = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
    if (shared_var1 == MAP_FAILED) {
        perror("mmap");
        close(shm_fd1);
        exit(EXIT_FAILURE);
    }


    // Initialize the shared variable to zero
    *(shared_var1) = 0;

    close(shm_fd1);

    // Unmap the shared memory segment
    // if (munmap(shared_var, sizeof(int)) == -1) {
    //     perror("munmap");
    //     exit(EXIT_FAILURE);
    // }

    if(strcmp(argv[1],"init") == 0){
        if(strlen(argv[2]) && init(argv[2],&counter,files) == 0){
            printf("The empty staging area was created!\n");
            printf("%s",delimitingLine);
            printSuccesBanner();
            // *shared_var1 = *shared_var1 + 3;
            // printf("Initial value in shared memory: %d\n", *shared_var1);
            printSnapshootIntoFile(files,argv[2]);
        }
    }else if(strcmp(argv[1],"status") == 0){
        status(counter,files);
        // printf("Initial value in shared memory: %d\n", *shared_var1);
    }else{
        printErrorBanner();
        printf("Invalid inputs");
        exit(EXIT_FAILURE);
    }

    return 0;
}