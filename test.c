#include <stdio.h>
#include <string.h>


int main(int argc, char** argv){
    
    if(strcmp(argv[1],"test") == 0){
        printf("maladet");
    }else if(strcmp(argv[1],"test2") == 0){
        printf("maladet2");
    }

    return 0;
}