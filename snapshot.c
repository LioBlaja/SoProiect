#include "snapshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

char* errorLine1 = "  ___ ___ ___  ___  ___ ";
char* errorLine2 = " | __| _ \\ _ \\/ _ \\| _ \\";
char* errorLine3 = " | _||   /   / (_) |   /";
char* errorLine4 = " |___|_|_\\_|_\\\\___/|_|_\\";

char* succesLine1 = "  ___ _   _  ___ ___ ___ ___ ";
char* succesLine2 = " / __| | | |/ __/ __| __/ __|";
char* succesLine3 = " \\__ \\ |_| | (_| (__| _|\\__ \\";
char* succesLine4 = " |___/\\___/ \\___\\___|___|___/";
                        

// void checkDirectory(DIR* dirPath) {
    
//     if (dirPath == NULL) {
//         if (errno == ENOENT) { // errno was set as ENOENT then the pointed dir doesn't exist
//             printf("sydgsuds");
//             printErrorBanner();
//         } else if (errno == ENOTDIR) { // errno was set as ENOTDIR then the path points to a file
//             perror("The name given as argument is a file!");
//         } else {
//             perror("Failed to open directory");
//         }
//         exit(EXIT_FAILURE); 
//     }
// }

void checkDirectory(const char* dirPath) {
    DIR *directory = opendir(dirPath);
    if (directory == NULL) {
        if (errno == ENOENT) {
            perror("Directory doesn't exist!");
        } else if (errno == ENOTDIR) {
            perror("The name given as argument is a file!");
        } else {
            perror("Failed to open directory"); // Fallback error message
        }
        exit(EXIT_FAILURE);
    }
    closedir(directory);
}

MetaDataFile_T addData(char *name) // uses file stat to set all the data that we need to know about a file or directory and returns the obtained data
{
    MetaDataFile_T retFile;
    struct stat file_stat;
    if (stat(name, &file_stat) == -1) // if the file status returns an error code => stop the program, print a proper message
    {
        if (errno == ENOENT || errno == EFAULT)
        {
            // printErrorBanner();
            perror("Nonexistent directory or file");
            exit(-1);
        }
        if (errno == EACCES)
        {
            perror("Can't access the directory/file, permission denied");
            // printErrorBanner();
            exit(-1);
        }
        perror(strerror(errno));
        exit(-1);
    }

    retFile.file_id = file_stat.st_ino;
    retFile.file_size = file_stat.st_size;                    // -> add data to the array
    retFile.file_type = (S_ISDIR(file_stat.st_mode) ? 0 : 1);
    retFile.file_last_modified = file_stat.st_mtime;

    return retFile;
}

int init(const char *dirPath, int *counter, MetaDataFile_T *files) {
    DIR *dir = opendir(dirPath);
    if (!dir) {
        perror("opendir");
        return -1;
    }

    struct dirent *aux;
    while ((aux = readdir(dir)) != NULL) {
        if (aux->d_name[0] == '.' || strcmp(aux->d_name, "..") == 0) {
            continue;  // Skip current directory and parent directory entries
        }

        char tempPath[PATH_SIZE];
        snprintf(tempPath, PATH_SIZE, "%s/%s", dirPath, aux->d_name);

        struct stat file_stat;
        if (stat(tempPath, &file_stat) == -1) {
            perror("stat");
            closedir(dir);
            return -1;
        }

        // if (files == NULL) {
        //     files = malloc(sizeof(MetaDataFile_T));
        //     if (!files) {
        //         perror("malloc");
        //         closedir(dir);
        //         return -1;
        //     }
        // } else {
        //     MetaDataFile_T *temp = realloc(files, (*counter + 1) * sizeof(MetaDataFile_T));
        //     if (!temp) {
        //         perror("realloc");
        //         closedir(dir);
        //         return -1;
        //     }
        //     files = temp;
        // }

        (files)[*counter] = addData(tempPath);
        strncpy((files)[*counter].file_name, aux->d_name, FILE_NAME_SIZE);
        (files)[*counter].file_name[FILE_NAME_SIZE - 1] = '\0';  // Ensure null-terminated string
        (*counter)++;

        if ((files)[(*counter) - 1].file_type == 0) {
            if (init(tempPath, counter, files) == -1) {
                closedir(dir);
                return -1;  // Propagate error from recursive call
            }
        }
    }

    closedir(dir);
    return 0;  // Success
}

// int init(char const *dirPath, int *counter, MetaDataFile_T** files) {
//     DIR *dir = opendir(dirPath);
    
//     checkDirectory(dirPath);
    
//     struct dirent *aux;
//     while ((aux = readdir(dir)) != NULL) {
//         if (aux->d_name[0] == '.') // Skip current directory and parent directory entries
//             continue;

//         char tempPath[PATH_SIZE];
//         snprintf(tempPath, PATH_SIZE, "%s/%s", dirPath, aux->d_name);

//         *files = realloc(*files, sizeof(MetaDataFile_T) * ((*counter) + 1));

//         if (*files == NULL) {
//             perror("realloc");
//             exit(EXIT_FAILURE);
//         }

//         (*files)[*counter] = addData(tempPath);
//         strcpy((*files)[*counter].file_name, aux->d_name);
//         (*counter)++; 

//         if ((*files)[(*counter) - 1].file_type == 0) {
//             init(tempPath, counter, files);
//         }
//     }
//     closedir(dir);
//     return 0;
// }

int status(int counter, MetaDataFile_T *files) {
    printf("\n%-30s | %-10s | %-12s | %-12s | %-25s\n", "File Name", "Type", "File ID", "File Size", "Last Modified");
    printf("-----------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < 100; i++) {
        if(files[i].file_id != 0){
            printf("%-30s | %-10s | %-12ld | %-12ld | %s", 
                files[i].file_name, 
                files[i].file_type ? "File" : "Directory", 
                files[i].file_id, 
                files[i].file_size, 
                ctime(&(files[i].file_last_modified)));
        }
    }
    printf("\n");
}

void printErrorBanner(){
    printf("\n%s\n%s\n%s\n%s\n",errorLine1,errorLine2,errorLine3,errorLine4);
}

void printSuccesBanner(){
    printf("\n%s\n%s\n%s\n%s\n",succesLine1,succesLine2,succesLine3,succesLine4);
}

void printSnapshootIntoFile(MetaDataFile_T *files, char* dir){
    char path[256]; 
    
    strcpy(path, "./"); 
    strcat(path, dir); 
    strcat(path, "/snapshot.txt"); 

    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(fp, "%-30s | %-10s | %-12s | %-12s | %-25s\n", "File Name", "Type", "File ID", "File Size", "Last Modified");
    fprintf(fp, "-----------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < 100; i++) {
        if(files[i].file_id != 0){
            fprintf(fp, "%-30s | %-10s | %-12ld | %-12ld | %s", 
                files[i].file_name, 
                files[i].file_type ? "File" : "Directory", 
                files[i].file_id, 
                files[i].file_size, 
                ctime(&(files[i].file_last_modified)));
        }
    }

    fclose(fp);
}