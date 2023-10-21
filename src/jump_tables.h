#pragma once
#include <stdint.h>

void *jump_table8(uint16_t opcode);
void *jump_tableE(uint16_t opcode);
void *jump_tableF(uint16_t opcode);
void *jump_table0(uint16_t opcode);
