#include <time.h>    // For time_t
#include <dirent.h>

#define FILE_NAME_SIZE 100
#define PATH_SIZE 1000
#define ARR_SIZE 255


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
int init(char const *, int *, MetaDataFile_T*);
int status(int, MetaDataFile_T*);
void printSnapshootIntoFile(MetaDataFile_T *, char*);