#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "hashmap.h"

const unsigned initial_size = 1;
struct hashmap_s hashmap;

char *pass(char *buffer, int length, int depth);

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

char *get_word(char *buffer, int *index, int length){
    char *word = malloc(sizeof(char));
    int word_size = 0;
    while(isalpha(buffer[*index]) && *index < length){
        word[word_size] = buffer[*index];
        word_size++;
        *index += 1;
    }
    word[word_size] = '\0';
    return word;
} 

char *get_filename(char *buffer, int *index, int length){
    char *word = malloc(sizeof(char));
    int word_size = 0;
    while((isalpha(buffer[*index]) || buffer[*index] == '.' || isdigit(buffer[*index])) && *index < length){
        word[word_size] = buffer[*index];
        word_size++;
        *index += 1;
    }
    word[word_size] = '\0';
    return word;
} 

char *get_value(char *buffer, int *index, int length){
    char *value = malloc(sizeof(char) * 4);
    int value_size = 0;
    while(buffer[*index] != '\n' && *index < length){
        value[value_size] = buffer[*index];
        value_size++;
        *index += 1;
    }
    value[value_size] = '\0';
    return value;
} 

void eof_error(char *buffer, int index, int length){
    if(buffer[index] == '\0' || buffer == NULL || index > length){
        fprintf(stderr, "error: reached end of file\n");
        exit(1);
    }
}

char *prepro(char *file_name, int *length, int depth){
    char *buffer = read_file_to_buff(file_name, length);
    if(!buffer){
        fprintf(stderr, "error reading file\n");
        exit(1);
    }
    char *result = pass(buffer, *length, depth);

    return(result);
}

char *pass(char *buffer, int length, int depth){
    if(depth > 500){
        fprintf(stderr, "error: recursive import detected\n");
        exit(1);
    }
    int index = 0; 
    char *output = malloc(sizeof(char) * 1024);
    int output_index = 0;
    while(index < length){
        if(buffer[index] == '@'){
            index++;
            eof_error(buffer, index, length);
            char *word = get_word(buffer, &index, length);
            if(strcmp(word, "def") == 0){
                index++;
                eof_error(buffer, index, length);
                char *def = get_word(buffer, &index, length);
                index++;
                eof_error(buffer, index, length);
                char *value = get_value(buffer, &index, length);
                int put_error = hashmap_put(&hashmap, def, strlen(def), value);
                assert(put_error == 0 && "COULD NOT PLACE INTO HASHMAP\n");
            } else if(strcmp(word, "imp") == 0){
                index++;
                eof_error(buffer, index, length);
                if(buffer[index] != '"'){
                    fprintf(stderr, "error: expected close paren");
                    exit(1);
                }
                index++;
                eof_error(buffer, index, length);
                char *imported_file = get_filename(buffer, &index, length);
                eof_error(buffer, index, length);
                if(buffer[index] != '"'){
                    fprintf(stderr, "error: expected close paren");
                    exit(1);
                }
                index++;
                eof_error(buffer, index, length);
                int imported_length = 0;
                char *imported_buffer = prepro(imported_file, &imported_length, depth + 1);
                imported_length = strlen(imported_buffer);
                for(int i = 0; i < imported_length; i++){
                    output[output_index] = imported_buffer[i];
                    output_index++;
                }
            } else {
                fprintf(stderr, "Unexpected keyword: %s\n", word);
                exit(1);
            }
        } else if(isalpha(buffer[index])){
            // set a temp index because we dont want to iterate index here
            int temp_index = index;
            char *word = get_word(buffer, &temp_index, length);
            char* const element = hashmap_get(&hashmap, word, strlen(word));
            if(element){
                // if found in hashmap, set the element value instead of word value
                for(size_t i = 0; i < strlen(element); i++){
                    output[output_index] = element[i];
                    output_index++;
                }
                index = temp_index;
            } else {
                // if not found in hashmap, set the word value instead of element value
                for(size_t i = 0; i < strlen(word); i++){
                    output[output_index] = word[i];
                    output_index++;
                }
                index = temp_index;
            }
        } 

        output[output_index++] = buffer[index];
        index++;
    } 
    //output_index--;
    output[output_index] = '\0';
    return(output);
}



int main(){
    int hashmap_error = hashmap_create(initial_size, &hashmap);
    assert(hashmap_error == 0 && "COULD NOT INITIALIZE HASHMAP\n");
    int length = 0;
    printf("%s\n", prepro("test.tasm", &length, 0));
}
