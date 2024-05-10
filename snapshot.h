#include <time.h>    // For time_t
#include <dirent.h>

#define FILE_NAME_SIZE 100
#define PATH_SIZE 1000
#define ARR_SIZE 255
#define BIN_FILE_NAME "data.bin"


typedef struct File
{
    int file_type;
    char file_name[FILE_NAME_SIZE];
    long file_id;
    long file_size;
    time_t file_last_modified;

} MetaDataFile_T; 

void printErrorBanner(void);
void printSuccesBanner(void);                            
int init(char const *,int *,MetaDataFile_T*);
void status(MetaDataFile_T *, int);
void printSnapshootIntoFile(MetaDataFile_T *,MetaDataFile_T *, char*,int,char*);
void addDirectoriesToBin(int,char**);
void writeMetaDataIntoBin(char**,int);
void startChildProcess(char *, char *);
int checkDirectory(const char*);



// void addDirectoriesToBin(int argc,char **argv){
//     FILE *file;
//     char *data;
//     int length = 0;

//     if(strcmp(argv[1],"-o") != 0){
//         // Calculate total length of all arguments + number of '|' separators
//         for (int i = 1; i < argc; i++) {
//             length += strlen(argv[i]);
//         }

//         // Add space for '|' separators between arguments
//         length += argc - 2;

//         // Allocate memory to store the data
//         data = (char *)malloc(length + 1); // +1 for null terminator
//         if (data == NULL) {
//             fprintf(stderr, "Memory allocation failed.\n");
//             return;
//         }

//         // Construct the data string with '|' separators
//         strcpy(data, argv[1]);
//         for (int i = 2; i < argc; i++) {
//             strcat(data, "|");
//             strcat(data, argv[i]);
//         }

//         // Open file in binary write mode (truncates existing content)
//         // file = fopen(BIN_FILE_NAME, "wb");
//         file = fopen(BIN_FILE_NAME, "rb+");
//         char line[256];
//         if (file == NULL) {
//             fprintf(stderr, "Unable to open file.\n");
//             free(data);
//             return;
//         }
//         if (strcmp(line, data) != 0) {
//         // If not, move to the beginning of the file and write the new line
//             fseek(file, 0, SEEK_SET);
//             fputs(data, file);
//         }

//         // Write the data string to the file
//         // fwrite(data, sizeof(char), length, file);
//         // fprintf(file, "\n");

//         // Close the file
//         fclose(file);

//         // Free allocated memory
//         free(data);

//         printSuccesBanner();
//     }else{
//         // Calculate total length of all arguments + number of '|' separators
//         for (int i = 3; i < argc; i++) {
//             length += strlen(argv[i]);
//         }

//         // Add space for '|' separators between arguments
//         length += argc - 4;

//         // Allocate memory to store the data
//         data = (char *)malloc(length + 1); // +1 for null terminator
//         if (data == NULL) {
//             fprintf(stderr, "Memory allocation failed.\n");
//             return;
//         }

//         // Construct the data string with '|' separators
//         strcpy(data, argv[3]);
//         for (int i = 4; i < argc; i++) {
//             strcat(data, "|");
//             strcat(data, argv[i]);
//         }

//         // Open file in binary write mode (truncates existing content)
//         file = fopen(BIN_FILE_NAME, "wb");
//         if (file == NULL) {
//             fprintf(stderr, "Unable to open file.\n");
//             free(data);
//             return;
//         }

//         // Write the data string to the file
//         fwrite(data, sizeof(char), length, file);
//         // fprintf(file, "\n");

//         // Close the file
//         fclose(file);

//         // Free allocated memory
//         free(data);

//         printSuccesBanner();
//     }
    
// }

// char **directories = NULL;
// int dirCount = 0;//directories count

// void test(){
//     for (int i = 0; i < dirCount; i++)
//     {
//         printf("%s|",directories[i]);
//     }
//     printf("\n");
// }

// void readDirectoriesNames(){
//     FILE *file;
//     char *line = NULL;
//     int len = 256; // Initial buffer size
//     int read;

//     // Open the file in binary read mode
//     file = fopen(BIN_FILE_NAME, "rb");
//     if (file == NULL) {
//         fprintf(stderr, "Unable to open file.\n");
//         return;
//     }

//     // Allocate memory for the line buffer
//     line = (char *)malloc(len);
//     if (line == NULL) {
//         fprintf(stderr, "Memory allocation failed.\n");
//         fclose(file);
//         return;
//     }

//     // Read the first line from the file
//     read = fread(line, sizeof(char), len - 1, file);
//     if (read <= 0) {
//         fprintf(stderr, "Error reading file.\n");
//         fclose(file);
//         free(line);
//         return;
//     }
//     line[read] = '\0'; // Null-terminate the string

//     // Close the file
//     fclose(file);

//     // Tokenize the line based on '|'
//     char *token;
//     token = strtok(line, "|");
//     while (token != NULL) {
//         directories = realloc(directories, (dirCount + 1) * sizeof(char *));
//         if (directories == NULL) {
//             fprintf(stderr, "Memory allocation failed.\n");
//             free(line);
//             return;
//         }
//         // printf("size ::%ld",strlen(token));
//         directories[dirCount] = strdup(token);
//         if (directories[dirCount] == NULL) {
//             fprintf(stderr, "Memory allocation failed.\n");
//             free(line);
//             return;
//         }
//         dirCount++;
//         token = strtok(NULL, "|");
//     }

//     // Free the allocated memory for line
//     free(line);
//     //test();
// }

// int getOccurencesNumber(char* dir){
//     int aux = 0;
//     char line[256]; 
//     while (fgets(line, sizeof(line), BIN_FILE_NAME)) {
//         char *token = strtok(line, "|");
//         if(strcmp(line,dir) == 0){
//             aux++;
//         }
//     }
//     return aux;
// }

// int checkForDiffSnap(MetaDataFile_T* files, char* dir,int filesCount){
    
//     MetaDataFile_T oldDataReadFromBin[ARR_SIZE];

//     FILE *file = fopen(BIN_FILE_NAME, "rw+");
//     if (file == NULL) {
//         perror("Error opening file");
//         return -1;
//     }

//     char line[256]; 
//     int occurencesCounter = getOccurencesNumber(dir);
//     while (fgets(line, sizeof(line), file)) {
//         char *token = strtok(line, "|");
//         // printf("Tokensdsdsds: %s\n", line);
//         if(strcmp(line,dir) == 0){
//             // token = strtok(NULL, "|"); // Get the second token files count from bin
//             // if (token != NULL) {
//             //     if(atoi(token) != filesCount){
//             //         int indexForStruct = 0;
//             //         for(int i = 0;i < filesCount;i++){
//             //             fputs("\n", file);
//             //         }
//             //     }//then a file was added or removed so we need to update the data into bin and snapshot
//             // }
//             occurencesCounter++;
//         }
//     }
//     printf("sdos %d\n",occurencesCounter);
    
//     fclose(file);
//     return 0;
// }

