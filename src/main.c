#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> // для fork exec pipe
                    //    close read write
#include <sys/wait.h>  // wait waitpid
#include <fcntl.h>  // open
#include <errno.h>  // errno

int main(void)
{
    pid_t pid;
    int my_pipe[2];
    char filename[100];
    int status;
    char path_to_file[300];

    if (pipe(my_pipe) == -1){
        perror("pipe");
        exit(1);
    }

    write(STDOUT_FILENO, "Введите имя файла(без расширения):\n", 63);
    int bytes_read = read(STDIN_FILENO, filename, sizeof(filename) - 1);
    if (bytes_read > 0){
        filename[bytes_read] = '\0';
        char *temp_ch = strchr(filename, '\n');
        if (temp_ch) *temp_ch = '\0';
    }
    else{
        perror("read");
        exit(1);
    }

    // Формируем путь к файлу
    strcpy(path_to_file, "./test/");
    strcat(path_to_file, filename);
    strcat(path_to_file, ".txt");

    //printf("%s\n", path_to_file);

    int file_dt = open(path_to_file, O_RDONLY);
    if (file_dt == -1){
        perror("open");
        exit(1);
    }

    pid = fork();

    if (pid==0) // мы в ребенке
    {
        close(my_pipe[0]); // закрываем на чтение, сможем только писать
        // "Стандартный поток ввода дочернего процесса переопределяется открытым файлом", так что через dup2() переназначим дескриптор файла на дескриптор ввода для child,
        // чтобы для child чтение файла было как stdin 
        if(dup2(file_dt, STDIN_FILENO) == -1) {perror("dup2"); exit(1);}
        close(file_dt);

        // "Запускаем(замещаем)" дочерний процесс нашим child.
        // Первый аргумент - путь до программы, следующие аргументы передаются в main(int argc, char *argv[]), всегда в конце пишется NULL
        execl("./child", "child", NULL);
        perror("execl"); // сработает только если будет ошибка
        exit(1);
    }
    else // мы в родителе
    {
        close(my_pipe[1]); //Закрываем запись, нам нужно только читать

        int bytes_read;
        char buffer[100];

        while ((bytes_read = read(my_pipe[0], buffer, sizeof(buffer))) > 0){
            buffer[bytes_read] = '\0';
            write(STDOUT_FILENO, buffer, bytes_read);
        }

        close(my_pipe[0]);
        // Ждем завершения дочернего процесса и получаем его статус
        
        waitpid(pid, &status, 0);
        /*
        pid - ID дочернего процесса, который мы получили от fork()
        &status - указатель на переменную, куда будет записан статус завершения
        0 - флаги ожидания (0 означает обычное ожидание)
        Функция блокирует выполнение родительского процесса, пока дочерний не завершится
        */
        
        // Проверяем, как завершился дочерний процесс
        /*
        WIFEXITED(status) - макрос, который возвращает true, если процесс завершился нормально
        Если true - процесс вызвал exit() или return из main()*/
        if (WIFEXITED(status)) {
            int child_exit_code = WEXITSTATUS(status); // Извлекаем код возврата дочернего процесса
            if (child_exit_code != 0) {
                return child_exit_code; // Возвращаем код ошибки дочернего процесса
            }

    }

    return 0;
}
}
