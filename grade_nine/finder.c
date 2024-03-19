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
long long int len_str = 0;
int c = 0;

int find_substr(char string[], ssize_t size) {
    for (int i = 0; i < size; ++i) {
        len_str += 1;
        if (string[i] == substr[c]) {
            c += 1;
            if (c == len) {
                return len_str - c;
            }
        }
        else {
            c = 0;
            if (string[i] == substr[c]) {
                c += 1;
            }
        }
    }
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Only 1 arguments expected: <filename_in> <filename_out> <substring>\n");
        exit(-1);
    }

    char *pipe_fd_read = "read.fifo";
    char *pipe_fd_write = "write.fifo";
    char buf[BUF_SIZE];
    ssize_t fd_read;
    ssize_t fd_write;

    substr = argv[1];
    printf("Given substr: %s\n", substr);
    len = strlen(substr);
    printf("Len of given substr: %d\n", len);

    int length_file;

    mknod(pipe_fd_read, S_IFIFO | 0666, 0);
    mknod(pipe_fd_write, S_IFIFO | 0666, 0);

    int read_pipe = open(pipe_fd_read, O_RDONLY);

    if (read_pipe < 0) {
        printf("Can not open pipe.\n");
        exit(-1);
    }

    printf("Searching for first index of given substr.\n");
    int index;

    while ((fd_read = read(read_pipe, buf, BUF_SIZE)) > 0) {
        index = find_substr(buf, fd_read);
        if (index != -1) {
            close(read_pipe);
            break;
        }
        close(read_pipe);
    }

    printf("First index of substring: %d\n", index);
    char answer[sizeof(index)];
    sprintf(answer, "%d", index);

    int write_pipe = open(pipe_fd_write, O_WRONLY);
    if (write_pipe < 0) {
        printf("Can not open pipe.\n");
        exit(-1);
    }
    fd_write = write(write_pipe, answer, strlen(answer));
    close(write_pipe);
    return 0;
}