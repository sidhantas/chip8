#include <ncurses.h>
#include <inttypes.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>

#include "IO.h"
#include "chip8.h"
#include "opcodes.h"

void initialize_font() {
    for (uint16_t i = 0; i < FONTSET_SIZE; i++) {
        memory[FONT_SET_ADDRESS + i] = fontset[i];
    }
}

void load_rom(char const *filename) {
    struct stat st;
    ROM = fopen(filename, "r");
    fstat(fileno(ROM), &st);
    uint16_t buffer_size = st.st_size;
    uint8_t *buffer = malloc(buffer_size);
    fread(buffer, sizeof(uint8_t), buffer_size, ROM);

    for (int i = 0; i < buffer_size; i++) {
        memory[i + START_ADDRESS] = buffer[i];
    }
    pc = START_ADDRESS;
    fclose(ROM);
    free(buffer);
}

void dump_memory() {
    for (int i = START_ADDRESS; i < MEMORY_SPACE; i += 2) {
        uint16_t instruction = memory[i];
        instruction = instruction << BYTE_SIZE;
        instruction |= memory[i + 1];
        printf("%d: 0x%x\n", i - START_ADDRESS, instruction);
    }
}

int main(int argc, char **argv) {
    pthread_t id;
    srand(time(NULL));
    initialize_hardware();
    initialize_font();
    setlocale(LC_CTYPE, "");
    load_rom(argv[1]);
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    clear();
    pthread_create(&id, NULL, enable_keyboard_raw_mode, NULL);
    instruction_loop();
    // dump_memory();
    pthread_join(id, NULL);
    fflush(stdout);
    endwin();
}
