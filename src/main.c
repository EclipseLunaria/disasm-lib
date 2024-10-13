#include <stdio.h>
#include <sys/stat.h>

#include "arm_decoder.h"
#include "thumb_decoder.h"

#include "constants.h"
#include "debug.h"

int main(int argc, char* argv[]) {

    char* outfile = "dump.s";
    printf("argc %d\n", argc);
    if (argc == 1) {
        printf("NO FILE SPECIFIED\n");
        return -1;
    }

    FILE* rom = fopen(argv[1], "rb");
    FILE* out = fopen(outfile, "w");

    if (rom == NULL) {
        perror("Error opening file");
        return 1;
    }
    if (out == NULL) {
        perror("Error opening file");
        return 1;
    }
    struct stat st;
    stat(argv[1], &st);
    uint32_t size = st.st_size;
    printf("size: %x\n", size);

    int address = 0;

    uint32_t instruction_buffer[1024];
    memset(instruction_buffer, 0, 1024 * 4);

    char asm_buffer[BUFFER_SIZE];
    memset(asm_buffer, 0, BUFFER_SIZE);

    fprintf(out, "Assembly,Address,Instruction\n");
    while (address < size - 1) {
        int r = fread(instruction_buffer, 4, 1024, rom);
        if (!r)
            break;
        for (uint32_t i = 0; i < r; i++) {
            memset(asm_buffer, 0, BUFFER_SIZE);
            uint32_t instruction = instruction_buffer[i];
            decode_arm_instruction(instruction_buffer[i], asm_buffer);
            fprintf(out, "0x%08x,  %08x, %-32s ;", address * 4, instruction_buffer[i], asm_buffer);
            if (IS_BRANCH(instruction)) {
                int offset = (instruction & 0x7FFFFF);
                if ((instruction >> 23) & 1)
                    offset *= -1;
                fprintf(out, " jumps to instruction 0x%x", address * 4 + 8 + offset);
            }
            fprintf(out, "\n");
            address++;
            fflush(out);
        }
    }
    fflush(out);
    fclose(rom);
    fclose(out);
}