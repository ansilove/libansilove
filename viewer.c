#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ansilove.h"

void print_help(const char *progname) {
    fprintf(stderr, "Usage: %s [OPTIONS] <ansi-file>... [columns]\n", progname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --speed=BAUD       Simulate modem speed (300, 1200, 2400, 9600, 14400, 28800, 33600, 56000)\n");
    fprintf(stderr, "  --transparent-bg   Use ANSI black (ESC[40m) for black backgrounds (terminal transparency)\n");
    fprintf(stderr, "  --help             Show this help message\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "  %s file.ans                      # Display ANSI art\n", progname);
    fprintf(stderr, "  %s --speed=2400 file.ans         # Simulate 2400 baud modem\n", progname);
    fprintf(stderr, "  %s --transparent-bg file.ans     # Use terminal's transparent black\n", progname);
    fprintf(stderr, "  %s file1.ans file2.ans           # Display multiple files\n", progname);
    fprintf(stderr, "  %s file.ans > output.utf8ansi    # Save to file\n", progname);
    fprintf(stderr, "\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_help(argv[0]);
        return 0;
    }

    int columns = 0;
    int baud_rate = 0;
    int transparent_bg = 0;
    int first_file = 1;
    
    for (int i = 1; i < argc && argv[i][0] == '-'; i++) {
        if (strncmp(argv[i], "--speed=", 8) == 0) {
            baud_rate = atoi(argv[i] + 8);
            first_file++;
        } else if (strcmp(argv[i], "--transparent-bg") == 0) {
            transparent_bg = 1;
            first_file++;
        } else if (strcmp(argv[i], "--help") != 0 && strcmp(argv[i], "-h") != 0) {
            break;
        }
    }
    
    int file_count = argc - first_file;
    
    if (argc >= first_file + 2) {
        char *endptr;
        long val = strtol(argv[argc - 1], &endptr, 10);
        if (*endptr == '\0' && val > 0 && val < 10000) {
            columns = val;
            file_count--;
        }
    }

    for (int i = 0; i < file_count; i++) {
        int file_idx = first_file + i;
        struct ansilove_ctx ctx;
        struct ansilove_options opts;

        memset(&ctx, 0, sizeof(ctx));
        memset(&opts, 0, sizeof(opts));

        if (ansilove_init(&ctx, &opts) != 0) {
            fprintf(stderr, "Init failed: %s\n", argv[file_idx]);
            continue;
        }

        if (ansilove_loadfile(&ctx, argv[file_idx]) != 0) {
            fprintf(stderr, "Load failed: %s\n", argv[file_idx]);
            ansilove_clean(&ctx);
            continue;
        }

        opts.mode = ANSILOVE_MODE_TERMINAL;
        opts.truecolor = transparent_bg ? 1 : 0;
        if (columns > 0) {
            opts.columns = columns;
        }
        
        if (ansilove_terminal(&ctx, &opts) != 0) {
            fprintf(stderr, "Terminal conversion failed: %s\n", argv[file_idx]);
            ansilove_clean(&ctx);
            continue;
        }

        size_t output_len;
        uint8_t *output = ansilove_terminal_emit(&ctx, &output_len);
        
        if (output && output_len > 0) {
            if (baud_rate > 0) {
                int effective_baud = (int)(baud_rate * 4.73);
                int bytes_per_sec = effective_baud / 10;
                int delay_us = 1000000 / bytes_per_sec;
                for (size_t j = 0; j < output_len; j++) {
                    putchar(output[j]);
                    fflush(stdout);
                    usleep(delay_us);
                }
            } else {
                fwrite(output, 1, output_len, stdout);
            }
        }

        ansilove_clean(&ctx);
    }
    
    return 0;
}
