//
// Created by IWOFLEUR on 31.01.2026.
//

#ifndef COMMAND_H
#define COMMAND_H

#include "types.h"
#include "stack.h"

bool piet_execute_cmd(PietCommand cmd, Stack *stack, int block_size);

#endif //COMMAND_H
