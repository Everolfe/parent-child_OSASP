#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 * Программа выводит имя программы, PID, PPID и переменные окружения.
 * Если передан аргумент "env", программа читает переменные окружения из файла "env".
 * Иначе выводит все переменные окружения.
 */

int main(int argc, char *argv[], char *envp[]) {
    // Вывод имени программы, PID и PPID
    printf("Name: %s\n", argv[0]);
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());

    // Проверка аргументов
    if (argc < 2) {
        printf("Usage: %s [env]\n", argv[0]);
        printf("  env: Read environment variables from file 'env'.\n");
        printf("  No arguments: Print all environment variables.\n");
        return EXIT_SUCCESS;
    }

    // Обработка аргумента "env"
    if (strcmp(argv[1], "env") == 0) {
        FILE *file = fopen("env", "r");
        if (!file) {
            perror("fopen");
            return EXIT_FAILURE;
        }

        // Чтение файла построчно
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        while ((read = getline(&line, &len, file)) != -1) {
            // Удаление символа новой строки
            line[strcspn(line, "\n")] = 0;

            // Получение значения переменной окружения
            char *value = getenv(line);
            if (value) {
                printf("%s=%s\n", line, value);
            } else {
                printf("%s is not set\n", line);
            }
        }

        // Освобождение памяти и закрытие файла
        free(line);
        fclose(file);
    } else {
        // Вывод всех переменных окружения
        for (int i = 0; envp[i]; i++) {
            printf("%s\n", envp[i]);
        }
    }

    return EXIT_SUCCESS;
}
