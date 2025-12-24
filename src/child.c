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
    int a, b, c, sum = 0;

    int count_while = 0; // чтобы если что выйти из while

    char input_buffer[200];
    char* ptr_input_buf = input_buffer;
    int bytes_read;

    bytes_read = read(STDIN_FILENO, input_buffer, sizeof(input_buffer) - 1);
    
    if (bytes_read > 0) {
        input_buffer[bytes_read] = '\0';

        if (sscanf(ptr_input_buf, "%d %d %d", &a, &b, &c) != 3){
            char error_msg[] = "Ошибка чтения чисел\n";
            write(STDOUT_FILENO, error_msg, sizeof(error_msg) - 1);
            exit(1);
        }

        while (*ptr_input_buf != '\0'){
            
            if (sscanf(ptr_input_buf, "%d %d %d", &a, &b, &c) == 3) {
                sum = 0;
                sum = a + b + c;
            }
            else{
                break;
            }

            char result[100];
            int len = snprintf(result, sizeof(result), "Результат: %d \n", sum);
            write(STDOUT_FILENO, result, len);

            ptr_input_buf = strchr(ptr_input_buf, '\n');
            if (ptr_input_buf == NULL) break;
            ptr_input_buf++;

            count_while+=1;
            if (count_while >= 100){
                char error_msg[] = "Ошибка бесконечный while\n";
                write(STDOUT_FILENO, error_msg, sizeof(error_msg) - 1);
                exit(1);
            }
        }
    }


    return 0;
}
