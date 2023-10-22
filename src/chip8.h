#pragma once
#include "argp.h"
#include <inttypes.h>
#include <stdio.h>

#define NUM_REGISTERS 16
#define MEMORY_SPACE 4096
#define STACK_SIZE 16
#define INSTRUCTION_SIZE 2
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define BYTE_SIZE 8
#define START_ADDRESS 0x200
#define FONT_SET_ADDRESS 0x50
#define FONTSET_SIZE 80
#define DISPLAY_SIZE DISPLAY_HEIGHT * (DISPLAY_WIDTH / BYTE_SIZE)
#define KEYPAD_SIZE 16
extern uint8_t registers[NUM_REGISTERS];
extern uint8_t memory[MEMORY_SPACE];
extern uint16_t index_register;
extern uint16_t pc;
extern uint16_t stack[STACK_SIZE];
extern uint16_t sp;
extern uint8_t delay_timer;
extern uint8_t sound_timer;
extern uint8_t
    display[DISPLAY_SIZE];
extern uint16_t opcode;
extern uint8_t kp[KEYPAD_SIZE];
extern int8_t most_recent_key;
extern FILE *ROM;
extern uint16_t clock_rate;

static const uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


void initialize_hardware();
void instruction_loop();
uint16_t fetch_instruction();
void * decode_instruction(uint16_t opcode);
void execute_instruction(void (*instruction)());
int8_t atohex(uint8_t ch);
void update_tick();
