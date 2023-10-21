#include "opcodes.h"
#include "chip8.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void CLS_00E0() { memset(display, 0, DISPLAY_SIZE); }

void RET_00EE() {
    sp--;
    pc = stack[sp];
}

void JP_1nnn() { pc = opcode & 0xFFF; }

void CALL_2nnn() {
    stack[sp] = pc;
    sp++;

    pc = opcode & 0xFFF;
}

void SE_3xkk() {
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t kk = opcode & 0x00FF;

    if (registers[reg] == kk) {
        pc += INSTRUCTION_SIZE;
    }
}

void SNE_4xkk() {
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t kk = opcode & 0x0FF;

    if (registers[reg] != kk) {
        pc += INSTRUCTION_SIZE;
    }
}

void SE_5xy0() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;

    if (registers[reg1] == registers[reg2]) {
        pc += INSTRUCTION_SIZE;
    }
}

void LD_6xkk() {
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t immediate = opcode & 0x00FF;
    registers[reg] = immediate;
}

void ADD_7xkk() {
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t immediate = opcode & 0x00FF;

    registers[reg] = registers[reg] + immediate;
}

void LD_8xy0() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;

    registers[reg1] = registers[reg2];
}

void OR_8xy1() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;

    registers[reg1] |= registers[reg2];
}

void AND_8xy2() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;

    registers[reg1] &= registers[reg2];
}

void XOR_8xy3() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;

    registers[reg1] ^= registers[reg2];
}

void ADD_8xy4() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    uint16_t sum = (uint16_t)registers[reg1] + (uint16_t)registers[reg2];
    uint8_t carry = (sum & 0x0100) >> 8;
    sum &= 0xFF;
    registers[reg1] = sum;
    registers[0xF] = carry;
}

void SUB_8xy5() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    int16_t difference = (int16_t)registers[reg1] - (int16_t)registers[reg2];
    registers[reg1] = (uint8_t)difference;
    registers[0xF] = difference > 0;
}

void SHR_8xy6() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t overwrite = registers[reg1] & 1;

    registers[reg1] = registers[reg1] >> 1;
    registers[0xF] = overwrite;
}

void SUBN_8xy7() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    int16_t difference = (int16_t)registers[reg2] - (int16_t)registers[reg1];
    registers[reg1] = (uint8_t)difference;
    registers[0xF] = difference > 0;

}

void SHL_8xyE() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    uint8_t overwrite = registers[reg1] >> 7;
    registers[reg1] = registers[reg1] << 1;
    registers[0xF] = overwrite;
}

void SNE_9xy0() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;

    if (registers[reg1] != registers[reg2]) {
        pc += INSTRUCTION_SIZE;
    }
}

void LD_Annn() {
    uint16_t immediate = opcode & 0x0FFF;
    index_register = immediate;
}

void JP_Bnnn() {
    uint16_t immediate = opcode & 0x0FFF;
    pc = immediate + registers[0];
}

void RND_Cxkk() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t immediate = (opcode & 0x00FF);
    uint8_t rand_num = (uint8_t)(rand() % UINT8_MAX);
    registers[reg1] = rand_num & immediate;
}


uint8_t get_bit_of_byte(uint8_t byte, uint8_t bit_num) {
    uint8_t and_byte = 1 << (BYTE_SIZE - bit_num);
    return (byte & and_byte) >> bit_num;
}

bool set_pixel(uint8_t row, uint8_t col) {
    uint8_t x = col / BYTE_SIZE;
    uint8_t xMax = DISPLAY_WIDTH / BYTE_SIZE;
    uint8_t y = row;
    uint8_t byte_index = y * xMax + x;
    uint8_t byte = display[byte_index];
    uint8_t exact_bit = col % BYTE_SIZE;
    uint8_t set_bit = 1 << exact_bit;
    byte |= set_bit;

    display[byte_index] = byte;
    return !get_bit_of_byte(byte, exact_bit); 
}

void DRW_Dxyn() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg2 = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F);

    uint8_t xPos = registers[reg1] % DISPLAY_WIDTH;
    uint8_t yPos = registers[reg2] % DISPLAY_HEIGHT;

    registers[0xF] = 0;
    for (uint8_t i = 0; i < n; i++) {
        uint8_t sprite_byte = memory[index_register + i];
        uint8_t row = yPos + i;
        for (uint8_t k = 0; k < BYTE_SIZE; k++) {
            uint8_t col = xPos + k;
            uint8_t sprite_pixel = (sprite_byte & (0x80 >> k)) >> (7 - k);
            uint8_t *display_byte = &display[row * (DISPLAY_WIDTH / BYTE_SIZE) + col / BYTE_SIZE];
            uint8_t display_pixel = (*display_byte & (1 << col % BYTE_SIZE)) >> (col % BYTE_SIZE);
            if (sprite_pixel) {
                if (display_pixel) {
                    registers[0xF] = 1;
                }
                *display_byte ^= sprite_pixel << (col % BYTE_SIZE);
            }
        }
    }
}

void SKP_Ex9E() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    if (kp[registers[reg1]]) {
        pc += 2;
    }
}

void SKNP_ExA1() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    if (!kp[registers[reg1]]) {
        pc += 2;
    }
}

void LD_Fx07() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    registers[reg1] = delay_timer;
}

void LD_Fx0A() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    static bool waiting = false;
    if (!waiting) {
        most_recent_key = -1;
        waiting = true;
    }
    if (most_recent_key < 0) {
        pc -= INSTRUCTION_SIZE;
        return;
    }
    waiting = false;
    registers[reg1] = most_recent_key;
}

void LD_Fx15() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    delay_timer = registers[reg1];
}

void LD_Fx18() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    sound_timer = registers[reg1];
}

void ADD_Fx1E() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    index_register += registers[reg1];
}

void LD_Fx29() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    index_register = FONT_SET_ADDRESS + 5 * registers[reg1];
}

void LD_Fx33() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    uint8_t reg1val = registers[reg1];
    memory[index_register] = reg1val / 100;
    reg1val %= 100;
    memory[index_register + 1] = reg1val / 10;
    reg1val %= 10;
    memory[index_register + 2] = reg1val;
}

void LD_Fx55() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= reg1; i++) {
        memory[index_register + i] = registers[i];
    }
}

void LD_Fx65() {
    uint8_t reg1 = (opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= reg1; i++) {
        registers[i] = memory[index_register + i];
    }
}
