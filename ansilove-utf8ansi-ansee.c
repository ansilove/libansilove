#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "ansilove.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.ans> <output.png>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_png = argv[2];

    struct ansilove_ctx ctx;
    struct ansilove_options opts;

    memset(&ctx, 0, sizeof(ctx));
    memset(&opts, 0, sizeof(opts));

    if (ansilove_init(&ctx, &opts) != 0) {
        fprintf(stderr, "Init failed\n");
        return 1;
    }

    if (ansilove_loadfile(&ctx, input_file) != 0) {
        fprintf(stderr, "Load failed: %s\n", input_file);
        return 1;
    }

    opts.mode = ANSILOVE_MODE_TERMINAL;
    
    if (ansilove_terminal(&ctx, &opts) != 0) {
        fprintf(stderr, "Terminal conversion failed\n");
        return 1;
    }

    size_t output_len;
    uint8_t *output = ansilove_terminal_emit(&ctx, &output_len);
    
    if (!output || output_len == 0) {
        fprintf(stderr, "No output generated\n");
        ansilove_clean(&ctx);
        return 1;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        ansilove_clean(&ctx);
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        ansilove_clean(&ctx);
        return 1;
    }

    if (pid == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        
		execlp("ansee", "ansee", "-o", output_png, NULL);
        perror("execlp ansee");
        exit(1);
    } else {
        close(pipefd[0]);
        
        size_t written = 0;
        while (written < output_len) {
            ssize_t n = write(pipefd[1], output + written, output_len - written);
            if (n <= 0) break;
            written += n;
        }
        close(pipefd[1]);
        
        int status;
        waitpid(pid, &status, 0);
        
        ansilove_clean(&ctx);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            fprintf(stderr, "Converted: %s -> %s\n", input_file, output_png);
            return 0;
        } else {
            fprintf(stderr, "ansee failed\n");
            return 1;
        }
    }
}
