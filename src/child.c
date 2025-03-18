/*
 * Программа выводит имя процесса, его PID и PPID, а также переменные окружения.
 * Если программа запускается с параметром "env", она считывает имена переменных
 * окружения из файла "env" и выводит их значения. В противном случае выводит все
 * переменные окружения, переданные через envp.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ENV_FILE "env"

/**
* @brief Выводит переменные окружения, указанные в файле.
* 
* @param filename Имя файла, содержащего список переменных окружения.
*/
void print_env_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
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
}

/**
* @brief Выводит все переменные окружения, переданные через envp.
* 
* @param envp Массив строк, содержащий переменные окружения.
*/
void print_all_env(char *envp[]) {
    for (int i = 0; envp[i]; i++) {
        printf("%s\n", envp[i]);
    }
}

/**
* @brief Основная функция программы.
* 
* @param argc Количество аргументов командной строки.
* @param argv Массив аргументов командной строки.
* @param envp Массив переменных окружения.
* @return int Код завершения программы.
*/
int main(int argc, char *argv[], char *envp[]) {
    if (argc < 2) {
        printf("Использование: %s [env]\n", argv[0]);
        printf("  env - вывести переменные окружения из файла %s\n", ENV_FILE);
        return EXIT_FAILURE;
    }

    printf("Имя процесса: %s\n", argv[0]);
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());

    if (strcmp(argv[1], "env") == 0) {
        print_env_from_file(ENV_FILE);
    } else {
        print_all_env(envp);
    }

    return EXIT_SUCCESS;
}