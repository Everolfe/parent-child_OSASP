#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <locale.h>

#define MAX_CHILDREN 100
int compareStrings(const void *a, const void *b);
char **create_child_env();


int main() {
    // Устанавливаем локаль для сортировки
    setlocale(LC_COLLATE, "C");

    // Сортируем и выводим окружение
    extern char **environ;
    int env_count = 0;
    while (environ[env_count] != NULL) {
        env_count++;
    }

    char **sorted_environ = malloc(sizeof(char *) * (env_count + 1));
    if (!sorted_environ) {
        perror("malloc failed");
        return 1;
    }

    for (int i = 0; i < env_count; i++) {
        sorted_environ[i] = environ[i];
    }
    sorted_environ[env_count] = NULL;

    qsort(sorted_environ, env_count, sizeof(char *), compareStrings);

    for (int i = 0; sorted_environ[i] != NULL; i++) {
        printf("%s\n", sorted_environ[i]);
    }

    free(sorted_environ);

    // Создаем среду для дочернего процесса
    char **child_env = create_child_env();

    // Основной цикл обработки ввода
    int child_counter = 0;
    char input;

    while (1) {
        printf("Parent process: ");
        scanf(" %c", &input); // Чтение ввода пользователя

        if (input == '+' || input == '*') {
            if (child_counter >= MAX_CHILDREN) {
                printf("Reached the maximum number of children.\n");
                continue;
            }

            // Получаем путь к дочернему процессу
            char *child_path = getenv("CHILD_PATH");
            if (!child_path) {
                fprintf(stderr, "CHILD_PATH not set.\n");
                return 1;
            }

            // Формируем имя дочернего процесса
            char child_name[20];
            snprintf(child_name, sizeof(child_name), "child_%02d", child_counter);

            // Формируем полный путь к дочернему процессу
            char child_program[255];
            snprintf(child_program, sizeof(child_program), "%s/child", child_path);

            // Запускаем дочерний процесс
            pid_t pid = fork();
            if (pid == 0) {
                // Дочерний процесс
                if (input == '+') {
                    char *argv[] = {child_name, "env", NULL};
                    execve(child_program, argv, child_env);
                } else if (input == '*') {
                    char *argv[] = {child_name, NULL};
                    execve(child_program, argv, environ); // Передаем окружение родителя
                }
                perror("execve failed");
                exit(EXIT_FAILURE);
            } else if (pid > 0) {
                // Родительский процесс
                child_counter++;
                wait(NULL); // Ожидаем завершения дочернего процесса
            } else {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }
        } else if (input == 'q') {
            break; // Завершаем программу
        } else {
            printf("Invalid input. Try again.\n");
        }
    }

    // Освобождаем память
    for (int i = 0; child_env[i]; i++) {
        free(child_env[i]);
    }
    free(child_env);

    return 0;
}




// Функция для сравнения строк (для qsort)
int compareStrings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

// Функция для создания среды для дочернего процесса
char **create_child_env() {
    FILE *file = fopen("env", "r");
    if (!file) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    char **env = malloc(MAX_CHILDREN * sizeof(char *));
    char line[256];
    int i = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Удаляем символ новой строки
        char *value = getenv(line);
        if (value) {
            env[i] = malloc(strlen(line) + strlen(value) + 2);
            sprintf(env[i], "%s=%s", line, value);
            i++;
        }
    }
    env[i] = NULL;
    fclose(file);
    return env;
}