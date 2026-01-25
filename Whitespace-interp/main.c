#include <stdio.h>
#include <stdlib.h>

#include "interpreter.h"

char* source;

int main(int argc, char** argv) {

}

void read_from_file(const char* file_name) {
    const FILE* file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    source = (char*)malloc(size + 1);
    fread(source, 1, size, file);
    source[size] = '\0';

    fclose(file);
}