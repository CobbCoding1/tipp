#include <stdio.h>
#include <stdlib.h>

char *read_file_to_buff(char *file_name, int *length){
    FILE *file = fopen(file_name, "r"); 
    if(file == NULL){
        fprintf(stderr, "error: file not found: %s\n", file_name);
        exit(1);
    }

    char *current = {0};
    fseek(file, 0, SEEK_END);
    *length = ftell(file) - 1;
    fseek(file, 0, SEEK_SET);

    current = malloc((sizeof(char)) * (*length));
    fread(current, 1, *length, file);
    if(!current){
        fprintf(stderr, "error: could not read from file: %s\n", file_name);
        fclose(file);
        exit(1);
    }

    fclose(file);
    return current;
}


int main(){
    int length = 0;
    char *buffer = read_file_to_buff("test.tasm", &length);
    if(!buffer){
        fprintf(stderr, "error reading file\n");
        exit(1);
    }
    printf("%s\n", buffer);
}
