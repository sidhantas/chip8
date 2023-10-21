#include "chip8.h"
#include "opcodes.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ASSERT(message, test) do { if (!(test)) return message; } while (0)
#define run_test(test) do { char *message = test(); tests_run++; \
    if (message) return message; } while (0)
int tests_run;

static char * test_CLS_00E0_clears_display() {
    initialize_hardware();
    display[0] = 255;
    opcode = 0x00E0; 
    CLS_00E0();

    ASSERT("Error Display Did Not Clear", display[0] == 0);
    return NULL;
}

static char * test_RET_00EE() {
    initialize_hardware();
    const uint16_t initial_pc = 16;
    opcode = 0x20FF;
    pc = initial_pc;
    CALL_2nnn();
    RET_00EE();

    ASSERT("pc not set correctly", pc == initial_pc + INSTRUCTION_SIZE); 
    ASSERT("sp not in correct position", sp == 0);
    return NULL;
}

static char * test_JP_1nnn() {
   initialize_hardware();
   const uint16_t jump_addr = 0x333;
   const uint16_t opcode = 0x2000 | jump_addr;

   JP_1nnn();

   ASSERT("pc not set correctly", pc = jump_addr);
   return NULL;
}

static char * test_CALL_2nnn() {
    initialize_hardware();
    uint16_t old_pc = 0x00;
    pc = old_pc;
    opcode = 0x2333;
    CALL_2nnn();

    ASSERT("Invalid Program Counter", pc == 0x333); 
    ASSERT("Stack not updated", stack[sp - 1] == old_pc + INSTRUCTION_SIZE);
    return NULL;
}

static char * test_SE_3xkk() {
    initialize_hardware();
    uint16_t old_pc = 0x00;
    pc = old_pc;
    opcode = 0x3422;
    registers[4] = 0x22;

    SE_3xkk();
    ASSERT("test_SE_3xkk: Invalid PC", pc == old_pc + INSTRUCTION_SIZE);
    return NULL;
}

static char * test_SE_5xy0() {
    initialize_hardware();
    pc = 0x00;
    opcode = 0x5120;
    registers[1] = 5;
    registers[2] = 5;

    SE_5xy0();

    ASSERT("test_SE_5xy0: PC did not skip", pc == 0x2);
    return NULL;
}

static char * test_LD_6xkk() {
    initialize_hardware();
    opcode = 0x6F22;

    LD_6xkk();

    ASSERT("test_LD_6xkk: Invalid immediate in register", registers[0xF] == 0x22);
    return NULL;
}

static char * test_ADD_7xkk() {
    initialize_hardware();
    opcode = 0x7233;
    registers[0x2] = 0x10;
    ADD_7xkk();

    ASSERT("test_ADD_7xkk: Invalid sum in register", registers[0x2] == (0x10 + 0x33));
    return NULL;
}

static char * test_LD_8xy0() {
    initialize_hardware();
    opcode = 0x8120;

    registers[0x2] = 5;

    LD_8xy0();
    ASSERT("test_LD_8xy0: Invlaid value in register", registers[0x1] == 5);
    return NULL;
}

static char * test_OR_8xy1() {
    initialize_hardware();
    opcode = 0x8451;

    registers[0x4] = 5;
    registers[0x5] = 6;

    OR_8xy1();

    ASSERT("test_OR_8xy1: Invalid value in register", registers[0x4] == (5 | 6));
    return NULL;
}

static char * test_AND_8xy2() {
    initialize_hardware();
    opcode = 0x8672;

    registers[0x6] = 0x32;
    registers[0x7] = 0x48;

    AND_8xy2();

    ASSERT("test_AND_8xy2: Invalid value in register", registers[0x6] == (0x32 & 0x48));
    return NULL;
}

static char * test_XOR_8xy3() {
    initialize_hardware();
    opcode = 0x8893;

    registers[0x8] = 0x75;
    registers[0x9] = 0x36;

    XOR_8xy3();

    ASSERT("test_XOR_8xy3: Invalid value in register", registers[0x8] == (0x75 ^ 0x36));
    return NULL;
}

static char * test_ADD_8xy4() {
    initialize_hardware();
    opcode = 0x8AB4;
    registers[0xA] = 0xAA;
    registers[0xB] = 0xFF;

    ADD_8xy4();
    ASSERT("test_ADD_8xy4: Invalid sum in register", registers[0xA] == 0xA9);
    ASSERT("test_ADD_8xy4: Carry Not Set", registers[0xF] == 0x1);
    return NULL;
}

static char * test_SUB_8xy5() {
    initialize_hardware();
    opcode = 0x8CD5;
    registers[0xC] = 0xBB;
    registers[0xD] = 0xAA;

    SUB_8xy5();
    ASSERT("test_SUB_8xy5: Invalid difference in register", registers[0xC] == (0xBB - 0xAA));
    return NULL;
}

static char * test_SHR_8xy6() {
    initialize_hardware();
    opcode = 0x8E16;
    registers[0xE] = 0x9;

    SHR_8xy6();
    ASSERT("test_SHR_8xy6: Invalid value", registers[0xE] == (9 >> 1));
    ASSERT("test_SHR_8xy6: Vf not set", registers[0xF] == 1);
    return NULL;
}

static char * test_SUBN_8xy7() {
    initialize_hardware();
    opcode = 0x8237;
    registers[0x2] = 0x33;
    registers[0x3] = 0x25;

    SUBN_8xy7();
    ASSERT("test_SUBN_8xy7: Invalid result", registers[0x2] == (0xFF - 0xE + 1));
    ASSERT("test_SUBN_8xy7: VF not set", registers[0xF] == 0);
    return NULL;
}

static char * test_SHL_8xyE() {
    initialize_hardware();
    opcode = 0x845E;
    registers[0x4] = 0xF0;

    SHL_8xyE();
    ASSERT("test_SHL_8xyE: Invalid value", registers[0x4] == 0xE0);
    ASSERT("test_SHL_8xye: Flag not set", registers[0xF] == 1);
    return NULL;
}

static char * test_SNE_9xy0() {
    initialize_hardware();
    opcode = 0x9670;
    pc = 16;
    registers[0x6] = 0x15;
    registers[0x7] = 0x16;

    SNE_9xy0();
    ASSERT("test_SNE_9xy0: Invalid PC", pc == 16 + INSTRUCTION_SIZE);
    return NULL;
}

static char * test_LD_Annn() {
    initialize_hardware();
    opcode = 0xA123;
    LD_Annn();

    ASSERT("test_LD_Annn: Invalid Value in Index Register", index_register == 0x123);
    return NULL;
}

static char * test_JP_Bnnn() {
   initialize_hardware();
   opcode = 0xB334;
   registers[0x0] = 0x22;
   
   JP_Bnnn();
   ASSERT("test_JP_Bnnn: Invalid pc", pc == (0x334 + 0x22));
   return NULL;
}

static char * test_RND_Cxkk() {
    initialize_hardware();
    opcode = 0xCE23;
    
    srand(123);
    uint8_t r = rand() % UINT8_MAX;
    r &= 0x23;
    srand(123);
    RND_Cxkk();
    ASSERT("test_RND_Cxkk: Invalid value in register", r == registers[0xE]);
    return NULL;
}

static char * test_SKP_Ex9E() {
    initialize_hardware();
    opcode = 0xE49E;
    registers[0x4] = 4;
    kp[registers[0x4]] = 1;
    pc = 2;

    SKP_Ex9E();
    ASSERT("test_SKP_Ex9E: PC didn't skip", pc == 4);

    opcode = 0xE59E;
    registers[0x5] = 0xF;
    kp[registers[0x5]] = 0;
    pc = 26;

    SKP_Ex9E();
    ASSERT("test_SKP_Ex9E: PC did not stay the same", pc == 26);
    return NULL;
}

static char * test_SKNP_ExA1() {
    initialize_hardware();
    opcode = 0xE7A1;
    registers[0x7] = 0x3;
    kp[registers[0x7]] = 0;
    pc = 24;

    SKNP_ExA1();
    ASSERT("test_SKNP_ExA1: PC didn't skip", pc == 26);
    return NULL;
}

static char * test_fetch_instruction() {
    initialize_hardware();
    pc = START_ADDRESS;
    memory[pc] = 0x84;
    memory[pc + 1] = 0xF5;
    uint16_t fetched_instruction = fetch_instruction();

    ASSERT("test_fetch_instruction: Invalid Instruction Fetched", fetched_instruction == 0x84F5);
    return NULL;
}

static char * test_decode_instruction() {
    initialize_hardware();
    opcode = 0x83E7;
    void * decoded_instruction = decode_instruction(opcode);

    ASSERT("test_decode_instruction: Invalid Function Decoded", decoded_instruction == &SUBN_8xy7);
    return NULL;
}

static char * test_execute_instruction() {
    initialize_hardware();
    opcode = 0xA444;
    void * instruction = &LD_Annn;
    execute_instruction(instruction);
    ASSERT("test_execute_instruction: Instruction not executed", index_register == 0x444); 
    return NULL;
}

static char * all_tests() {
     run_test(test_CLS_00E0_clears_display);
     run_test(test_RET_00EE);
     run_test(test_JP_1nnn);
     run_test(test_CALL_2nnn);
     run_test(test_SE_3xkk);
     run_test(test_SE_5xy0);
     run_test(test_LD_6xkk);
     run_test(test_ADD_7xkk);
     run_test(test_LD_8xy0);
     run_test(test_OR_8xy1);
     run_test(test_AND_8xy2);
     run_test(test_XOR_8xy3);
     run_test(test_ADD_8xy4);
     run_test(test_SUB_8xy5);
     run_test(test_SUBN_8xy7);
     run_test(test_SHL_8xyE);
     run_test(test_SNE_9xy0);
     run_test(test_LD_Annn);
     run_test(test_JP_Bnnn);
     run_test(test_RND_Cxkk);
     run_test(test_SKP_Ex9E);
     run_test(test_SKNP_ExA1);
     run_test(test_fetch_instruction);
     run_test(test_decode_instruction);
     run_test(test_execute_instruction);
     return 0;
 }

 int main(int argc, char **argv) {
     char *result = all_tests();
     if (result != 0) {
         printf("%s\n", result);
     }
     else {
         printf("ALL TESTS PASSED\n");
     }
     printf("Tests run: %d\n", tests_run);
 
     return result != 0;
 }
