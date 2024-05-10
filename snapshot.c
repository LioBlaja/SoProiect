#include "snapshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#define LINE_SIZE 256

char* errorLine1 = "  ___ ___ ___  ___  ___ ";
char* errorLine2 = " | __| _ \\ _ \\/ _ \\| _ \\";
char* errorLine3 = " | _||   /   / (_) |   /";
char* errorLine4 = " |___|_|_\\_|_\\\\___/|_|_\\";

char* succesLine1 = "  ___ _   _  ___ ___ ___ ___ ";
char* succesLine2 = " / __| | | |/ __/ __| __/ __|";
char* succesLine3 = " \\__ \\ |_| | (_| (__| _|\\__ \\";
char* succesLine4 = " |___/\\___/ \\___\\___|___|___/";
                        
int checkDirectory(const char* dirPath) {
    DIR *directory = opendir(dirPath);
    if (directory == NULL) {
        if (errno == ENOENT) {
            return -1;
        } else if (errno == ENOTDIR) {
            return -1;
        } else {
            return -1;
        }
        return -1;
    }
    closedir(directory);
    return 1;
}

void printStringWithHiddenChars(const char *str) { // used for some debugg
    printf("String with hidden characters:\n");
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < ' ' || str[i] > '~') {
            // Print non-printable characters as hexadecimal
            printf("\\x%02X", (unsigned char)str[i]);
        } else {
            // Print printable characters as is
            printf("%c", str[i]);
        }
    }
    printf("\n");
}
MetaDataFile_T addData(char *name){
    MetaDataFile_T auxStruct;
    struct stat file_stat;
    if (stat(name, &file_stat) == -1) {
        if (errno == ENOENT || errno == EFAULT){
            perror("Non existent directory or file");
            exit(-1);
        }
        if (errno == EACCES){
            perror("Can't access the directory/file, permission denied");
            exit(-1);
        }
        perror(strerror(errno));
        exit(-1);
    }

    auxStruct.file_id = file_stat.st_ino;
    auxStruct.file_size = file_stat.st_size;
    auxStruct.file_type = (S_ISDIR(file_stat.st_mode) ? 0 : 1);
    auxStruct.file_last_modified = file_stat.st_mtime;

    return auxStruct;
}

int init(const char *dirPath,int* filesCounter,MetaDataFile_T *files) {
    int count = *filesCounter;

    DIR *dir = opendir(dirPath);

    if(dir == NULL){
        perror("directory open error");
        exit(EXIT_FAILURE);
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
        
        (files)[count] = addData(tempPath);
        strncpy((files)[count].file_name, aux->d_name, FILE_NAME_SIZE);
        (files)[count].file_name[FILE_NAME_SIZE - 1] = '\0';  // Ensure null-terminated string
        count++;
        

        if ((files)[count - 1].file_type == 0) {
            if (init(tempPath,&count,files) == -1) {
                closedir(dir);
                return -1;  // Propagate error from recursive call
            }
        }
    }
    closedir(dir);
    *filesCounter = count;
    return 0;  // Success
}

void readDirInfoFromBin(const char* dirPath, MetaDataFile_T* files){
    FILE *fp;
    char line[LINE_SIZE];
    int arrayIndex = 0;

    fp = fopen(BIN_FILE_NAME, "rb");
    if (fp == NULL) {
        printf("Error opening file %s.\n", dirPath);
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        char *token;
        
        token = strtok(line, "|");
        if (strcmp(token, dirPath) == 0) {
            while (fgets(line,sizeof(line),fp)){
                token = strtok(line, "|"); //file type
                if (strcmp(token,"0") || strcmp(token,"1") )
                {
                    if(atoi(token) == 1){
                        files[arrayIndex].file_type = 1;
                    }else{
                        files[arrayIndex].file_type = 0;
                    }

                    if((token = strtok(NULL, "|")) != NULL && (strcmp(token, "\x0A") != 0))
                    {
                        strcpy(files[arrayIndex].file_name,token);
                    }
                    if((token = strtok(NULL, "|")) != NULL && (strcmp(token, "\x0A") != 0))
                    {
                        files[arrayIndex].file_id = atoi(token);
                    }
                    if((token = strtok(NULL, "|")) != NULL && (strcmp(token, "\x0A") != 0))
                    {
                        files[arrayIndex].file_size = atoi(token);
                    }
                    if((token = strtok(NULL, "|")) != NULL && (strcmp(token, "\x0A") != 0))
                    {
                        files[arrayIndex].file_last_modified = atoi(token);
                    }
                    
                    arrayIndex++;
                }
            }
            
        }
    }
    // Close the file
    fclose(fp);
}

void status(MetaDataFile_T *files, int counter) {
    printf("\n%-30s | %-10s | %-12s | %-12s | %-25s\n", "File Name", "Type", "File ID", "File Size", "Last Modified");
    printf("-----------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < counter; i++) {
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

void printSnapshootIntoFile(MetaDataFile_T *filesOld,
                            MetaDataFile_T* filesNew, 
                            char* dir,int filesCounter,char* outputName){
    char* dirToAdd = dir;
    if (outputName != NULL)
    {
        dirToAdd = outputName;
    }
    
    char path[LINE_SIZE]; 

    strcpy(path, "./"); 
    strcat(path, dirToAdd); 
    strcat(path, "/snapshot_"); // Add directory name to filename
    char *duplicate = strdup(dir);
    // Iterate over the duplicate string and replace '/' with '_'
    for (int i = 0; duplicate[i] != '\0'; i++) {
        if (duplicate[i] == '/') {
            duplicate[i] = '_';
        }
    }
    strcat(path, duplicate); // Add directory name to filename
    strcat(path, ".txt"); // Add directory name to filename

    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }
    fprintf(fp,"\nNew Snapshot of %s\n\n",dir);
    fprintf(fp, "%-25s | %-10s | %-8s | %-10s | %-18s\n", "File Name", "Type", "File ID", "File Size", "Last Modified");
    fprintf(fp, "---------------------------------------------------------------------------------------\n");

    for (int i = 0; i < filesCounter; i++) {
        if(filesNew[i].file_id != 0){
            fprintf(fp, "%-25s | %-10s | %-8ld | %-10ld | %-18s", 
                filesNew[i].file_name, 
                filesNew[i].file_type ? "File" : "Directory", 
                filesNew[i].file_id, 
                filesNew[i].file_size, 
                ctime(&(filesNew[i].file_last_modified)));
        }
    }
    fprintf(fp, "---------------------------------------------------------------------------------------\n");
    fclose(fp);
}

void addDirectoryStructToBin(int filesCount, MetaDataFile_T* files, char* dir){
    FILE *file = fopen(BIN_FILE_NAME, "ab");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Write additional string followed by '|'
    fprintf(file, "%s|", dir);
    fprintf(file,"%d",filesCount);

    fprintf(file, "\n");

    // Write struct data, separating fields by '|'
    for (size_t i = 0; i < filesCount; i++) {
        fprintf(file, "%d|%s|%ld|%ld|%ld|", files[i].file_type, files[i].file_name, files[i].file_id, files[i].file_size, files[i].file_last_modified);
        fprintf(file, "\n");
    }

    fclose(file);
}


void writeMetaDataIntoBin(char** argv,int argc){
    if(fopen(BIN_FILE_NAME, "wb") == NULL)
        printf("error when clear the bin file\n");
    int filesCounters[argc - 2];
    printSuccesBanner();
    for (int i = 1; i < argc; i++)
        {
            int temp = 0;
            MetaDataFile_T tempDirNewData[ARR_SIZE];
            MetaDataFile_T tempDirDataForReading[ARR_SIZE];
            filesCounters[i] = 0;
            if(init(argv[i],&temp,tempDirNewData) != -1){
                
                filesCounters[i] = temp;
                
                addDirectoryStructToBin(filesCounters[i],tempDirNewData,argv[i]);
                readDirInfoFromBin(argv[i],tempDirDataForReading);
                if(strcmp(argv[1],"-o") == 0){
                    printSnapshootIntoFile(tempDirDataForReading,tempDirNewData,argv[i],filesCounters[i],argv[2]);
                }else{
                    printSnapshootIntoFile(tempDirDataForReading,tempDirNewData,argv[i],filesCounters[i],NULL);
                }
            }
        }
}

void startChildProcess(char *dir, char *outputDir) {
    pid_t pid = fork();

    if(pid < 0)
    {
        perror(strerror(errno));
        exit(-1);
    }else if(pid == 0)
    {
        int temp = 0;
        MetaDataFile_T tempDirNewData[ARR_SIZE];
        MetaDataFile_T tempDirDataForReading[ARR_SIZE];
        int fileCounters = 0;
        if(init(dir,&temp,tempDirNewData) != -1){
            fileCounters = temp;

            addDirectoryStructToBin(fileCounters,tempDirNewData,dir);
            readDirInfoFromBin(dir,tempDirDataForReading);

            printSnapshootIntoFile(tempDirDataForReading,tempDirNewData,dir,fileCounters,outputDir);
        }
            
        exit(0);
    }
}