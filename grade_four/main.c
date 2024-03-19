#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>


const int BUF_SIZE = 5000;
char *substr = "";
int len;

int find_substr(char string[], ssize_t size) {
    int c = 0;
    for (int i = 0; i < size; ++i) {
        if (string[i] == substr[c]) {
            c += 1;
            if (c == len) {
                return i - c + 1;
            }
        } else {
            c = 0;
            if (string[i] == substr[c]) {
                c += 1;
            }
        }
    }
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Only 3 arguments expected: <filename_in> <filename_out> <substring>\n");
        exit(-1);
    }

    int pipe_fd_read[2];
    int pipe_fd_write[2];
    char buf[BUF_SIZE];
    ssize_t fd_read;
    ssize_t fd_write;

    substr = argv[3];
    printf("Given substr: %s\n", substr);
    len = strlen(substr);
    printf("Len of given substr: %d\n", len);

    if (pipe(pipe_fd_read)) {
        printf("Can not open pipe\n");
        exit(-1);
    }

    if (pipe(pipe_fd_write)) {
        printf("Can not open pipe\n");
        exit(-1);
    }

    int length_file;

    pid_t pid = fork();
    if (pid < 0) {
        printf("Can not run process\n");
        exit(-1);
    } else if (pid == 0) {
        pid = fork();
        if (pid < 0) {
            printf("Can not run process\n");
            exit(-1);
        } else if (pid == 0) {
            close(pipe_fd_read[1]);
            close(pipe_fd_write[0]);
            printf("Searching for first index of given substr.\n");
            fd_read = read(pipe_fd_read[0], buf, BUF_SIZE);
            int index = find_substr(buf, fd_read);
            printf("First index of substring: %d\n", index);
            char answer[sizeof(index)];
            sprintf(answer, "%d", index);
            fd_write = write(pipe_fd_write[1], answer, strlen(answer));
            close(pipe_fd_read[1]);
            close(pipe_fd_read[0]);
        } else {
            close(pipe_fd_read[0]);
            close(pipe_fd_read[1]);
            close(pipe_fd_write[1]);
            int output_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
            if (output_file < 0) {
                printf("Can not open or create file.\n");
                exit(-1);
            }
            printf("Writing results into the file.\n");
            int bytes = read(pipe_fd_write[0], buf, BUF_SIZE);
            if (index < 0) {
                printf("Can not read from writing pipe.\n");
                exit(-1);
            }
            ssize_t write_ = write(output_file, buf, bytes);
            close(output_file);
            close(pipe_fd_write[0]);
        }
    } else {
        close(pipe_fd_read[0]);
        close(pipe_fd_write[0]);
        close(pipe_fd_write[1]);
        printf("Reading data from file: %s\n", argv[1]);
        int input_file = open(argv[1], O_RDONLY);
        if (input_file < 0) {
            printf("Can not open input file\n");
            exit(-1);
        }
        int read_data = read(input_file, buf, BUF_SIZE);
        if (read_data > 0) {
            length_file = write(pipe_fd_read[1], buf, read_data);
        }
        printf("Read data from file: %s\n", argv[1]);
        close(input_file);
        close(pipe_fd_read[1]);
    }
    return 0;
}