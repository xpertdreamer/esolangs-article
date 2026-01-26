//
// Created by IWOFLEUR on 26.01.2026.
//

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "interpreter.h"

void instr_push(Interpreter* interpreter);
void instr_duplicate(Interpreter* interpreter);
void instr_swap(Interpreter* interpreter);
void instr_discard(Interpreter* interpreter);
void instr_add(Interpreter* interpreter);
void instr_sub(Interpreter* interpreter);
void instr_mul(Interpreter* interpreter);
void instr_div(Interpreter* interpreter);
void instr_mod(Interpreter* interpreter);
void instr_heap_store(Interpreter* interpreter);
void instr_heap_retrieve(Interpreter* interpreter);
void instr_mark(Interpreter* interpreter);
void instr_call_subroutine(Interpreter* interpreter);
void instr_jump(Interpreter* interpreter);
void instr_jump_if_zero(Interpreter* interpreter);
void instr_jump_if_neg(Interpreter* interpreter);
void instr_ret(Interpreter* interpreter);
void instr_end(Interpreter* interpreter);
void instr_out_char(Interpreter* interpreter);
void instr_out_num(Interpreter* interpreter);
void instr_in_char(Interpreter* interpreter);
void instr_in_num(Interpreter* interpreter);
void fc_add_label(Interpreter* interpreter, int label, int position);
void fc_find_label(Interpreter* interpreter, int label);
char parse_next_char(ParserState *parser);
char parse_peek_char(ParserState *parser);
void parse_skip_ws(ParserState *parser);
int parse_number(ParserState *parser);
int parse_label(ParserState *parser);

#endif //INSTRUCTION_H
