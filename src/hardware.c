#include "chip8.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TIMER_TICK_NS 16666667L

uint8_t registers[NUM_REGISTERS];
uint8_t memory[MEMORY_SPACE];
uint16_t index_register;
uint16_t pc;
uint16_t stack[STACK_SIZE];
uint16_t sp;
uint8_t delay_timer;
uint8_t sound_timer;
uint8_t display[DISPLAY_SIZE];
uint16_t opcode;
uint8_t kp[KEYPAD_SIZE];
FILE *ROM;
int8_t most_recent_key;

void initialize_hardware() {
    memset(registers, 0, NUM_REGISTERS);
    memset(memory, 0, MEMORY_SPACE);
    memset(stack, 0, STACK_SIZE);
    memset(display, 0, DISPLAY_SIZE);
    memset(kp, 0, KEYPAD_SIZE);
    index_register = 0;
    pc = 0;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    opcode = 0;
    ROM = NULL;
    most_recent_key = -1;
}

void update_tick() {
    struct timespec tim, tim2;
    static struct timespec start_time = {0, 0};
    struct timespec end_time = {0, 0};
    if (start_time.tv_nsec == 0 && start_time.tv_sec == 0) {
        clock_gettime(CLOCK_REALTIME, &start_time);
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    double elapsed_time = (double)(end_time.tv_sec - start_time.tv_sec) * 1.0e9 + (double)(end_time.tv_nsec - start_time.tv_nsec);

    if (elapsed_time > (double)TIMER_TICK_NS) {
        if (delay_timer > 0) {
            delay_timer--;
        }
        if (sound_timer > 0) {
            sound_timer--;
        }
        clock_gettime(CLOCK_REALTIME, &start_time);
    } 
}
