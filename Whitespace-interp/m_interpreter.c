//
// Created by IWOFLEUR on 26.01.2026.
//
#include <stdio.h>

#include "interpreter.h"
#include "instruction.h"

void collect_labels(Interpreter* interpreter) {
    ParserState parser = interpreter->parser;
    int saved_pos = parser.position;

    // Сброс
    parser.position = 0;
    parser.line = 1;
    parser.col = 1;
    interpreter->label_count = 0;

    while (parser.position < parser.length) {
        char c = parse_next_char(&parser);

        if (c == LINEFEED) {
            c = parse_next_char(&parser);
            if (c == SPACE) {
                c = parse_next_char(&parser);
                if (c == SPACE) {
                    // Mark label
                    int label = parse_label(&parser);
                    fc_add_label(interpreter, label, parser.position);
                    // printf("DEBUG: Found label %d at position %d\n", label, parser.position);
                }
            }
        }
    }

    interpreter->parser.position = saved_pos;
    // printf("DEBUG: Collected %d labels\n", interpreter->label_count);
}

bool exec_instruction(Interpreter* interpreter) {
    char c = parse_next_char(&interpreter->parser);

    // Skip comments (non-whitespace characters)
    while (c != SPACE && c != TAB && c != LINEFEED && c != EOF) {
        c = parse_next_char(&interpreter->parser);
    }

    if (c == EOF) {
        return false;
    }

    switch (c) {
        case SPACE:
            // Stack manipulation
            c = parse_next_char(&interpreter->parser);
            if (c == SPACE) {
                instr_push(interpreter);
            } else if (c == LINEFEED) {
                c = parse_next_char(&interpreter->parser);
                if (c == SPACE) {
                    instr_duplicate(interpreter);
                } else if (c == TAB) {
                    c = parse_next_char(&interpreter->parser);
                    if (c == SPACE) {
                        // Copy n
                        int n = parse_number(&interpreter->parser);
                        int value = st_peek(interpreter->stack, n);
                        st_push(interpreter->stack, value);
                    } else if (c == LINEFEED) {
                        // Slide n
                        int n = parse_number(&interpreter->parser);
                        int top = st_pop(interpreter->stack);
                        interpreter->stack->top -= n;
                        if (interpreter->stack->top < -1) {
                            interpreter->stack->top = -1;
                        }
                        st_push(interpreter->stack, top);
                    } else {
                        // Swap (c == '\n')
                        instr_swap(interpreter);
                    }
                } else if (c == LINEFEED) {
                    // Discard
                    instr_discard(interpreter);
                }
            }
            break;

        case TAB: {
            c = parse_next_char(&interpreter->parser);
            switch (c) {
                case SPACE: {
                    // Arithmetic
                    c = parse_next_char(&interpreter->parser);
                    c = parse_next_char(&interpreter->parser);

                    if (c == SPACE) {
                        instr_add(interpreter);
                    } else if (c == TAB) {
                        instr_sub(interpreter);
                    } else if (c == LINEFEED) {
                        instr_mul(interpreter);
                    } else {
                        c = parse_next_char(&interpreter->parser);
                        c = parse_next_char(&interpreter->parser);
                        if (c == SPACE) {
                            instr_div(interpreter);
                        } else if (c == TAB) {
                            instr_mod(interpreter);
                        }
                    }
                    break;
                }

                case TAB: {
                    // Heap access
                    c = parse_next_char(&interpreter->parser);
                    if (c == SPACE) {
                        instr_heap_store(interpreter);
                    } else if (c == TAB) {
                        instr_heap_retrieve(interpreter);
                    }
                    break;
                }

                case LINEFEED: {
                    // I/O
                    c = parse_next_char(&interpreter->parser);
                    c = parse_next_char(&interpreter->parser);

                    if (c == SPACE) {
                        instr_out_char(interpreter);
                    } else if (c == TAB) {
                        instr_out_num(interpreter);
                    } else {
                        c = parse_next_char(&interpreter->parser);
                        c = parse_next_char(&interpreter->parser);
                        if (c == SPACE) {
                            instr_in_char(interpreter);
                        } else if (c == TAB) {
                            instr_in_num(interpreter);
                        }
                    }
                    break;
                }

                default:
                    // Error: invalid IMP after TAB
                    fprintf(stderr, "Invalid IMP after TAB\n");
                    return false;
            }
            break;
        }

        case LINEFEED: {
            // Flow control
            c = parse_next_char(&interpreter->parser);

            if (c == SPACE) {
                c = parse_next_char(&interpreter->parser);
                if (c == SPACE) {
                    instr_mark(interpreter);
                } else if (c == TAB) {
                    instr_call_subroutine(interpreter);
                } else if (c == LINEFEED) {
                    instr_jump(interpreter);
                }
            } else if (c == TAB) {
                c = parse_next_char(&interpreter->parser);
                if (c == SPACE) {
                    instr_jump_if_zero(interpreter);
                } else if (c == TAB) {
                    instr_jump_if_neg(interpreter);
                } else if (c == LINEFEED) {
                    instr_ret(interpreter);
                }
            } else if (c == LINEFEED) {
                c = parse_next_char(&interpreter->parser);
                if (c == LINEFEED) {
                    instr_end(interpreter);
                    return false;
                }
            }
            break;
        }

        default:
            // Should never reach here due to comment skipping
            fprintf(stderr, "Unexpected character: %c (ASCII %d)\n", c, c);
            return false;
    }

    return true;
}

void interpreter_run(Interpreter* interpreter) {
    if (!interpreter->parser.source) {
        fprintf(stderr, "No instruction found\n");
        return;
    }

    collect_labels(interpreter);

    interpreter->parser.position = 0;
    interpreter->parser.line = 1;
    interpreter->parser.col = 1;
    interpreter->running = true;

    while (interpreter->running && interpreter->parser.position < interpreter->parser.length)
        exec_instruction(interpreter);
}