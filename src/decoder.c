#include "chip8.h"
#include "jump_tables.h"
#include "opcodes.h"
#include <ncurses.h>
#include <unistd.h>
#include <sys/time.h>
#include <wchar.h>

uint8_t get_pixel(uint8_t row, uint8_t col) {
    uint8_t x = col / BYTE_SIZE;
    uint8_t xMax = DISPLAY_WIDTH / BYTE_SIZE;
    uint8_t y = row;
    uint8_t exact_bit = col % BYTE_SIZE;
    uint8_t byte_index = y * xMax + x;
    uint8_t byte = display[byte_index];

    uint8_t byte_to_get = byte & (1 << exact_bit);
    uint8_t ret = byte_to_get >> exact_bit;
    return byte_to_get >> exact_bit;
}

void print_display() {
    wchar_t white_block = 0x2588;
    wchar_t space = 0x0020;
    for (int i = 0; i < DISPLAY_HEIGHT; i++) {
        for (int j = 0; j < DISPLAY_WIDTH; j++) {
           mvaddwstr(i,j, get_pixel(i, j) ? &white_block : &space);
        }
    }
    refresh();
}

void instruction_loop() {
    struct timeval start, end, diff;
    while ((opcode = fetch_instruction())) {
        gettimeofday(&start, NULL);
        update_tick();
        void *instruction = decode_instruction(opcode);
        execute_instruction(instruction);
        print_display();
        gettimeofday(&end, NULL);
        if (end.tv_usec < start.tv_usec) {
            diff.tv_usec = 1000000 + end.tv_usec - start.tv_usec;
        } else {
            diff.tv_usec = end.tv_usec - start.tv_usec;
        }
        usleep((1000000 - diff.tv_usec) / clock_rate);
    }
}

uint16_t fetch_instruction() {
    uint16_t instruction = memory[pc];
    instruction = instruction << BYTE_SIZE;
    instruction |= memory[pc + 1];
    pc += INSTRUCTION_SIZE;
    return instruction;
}

void *decode_instruction(uint16_t opcode) {
    uint8_t prefix = (opcode & 0xF000) >> (12);
    switch (prefix) {
    case 0x0:
        return jump_table0(opcode);
    case 0x1:
        return &JP_1nnn;
    case 0x2:
        return &CALL_2nnn;
    case 0x3:
        return &SE_3xkk;
    case 0x4:
        return &SNE_4xkk;
    case 0x5:
        return &SE_5xy0;
    case 0x6:
        return &LD_6xkk;
    case 0x7:
        return &ADD_7xkk;
    case 0x8:
        return jump_table8(opcode);
    case 0x9:
        return &SNE_9xy0;
    case 0xA:
        return &LD_Annn;
    case 0xB:
        return &JP_Bnnn;
    case 0xC:
        return &RND_Cxkk;
    case 0xD:
        return &DRW_Dxyn;
    case 0xE:
        return jump_tableE(opcode);
    case 0xF:
        return jump_tableF(opcode);
    default:
        return NULL;
    }
}

void execute_instruction(void (*instruction)()) {
    if (instruction) {
        (*instruction)();
    }
}

void *jump_table0(uint16_t opcode) {
    uint8_t suffix = (opcode & 0x000F);
    switch (suffix) {
    case 0x0:
        return &CLS_00E0;
    case 0xE:
        return &RET_00EE;
    default:
        return NULL;
    }
}

void *jump_tableE(uint16_t opcode) {
    uint8_t suffix = (opcode & 0x000F);
    switch (suffix) {
    case 0x1:
        return &SKNP_ExA1;
    case 0xE:
        return &SKP_Ex9E;
    default:
        return NULL;
    }
}

void *jump_tableF(uint16_t opcode) {
    uint8_t suffix = (opcode & 0x00FF);

    switch (suffix) {
    case 0x0A:
        return &LD_Fx0A;
    case 0x07:
        return &LD_Fx07;
    case 0x15:
        return &LD_Fx15;
    case 0x18:
        return &LD_Fx18;
    case 0x1E:
        return &ADD_Fx1E;
    case 0x29:
        return &LD_Fx29;
    case 0x33:
        return &LD_Fx33;
    case 0x55:
        return &LD_Fx55;
    case 0x65:
        return &LD_Fx65;
    default:
        return NULL;
    }
}

void *jump_table8(uint16_t opcode) {
    uint8_t suffix = (opcode & 0x000F);
    switch (suffix) {
    case 0x0:
        return &LD_8xy0;
    case 0x1:
        return &OR_8xy1;
    case 0x2:
        return &AND_8xy2;
    case 0x3:
        return &XOR_8xy3;
    case 0x4:
        return &ADD_8xy4;
    case 0x5:
        return &SUB_8xy5;
    case 0x6:
        return &SHR_8xy6;
    case 0x7:
        return &SUBN_8xy7;
    case 0xE:
        return &SHL_8xyE;
    default:
        return NULL;
    }
}

void find_and_execute(opcodeMap *function_table, uint16_t table_size,
                      uint16_t opcode) {
    for (uint16_t i = 0; i < table_size; i++) {
        if (opcode == function_table[i].opcode) {
            (*(function_table[i].operation))();
        }
    }
}

int8_t atohex(uint8_t ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    } else if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    } else if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return -1;
}
