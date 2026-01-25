//
// Created by IWOFLEUR on 25.01.2026.
//

#ifndef INTERPRETER_H
#define INTERPRETER_H

// Global options
#define HEAP_SIZE 1024
#define STACK_SIZE 1024
#define BUF_SIZE 1024

// Lexical tokens
#define SPACE ' '
#define TAB '\t'
#define LINEFEED '\n'
#define NULL_TERM '\0'

void read_from_file(const char *file_name);

#endif //INTERPRETER_H
