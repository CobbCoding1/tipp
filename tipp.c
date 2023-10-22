#define TIPP_IMPLEMENTATION
#include "tipp.h"

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "USAGE: %s <file_name>\n", argv[0]);
        exit(1);
    }
    char *file_name = argv[1];
    int hashmap_error = hashmap_create(initial_size, &hashmap);
    assert(hashmap_error == 0 && "COULD NOT INITIALIZE HASHMAP\n");
    int length = 0;
    printf("%s\n", prepro(file_name, &length, 0));
}
