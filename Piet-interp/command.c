//
// Created by IWOFLEUR on 31.01.2026.
//

#include "command.h"
#include "log.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool piet_execute_cmd(PietCommand cmd, Stack *stack, int block_size) {
    int64_t a, b, res;

    switch (cmd) {
        case P_CMD_PUSH:
            return st_push(stack, block_size);

        case P_CMD_POP:
            return st_pop(stack) != 0;  // Just pop, don't care about value

        case P_CMD_ADD:
            a = st_pop(stack);
            b = st_pop(stack);
            res = b + a;
            return st_push(stack, res);

        case P_CMD_SUB:
            a = st_pop(stack);
            b = st_pop(stack);
            res = b - a;
            return st_push(stack, res);

        case P_CMD_MUL:
            a = st_pop(stack);
            b = st_pop(stack);
            res = b * a;
            return st_push(stack, res);

        case P_CMD_DIV:
            a = st_pop(stack);
            b = st_pop(stack);
            if (a == 0) {
                PIET_ERROR("Division by zero!\n");
                return false;
            }
            res = b / a;
            return st_push(stack, res);

        case P_CMD_MOD:
            a = st_pop(stack);
            b = st_pop(stack);
            if (a == 0) {
                PIET_ERROR("Modulo by zero!\n");
                return false;
            }
            res = b % a;
            return st_push(stack, res);

        case P_CMD_NOT:
            a = st_pop(stack);
            res = (a == 0) ? 1 : 0;
            return st_push(stack, res);

        case P_CMD_GREATER:
            a = st_pop(stack);
            b = st_pop(stack);
            res = (b > a) ? 1 : 0;
            return st_push(stack, res);

        case P_CMD_POINTER:
            a = st_pop(stack);
            // Let interpreter handle this
            return true;

        case P_CMD_SWITCH:
            a = st_pop(stack);
            // Let interpreter handle this
            return true;

        case P_CMD_DUPL:
            if (st_is_empty(stack)) return false;
            a = st_peek(stack);
            return st_push(stack, a);

        case P_CMD_ROLL:
            a = st_pop(stack);
            b = st_pop(stack);
            return st_roll(stack, b, a);

        case P_CMD_IN_NUM: {
            int num;
            if (scanf("%d", &num) != 1) {
                return false;
            }
            return st_push(stack, num);
        }

        case P_CMD_IN_CHAR: {
            int ch = getchar();
            if (ch == EOF) return false;
            return st_push(stack, ch);
        }

        case P_CMD_OUT_NUM:
            a = st_pop(stack);
            printf("%lld", (long long)a);
            fflush(stdout);
            return true;

        case P_CMD_OUT_CHAR:
            a = st_pop(stack);
            if (a >= 0 && a <= 255) {
                putchar((char)a);
                fflush(stdout);
                return true;
            }
            return false;

        case P_CMD_NONE:
            return true;

        default:
            return false;
    }
}