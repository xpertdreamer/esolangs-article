//
// Created by IWOFLEUR on 26.01.2026.
//
#include <stdint.h>
#include <stdio.h>

#include "interpreter.h"
#include "instruction.h"
#include "config.h"

#define INSTR_COUNT (sizeof(instruction_table) / sizeof(instruction_table[0]))

typedef struct {
    uint8_t len;                        // length of signature
    char sig[4];                        // SPACE / TAB / LINEFEED
    void (*handler)(Interpreter*);
    bool has_param;                     // Does this instruction have a parameter?
} Instruction;

static const Instruction instruction_table[] = {
    // STACK
    {2, {SPACE,     SPACE},                             instr_push,             true},    // Push number
    {3, {SPACE,     TAB,        SPACE},                 instr_copy,             true},    // Copy nth item (ADDED)
    {3, {SPACE,     TAB,        LINEFEED},              instr_slide,            true},    // Slide n items (ADDED)
    {3, {SPACE,     LINEFEED,   SPACE},                 instr_duplicate,        false},
    {3, {SPACE,     LINEFEED,   TAB},                   instr_swap,             false},
    {3, {SPACE,     LINEFEED,   LINEFEED},              instr_discard,          false},

    // ARITHMETIC
    {4, {TAB,       SPACE,      SPACE,      SPACE},     instr_add,              false},
    {4, {TAB,       SPACE,      SPACE,      TAB},       instr_sub,              false},
    {4, {TAB,       SPACE,      SPACE,      LINEFEED},  instr_mul,              false},
    {4, {TAB,       SPACE,      TAB,        SPACE},     instr_div,              false},
    {4, {TAB,       SPACE,      TAB,        TAB},       instr_mod,              false},

    // HEAP
    {3, {TAB,       TAB,        SPACE},                 instr_heap_store,       false},
    {3, {TAB,       TAB,        TAB},                   instr_heap_retrieve,    false},

    // I/O
    {4, {TAB,       LINEFEED,   SPACE,      SPACE},     instr_out_char,         false},
    {4, {TAB,       LINEFEED,   SPACE,      TAB},       instr_out_num,          false},
    {4, {TAB,       LINEFEED,   TAB,        SPACE},     instr_in_char,          false},
    {4, {TAB,       LINEFEED,   TAB,        TAB},       instr_in_num,           false},

    // FLOW
    {3, {LINEFEED,  SPACE,      SPACE},                 instr_mark,             true},    // Has label param
    {3, {LINEFEED,  SPACE,      TAB},                   instr_call_subroutine,  true},
    {3, {LINEFEED,  SPACE,      LINEFEED},              instr_jump,             true},
    {3, {LINEFEED,  TAB,        SPACE},                 instr_jump_if_zero,     true},
    {3, {LINEFEED,  TAB,        TAB},                   instr_jump_if_neg,      true},
    {3, {LINEFEED,  TAB,        LINEFEED},              instr_ret,              false},
    {3, {LINEFEED,  LINEFEED,   LINEFEED},              instr_end,              false},
};

// Helper to save/restore parser state
typedef struct {
    int position;
    int line;
    int col;
} ParserStateBackup;

static ParserStateBackup save_parser_state(ParserState *p) {
    return (ParserStateBackup){p->position, p->line, p->col};
}

static void restore_parser_state(ParserState *p, ParserStateBackup backup) {
    p->position = backup.position;
    p->line = backup.line;
    p->col = backup.col;
}

bool exec_instruction(Interpreter *interpreter) {
    ParserState *p = &interpreter->parser;

    // Save starting position in case we need to backtrack
    ParserStateBackup start_state = save_parser_state(p);

    // Read first meaningful character (skip comments)
    char first;
    do {
        first = parse_next_char(p);
        if (first == EOF) return false;
    } while (first != SPACE && first != TAB && first != LINEFEED);

    // Try each instruction that starts with this character
    for (size_t i = 0; i < INSTR_COUNT; i++) {
        const Instruction *ins = &instruction_table[i];

        if (ins->sig[0] != first) continue;

        // Restore to after first character
        restore_parser_state(p, start_state);
        parse_next_char(p);  // Re-read first char

        // Try to match the full signature
        bool match = true;
        for (int j = 1; j < ins->len; j++) {
            char c = parse_next_char(p);
            if (c != ins->sig[j]) {
                match = false;
                break;
            }
        }

        if (match) {
            // Success - execute the instruction
            ins->handler(interpreter);
            return interpreter->running;
        }
    }

    // No instruction matched - restore and error
    restore_parser_state(p, start_state);

    // Skip the problematic character so we don't loop
    parse_next_char(p);

    fprintf(stderr, "Unknown instruction at line %d (char: ", p->line);
    if (first == SPACE) fprintf(stderr, "SPACE");
    else if (first == TAB) fprintf(stderr, "TAB");
    else if (first == LINEFEED) fprintf(stderr, "LINEFEED");
    else fprintf(stderr, "%c", first);
    fprintf(stderr, ")\n");

    interpreter->running = false;
    return false;
}

void collect_labels(Interpreter* interpreter) {
    // Save current parser state
    ParserStateBackup saved_state = save_parser_state(&interpreter->parser);

    // Reset to beginning
    interpreter->parser.position = 0;
    interpreter->parser.line = 1;
    interpreter->parser.col = 1;
    interpreter->label_count = 0;

    // First pass: collect all labels
    while (interpreter->parser.position < interpreter->parser.length) {
        ParserStateBackup start_state = save_parser_state(&interpreter->parser);

        // Read first character
        char first = parse_next_char(&interpreter->parser);
        if (first == EOF) break;

        // Skip non-whitespace (comments)
        while (first != SPACE && first != TAB && first != LINEFEED && first != EOF) {
            first = parse_next_char(&interpreter->parser);
        }

        if (first == EOF) break;

        // Check if it's a label definition [L][S][S]
        if (first == LINEFEED) {
            ParserStateBackup before_second = save_parser_state(&interpreter->parser);
            char second = parse_next_char(&interpreter->parser);

            // Skip comments between LINEFEED and second char
            while (second != SPACE && second != TAB && second != LINEFEED && second != EOF) {
                second = parse_next_char(&interpreter->parser);
            }

            if (second == SPACE) {
                ParserStateBackup before_third = save_parser_state(&interpreter->parser);
                char third = parse_next_char(&interpreter->parser);

                // Skip comments
                while (third != SPACE && third != TAB && third != LINEFEED && third != EOF) {
                    third = parse_next_char(&interpreter->parser);
                }

                if (third == SPACE) {
                    // Found a label definition [L][S][S]
                    // Parse the label
                    int label = parse_label(&interpreter->parser);
                    if (label >= 0) {
                        // Label position should be where execution continues
                        // after parsing the entire label instruction
                        fc_add_label(interpreter, label, interpreter->parser.position);
                    }
                    continue; // Continue to next instruction
                } else {
                    // Not a label, restore and skip this instruction
                    restore_parser_state(&interpreter->parser, before_third);
                }
            } else {
                // Not a label, restore and skip this instruction
                restore_parser_state(&interpreter->parser, before_second);
            }
        }

        // If not a label definition, skip this instruction
        // Restore to start and use exec_instruction to skip
        restore_parser_state(&interpreter->parser, start_state);

        // Try to match and skip any instruction
        bool skipped = false;
        for (size_t i = 0; i < INSTR_COUNT; i++) {
            const Instruction *ins = &instruction_table[i];

            ParserStateBackup try_state = save_parser_state(&interpreter->parser);

            // Try to match the signature
            bool match = true;
            for (int j = 0; j < ins->len; j++) {
                char c = parse_next_char(&interpreter->parser);
                if (c != ins->sig[j]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                // Skip parameter if instruction has one
                if (ins->has_param) {
                    if (ins->sig[0] == SPACE && ins->sig[1] == SPACE) {
                        // Push - skip number
                        parse_number(&interpreter->parser);
                    } else if (ins->sig[0] == SPACE && ins->sig[1] == TAB) {
                        // Copy or Slide - skip number
                        parse_number(&interpreter->parser);
                    } else if (ins->sig[0] == LINEFEED &&
                               ins->sig[1] == SPACE &&
                               ins->sig[2] != LINEFEED) {
                        // Flow control with label - skip label
                        parse_label(&interpreter->parser);
                    } else if (ins->sig[0] == LINEFEED &&
                               ins->sig[1] == TAB &&
                               ins->sig[2] != LINEFEED) {
                        // Jump if zero/negative - skip label
                        parse_label(&interpreter->parser);
                    }
                }
                skipped = true;
                break;
            } else {
                restore_parser_state(&interpreter->parser, try_state);
            }
        }

        if (!skipped) {
            // Couldn't skip - move forward one char to avoid infinite loop
            parse_next_char(&interpreter->parser);
        }
    }

    // Restore original parser state
    restore_parser_state(&interpreter->parser, saved_state);
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