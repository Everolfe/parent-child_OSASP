/*
* Программа сортирует и выводит переменные окружения, а затем запускает дочерние процессы
* в зависимости от ввода пользователя. Дочерние процессы могут использовать либо
* специально созданное окружение, либо окружение родительского процесса.
*/
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <locale.h>

#define MAX_CHILDREN 100
#define CHILD_NAME_LEN 20
#define CHILD_PROGRAM_LEN 255
#define ENV_FILE "env"

extern char **environ;
/**
* @brief Сравнивает две строки для использования в qsort.
* 
* @param a Указатель на первую строку.
* @param b Указатель на вторую строку.
* @return int Результат сравнения строк.
*/
int compare_strings(const void *a, const void *b);

/**
* @brief Создает окружение для дочернего процесса на основе файла.
* 
* @return char** Массив строк, представляющих окружение.
*/
char **create_child_env();

int main(int argc, char *argv[], char *envp[]) {
    setlocale(LC_COLLATE, "C");
    setenv("LC_COLLATE", "C", 1);

    // Sort and print environment
    int env_count = 0;
    while (environ[env_count]) env_count++;
    
    char **sorted_env = malloc((env_count + 1) * sizeof(char *));
    if (!sorted_env) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }
    
    for (int i = 0; i < env_count; i++) {
        sorted_env[i] = environ[i];
    }
    sorted_env[env_count] = NULL;
    
    qsort(sorted_env, env_count, sizeof(char *), compare_strings);
    
    for (int i = 0; sorted_env[i]; i++) {
        printf("%s\n", sorted_env[i]);
    }
    free(sorted_env);

    // Main loop
    int child_counter = 0;
    char input;
    
    while (1) {
        printf("\nMenu:\n");
        printf("+ - Launch child with getenv() path\n");
        printf("* - Launch child with envp path\n");
        printf("& - Launch child with environ path\n");
        printf("q - Quit\n");
        printf("Enter command: ");
        scanf(" %c", &input);
        
        if (input == 'q') {
            break;
        }
        
        if (input != '+' && input != '*' && input != '&') {
            printf("Invalid command\n");
            continue;
        }
        
        if (child_counter >= MAX_CHILDREN) {
            printf("Maximum number of children reached\n");
            continue;
        }
        
        // Get child path
        char *child_path = NULL;
        if (input == '+') {
            child_path = getenv("CHILD_PATH");
        } else if (input == '*') {
            for (int i = 0; envp[i]; i++) {
                if (strncmp(envp[i], "CHILD_PATH=", 11) == 0) {
                    child_path = envp[i] + 11;
                    break;
                }
            }
        } else if (input == '&') {
            for (int i = 0; environ[i]; i++) {
                if (strncmp(environ[i], "CHILD_PATH=", 11) == 0) {
                    child_path = environ[i] + 11;
                    break;
                }
            }
        }
        
        if (!child_path) {
            printf("CHILD_PATH not found\n");
            continue;
        }
        
        // Create child name
        char child_name[CHILD_NAME_LEN];
        snprintf(child_name, sizeof(child_name), "child_%02d", child_counter);
        
        // Create full path
        char child_program[CHILD_PROGRAM_LEN];
        snprintf(child_program, sizeof(child_program), "%s/child", child_path);
        
        // Prepare arguments for execve
        char *child_argv[] = {child_name, "env", NULL};
        
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (input == '+') {
                char **child_env = create_child_env();
                if (!child_env) {
                    exit(EXIT_FAILURE);
                }
                execve(child_program, child_argv, child_env);
                perror("execve failed");
                for (int i = 0; child_env[i]; i++) free(child_env[i]);
                free(child_env);
            } else {
                // For '*' and '&' cases, use parent's environment
                execve(child_program, child_argv, environ);
                perror("execve failed");
            }
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // Parent process
            child_counter++;
            waitpid(pid, NULL, 0);
        } else {
            perror("fork failed");
        }
    }
    
    return EXIT_SUCCESS;
}
/**
* @brief Сравнивает две строки для использования в qsort.
* 
* @param a Указатель на первую строку.
* @param b Указатель на вторую строку.
* @return int Результат сравнения строк.
*/
int compare_strings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

/**
* @brief Создает окружение для дочернего процесса на основе файла.
* 
* @return char** Массив строк, представляющих окружение.
*/
char **create_child_env() {
    const char *required_vars[] = {
        "SHELL", "HOME", "HOSTNAME", "LOGNAME", "LANG", 
        "TERM", "USER", "LC_COLLATE", "PATH", NULL
    };
    
    // First count required vars
    int count = 0;
    while (required_vars[count]) count++;
    
    // Allocate space for env pointers + NULL
    char **env = malloc((count + 1) * sizeof(char *));
    if (!env) {
        perror("malloc failed");
        return NULL;
    }
    
    // Fill the environment
    int i = 0;
    for (int j = 0; required_vars[j]; j++) {
        char *value = getenv(required_vars[j]);
        if (value) {
            env[i] = malloc(strlen(required_vars[j]) + strlen(value) + 2);
            if (!env[i]) {
                perror("malloc failed");
                for (int k = 0; k < i; k++) free(env[k]);
                free(env);
                return NULL;
            }
            sprintf(env[i], "%s=%s", required_vars[j], value);
            i++;
        }
    }
    
    // Add variables from file if it exists
    FILE *file = fopen(ENV_FILE, "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0;
            // Skip if already in required vars
            int skip = 0;
            for (int j = 0; required_vars[j]; j++) {
                if (strcmp(line, required_vars[j]) == 0) {
                    skip = 1;
                    break;
                }
            }
            if (skip) continue;
            
            char *value = getenv(line);
            if (value) {
                env = realloc(env, (i + 2) * sizeof(char *));
                if (!env) {
                    perror("realloc failed");
                    fclose(file);
                    for (int k = 0; k < i; k++) free(env[k]);
                    free(env);
                    return NULL;
                }
                env[i] = malloc(strlen(line) + strlen(value) + 2);
                if (!env[i]) {
                    perror("malloc failed");
                    fclose(file);
                    for (int k = 0; k < i; k++) free(env[k]);
                    free(env);
                    return NULL;
                }
                sprintf(env[i], "%s=%s", line, value);
                i++;
            }
        }
        fclose(file);
    }
    
    env[i] = NULL;
    return env;
}