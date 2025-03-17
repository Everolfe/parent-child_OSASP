#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[]) {
    printf("Name: %s\n", argv[0]);
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());
    if (argc > 1 && strcmp(argv[1], "env") == 0) {
        FILE *file = fopen("env", "r");
        if (!file) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        char line[256];
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0;
            char *value = getenv(line);
            if (value) {
                printf("%s=%s\n", line, value);
            }
        }
        fclose(file);
    } else {
        for (int i = 0; envp[i]; i++) {
            printf("%s\n", envp[i]);
        }
    }

    return 0;
}