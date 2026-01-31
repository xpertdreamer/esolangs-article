//
// Created by IWOFLEUR on 31.01.2026.
//

#include "command.h"
#include "log.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool piet_execute_cmd(PietCommand cmd, Stack *stack, int block_size) {
    int64_t a, b, c, res;

    switch (cmd) {
        case P_CMD_PUSH: {
            return st_push(stack, block_size);
        }

        case P_CMD_POP: {
            return st_pop(stack);
        }

        case P_CMD_ADD: {
            a = st_pop(stack);
            b = st_pop(stack);
            if (!a || !b)
                return false;
            res = a + b;
            return st_push(stack, res);
        }

        case P_CMD_SUB: {
            a = st_pop(stack);
            b = st_pop(stack);
            if (!a || !b)
                return false;
            res = b -a;
            return st_push(stack, res);
        }

        case P_CMD_MUL: {
            a = st_pop(stack);
            b = st_pop(stack);
            if (!a || !b)
                return false;
            res = a * b;
            return st_push(stack, res);
        }

        case P_CMD_DIV: {
            a = st_pop(stack);
            b = st_pop(stack);
            if (!a || !b)
                return false;
            if (a == 0) {
                PIET_ERROR("Division by zero!\n");
                return false;
            }
            res = b / a;
            return st_push(stack, res);
        }

        case P_CMD_MOD: {
            a = st_pop(stack);
            b = st_pop(stack);
            if (!a || !b)
                return false;
            if (a == 0) {
                PIET_ERROR("Modulo by zero!\n");
                return false;
            }
            res = b % a;
            return st_push(stack, res);
        }

        case P_CMD_NOT: {
            a = st_pop(stack);
            if (!a)
                return false;
            res = (a == 0) ? 1 : 0;
            return st_push(stack, res);
        }

        case P_CMD_GREATER: {
            a = st_pop(stack);
            b = st_pop(stack);
            if (!a || !b)
                return false;
            res = (b > a) ? 1 : 0;
            return st_push(stack, res);
        }

        case P_CMD_POINTER: {
            a = st_pop(stack);
            if (!a)
                return false;
            // I think it will be better handled by interpreter
            return true;
        }

        case P_CMD_SWITCH: {
            a = st_pop(stack);
            if (!a)
                return false;
            // Also handled by interpreter
            return true;
        }

        case P_CMD_DUPL: {
            a = st_peek(stack);
            if (!a)
                return false;
            return st_push(stack, a);
        }

        case P_CMD_ROLL: {
            a = st_pop(stack);
            b = st_pop(stack);
            if (!a || !b)
                return false;
            return st_roll(stack, b, a);
        }

        case P_CMD_IN_NUM: {
            int num;
            if (scanf("%d", &num) != 1) {
                PIET_ERROR("Failed to read number input!\n");
                return false;
            }
            return st_push(stack, num);
        }

        case P_CMD_IN_CHAR: {
            int ch;
            fflush(stdout);
            ch = getchar();
            if (ch == '\n') {
                ch = getchar();
            }
            if (ch == EOF) {
                PIET_ERROR("Unexpected end of input!\n");
                return false;
            }
            return st_push(stack, ch);
        }

        case P_CMD_OUT_NUM: {
            a = st_pop(stack);
            if (!a)
                return false;
            printf("%lld\n", (long long)a);
            fflush(stdout);
            return true;
        }

        case P_CMD_OUT_CHAR: {
            a = st_pop(stack);
            if (!a)
                return false;
            if (a >= 0 && a <= 255) {
                putchar((char)a);
                fflush(stdout);
            } else {
                PIET_ERROR("Invalid char value: %lld\n", (long long)a);
                return false;
            }
            return true;
        }

        case P_CMD_NONE: {
            return true;
        }

        default:
            PIET_ERROR("Unknown command: %d\n", cmd);
            return false;
    }
}