#include <stdio.h>
#include <stdlib.h>

#include "interpreter.h"

void print_version(void);

int main(int argc, char** argv) {

}

void print_version(void) {
    printf("Whitespace-interpreter v0.1\n");
    printf("Implementation of every Whitespace instruction\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);
}