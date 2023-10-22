#include <getopt.h>
#include <inttypes.h>
#include <locale.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

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
    if (!ROM) {
        fprintf(stderr, "Unable to open file, Invalid / Does Not Exist");
        exit(1);
    }
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

void print_usage() {
    printf("Usage: chip8 -i [chip8_rom] -f [cpu_frequency (Hz)] -d\n");
    printf("-i --infile, input file for emulator to execute\n");
    printf("-f --frequency, emulator clock frequency\n");
    printf("-d --dump, dump rom file content\n");
}

int main(int argc, char **argv) {
    pthread_t id;
    int long_index = 0;
    int opt = 0;
    bool dump = false;
    static struct option program_options[] = {
        {"inflile", required_argument, 0, 'i'},
        {"frequency", required_argument, 0, 'f'},
        {"dump", no_argument, 0, 'd'},
        {0, 0, 0, 0}};
    srand(time(NULL));
    initialize_hardware();
    initialize_font();
    setlocale(LC_CTYPE, "");
    while ((opt = getopt_long(argc, argv, "i:f:d", program_options,
                              &long_index)) != -1) {
        switch (opt) {
            case 'i':
                load_rom(optarg);
                break;
            case 'f':
                clock_rate = atoi(optarg);
                break;
            case 'd':
                dump = true;
                break;
            default:
                print_usage();
                exit(1);
                break;
        }
    }

    if (dump) {
        dump_memory();
        fclose(ROM);
        exit(0);
    }
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    clear();
    pthread_create(&id, NULL, enable_keyboard_raw_mode, NULL);
    instruction_loop();
    pthread_join(id, NULL);
    fflush(stdout);
    fclose(ROM);
    endwin();
}
