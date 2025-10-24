#include <stdio.h>
#include <string.h>

int main() {
    char line[1000];
    FILE *f = popen("./ansilove-utf8ansi /home/tom/Downloads/fire-39/H4-2017.ANS 2>&1 | head -1", "r");
    if (!f) return 1;
    fgets(line, sizeof(line), f);
    pclose(f);
    
    printf("Total bytes: %lu\n", strlen(line));
    
    int visible = 0;
    int i = 0;
    while (line[i]) {
        if (line[i] == '\x1b') {
            while (line[i] && line[i] != 'm') i++;
            i++;
        } else {
            visible++;
            i++;
        }
    }
    printf("Visible chars (excluding ANSI codes): %d\n", visible);
    
    return 0;
}
