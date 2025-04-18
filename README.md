Программа предназначена для работы с переменными окружения и создания дочерних процессов. Она выполняет следующие задачи:

1) С ортировка и вывод переменных окружения:
    - Программа копирует переменные окружения, сортирует их в алфавитном порядке и выводит на экран.

2) Создание дочерних процессов:
    - В зависимости от ввода пользователя программа создает дочерние процессы, которые могут использовать либо специально подготовленное окружение, либо окружение родительского процесса.

3) Обработка ввода пользователя:
    - Программа ожидает ввода пользователя и выполняет соответствующие действия:
        +: Создает дочерний процесс с окружением, прочитанным из файла env;

        *: Создает дочерний процесс с окружением родительского процесса;

        q: Завершает работу программы;

Порядок сборки и запуска:

1. Клонировать репозиторий, прописав git clone https://github.com/Everolfe/parent-child_OSASP, или разархивировать каталог с проектом.

2. Перейти в каталог с проектом cd parent-child_OSASP, или cd "Имя разархивированного каталога".

3. Собрать проект используя make.

4. После сборки проекта можно использовать программу. ./parent

Структура программы:
main:

Устанавливает локаль для корректной сортировки строк.

Копирует переменные окружения в массив, сортирует его и выводит на экран.

Создает среду для дочернего процесса, читая переменные окружения из файла env.

Обрабатывает ввод пользователя и создает дочерние процессы с помощью fork и execve.

compare_strings:

Используется для сортировки массива строк с помощью qsort.

Сравнивает две строки и возвращает результат сравнения.

create_child_env:

Читает файл env и создает массив переменных окружения для дочернего процесса.

Возвращает указатель на массив строк, где каждая строка имеет формат ИМЯ=ЗНАЧЕНИЕ.
