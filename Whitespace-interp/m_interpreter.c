//
// Created by IWOFLEUR on 26.01.2026.
//
#include <stdio.h>
#include "interpreter.h"
#include "instruction.h"
#include "config.h"

// Instruction Modification Parameter (IMP)
typedef struct {
    char first;
    char second;
    char third;
    char fourth;
} InstructionSequence;

bool exec_instruction(Interpreter* interpreter) {
#ifdef DEBUG
    static int instr_count = 0;
    printf("\n[DEBUG] Position=%d, Stack top=%d, Instr=%d\n",
           interpreter->parser.position,
           interpreter->stack->top,
           instr_count++);
#endif

    InstructionSequence seq = {0};

    // Read first character, skip comments
    seq.first = parse_next_char(&interpreter->parser);
    while (seq.first != SPACE && seq.first != TAB && seq.first != LINEFEED && seq.first != EOF) {
        seq.first = parse_next_char(&interpreter->parser);
    }

    if (seq.first == EOF) {
        return false;
    }

    // STACK MANIPULATION: [SPACE]...
    if (seq.first == SPACE) {
        seq.second = parse_next_char(&interpreter->parser);

        if (seq.second == SPACE) {
            // [S][S][number] - Push
            instr_push(interpreter);

        } else if (seq.second == TAB) {
            seq.third = parse_next_char(&interpreter->parser);

            if (seq.third == SPACE) {
                // [S][T][S][number] - Copy nth item
                int n = parse_number(&interpreter->parser);
                if (interpreter->stack->top - n < 0) {
                    fprintf(stderr, "Copy: stack underflow at line %d\n",
                            interpreter->parser.line);
                    interpreter->running = false;
                    return false;
                }
                int value = st_peek(interpreter->stack, n);
                st_push(interpreter->stack, value);

            } else if (seq.third == LINEFEED) {
                // [S][T][L][number] - Slide n items off stack
                int n = parse_number(&interpreter->parser);
                if (interpreter->stack->top < 0) {
                    fprintf(stderr, "Slide: stack underflow at line %d\n",
                            interpreter->parser.line);
                    interpreter->running = false;
                    return false;
                }
                int top = st_pop(interpreter->stack);
                interpreter->stack->top -= n;
                if (interpreter->stack->top < -1) {
                    interpreter->stack->top = -1;
                }
                st_push(interpreter->stack, top);
            }

        } else if (seq.second == LINEFEED) {
            seq.third = parse_next_char(&interpreter->parser);

            if (seq.third == SPACE) {
                // [S][L][S] - Duplicate top item
                instr_duplicate(interpreter);

            } else if (seq.third == TAB) {
                // [S][L][T] - Swap top two items
                instr_swap(interpreter);

            } else if (seq.third == LINEFEED) {
                // [S][L][L] - Discard top item
                instr_discard(interpreter);
            }
        }

        return interpreter->running;
    }
    // ARITHMETIC, HEAP, I/O: [TAB]...
    if (seq.first == TAB) {
        seq.second = parse_next_char(&interpreter->parser);

        // ------------------------------------------------------------------------
        // ARITHMETIC: [T][S]...
        // ------------------------------------------------------------------------
        if (seq.second == SPACE) {
            seq.third = parse_next_char(&interpreter->parser);

            if (seq.third == SPACE) {
                seq.fourth = parse_next_char(&interpreter->parser);

                if (seq.fourth == SPACE) {
                    // [T][S][S][S] - Addition
                    instr_add(interpreter);

                } else if (seq.fourth == TAB) {
                    // [T][S][S][T] - Subtraction
                    instr_sub(interpreter);

                } else if (seq.fourth == LINEFEED) {
                    // [T][S][S][L] - Multiplication
                    instr_mul(interpreter);
                }

            } else if (seq.third == TAB) {
                seq.fourth = parse_next_char(&interpreter->parser);

                if (seq.fourth == SPACE) {
                    // [T][S][T][S] - Integer division
                    instr_div(interpreter);

                } else if (seq.fourth == TAB) {
                    // [T][S][T][T] - Modulo
                    instr_mod(interpreter);
                }
            }
        }

        // HEAP ACCESS: [T][T]...
        else if (seq.second == TAB) {
            seq.third = parse_next_char(&interpreter->parser);

            if (seq.third == SPACE) {
                // [T][T][S] - Store to heap
                instr_heap_store(interpreter);

            } else if (seq.third == TAB) {
                // [T][T][T] - Retrieve from heap
                instr_heap_retrieve(interpreter);
            }
        }

        // I/O: [T][L]...
        else if (seq.second == LINEFEED) {
            seq.third = parse_next_char(&interpreter->parser);

            if (seq.third == SPACE) {
                seq.fourth = parse_next_char(&interpreter->parser);

                if (seq.fourth == SPACE) {
                    // [T][L][S][S] - Output character
                    instr_out_char(interpreter);

                } else if (seq.fourth == TAB) {
                    // [T][L][S][T] - Output number
                    instr_out_num(interpreter);
                }

            } else if (seq.third == TAB) {
                seq.fourth = parse_next_char(&interpreter->parser);

                if (seq.fourth == SPACE) {
                    // [T][L][T][S] - Read character
                    instr_in_char(interpreter);

                } else if (seq.fourth == TAB) {
                    // [T][L][T][T] - Read number
                    instr_in_num(interpreter);
                }
            }
        }

        return interpreter->running;
    }

    // FLOW CONTROL: [LINEFEED]...
    if (seq.first == LINEFEED) {
        seq.second = parse_next_char(&interpreter->parser);

        if (seq.second == SPACE) {
            seq.third = parse_next_char(&interpreter->parser);

            if (seq.third == SPACE) {
                // [L][S][S][label] - Mark a location
                instr_mark(interpreter);

            } else if (seq.third == TAB) {
                // [L][S][T][label] - Call subroutine
                instr_call_subroutine(interpreter);

            } else if (seq.third == LINEFEED) {
                // [L][S][L][label] - Jump unconditionally
                instr_jump(interpreter);
            }

        } else if (seq.second == TAB) {
            seq.third = parse_next_char(&interpreter->parser);

            if (seq.third == SPACE) {
                // [L][T][S][label] - Jump if top of stack is zero
                instr_jump_if_zero(interpreter);

            } else if (seq.third == TAB) {
                // [L][T][T][label] - Jump if top of stack is negative
                instr_jump_if_neg(interpreter);

            } else if (seq.third == LINEFEED) {
                // [L][T][L] - Return from subroutine
                instr_ret(interpreter);
            }

        } else if (seq.second == LINEFEED) {
            // [L][L][L] - End program
            instr_end(interpreter);
        }

        return interpreter->running;
    }

    // UNKNOWN INSTRUCTION
    fprintf(stderr, "Unexpected character: %c (ASCII %d) at line %d\n",
            seq.first, seq.first, interpreter->parser.line);
    interpreter->running = false;
    return false;
}

void collect_labels(Interpreter* interpreter) {
    // Save current parser state
    int saved_pos = interpreter->parser.position;
    int saved_line = interpreter->parser.line;
    int saved_col = interpreter->parser.col;

    // Reset to beginning
    interpreter->parser.position = 0;
    interpreter->parser.line = 1;
    interpreter->parser.col = 1;
    interpreter->label_count = 0;

    // Scan for all [L][S][S] mark instructions
    while (interpreter->parser.position < interpreter->parser.length) {
        InstructionSequence seq = {0};

        seq.first = parse_next_char(&interpreter->parser);

        if (seq.first == LINEFEED) {
            seq.second = parse_next_char(&interpreter->parser);

            if (seq.second == SPACE) {
                seq.third = parse_next_char(&interpreter->parser);

                if (seq.third == SPACE) {
                    // Found a mark label instruction
                    int label = parse_label(&interpreter->parser);
                    fc_add_label(interpreter, label, interpreter->parser.position);

#ifdef DEBUG
                    printf("DEBUG: Found label %d at position %d\n",
                           label, interpreter->parser.position);
#endif
                }
            }
        }
    }

    // Restore original parser state
    interpreter->parser.position = saved_pos;
    interpreter->parser.line = saved_line;
    interpreter->parser.col = saved_col;

#ifdef DEBUG
    printf("DEBUG: Collected %d labels\n", interpreter->label_count);
#endif
}

void interpreter_run(Interpreter* interpreter) {
    if (!interpreter->parser.source) {
        fprintf(stderr, "No instruction found\n");
        return;
    }

    // First pass: collect all labels
    collect_labels(interpreter);

    // Second pass: execute
    interpreter->parser.position = 0;
    interpreter->parser.line = 1;
    interpreter->parser.col = 1;
    interpreter->running = true;

    while (interpreter->running &&
           interpreter->parser.position < interpreter->parser.length) {
        if (!exec_instruction(interpreter)) {
            break;
        }
    }
}