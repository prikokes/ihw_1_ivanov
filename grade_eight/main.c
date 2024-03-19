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

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Only 2 arguments expected: <filename_in> <filename_out> <substring>\n");
        exit(-1);
    }

    char *pipe_fd_read = "read.fifo";
    char *pipe_fd_write = "write.fifo";
    char buf[BUF_SIZE];
    ssize_t fd_read;
    ssize_t fd_write;

    int length_file;

    mknod(pipe_fd_read, S_IFIFO | 0666, 0);
    mknod(pipe_fd_write, S_IFIFO | 0666, 0);

    int read_pipe = open(pipe_fd_read, O_WRONLY);

    if (read_pipe < 0) {
        printf("Can not open pipe.\n");
        exit(-1);
    }

    printf("Reading data from file: %s\n", argv[1]);

    int input_file = open(argv[1], O_RDONLY);

    if (input_file < 0) {
        printf("Can not open input file\n");
        exit(-1);
    }
    int read_data = read(input_file, buf, BUF_SIZE);
    if (read_data > 0) {
        length_file = write(read_pipe, buf, read_data);
    }

    printf("Read data from file: %s\n", argv[1]);

    close(input_file);
    close(read_pipe);

    int write_pipe = open(pipe_fd_write, O_RDONLY);
    if (write_pipe < 0) {
        printf("Can not open pipe.\n");
        exit(-1);
    }

    int output_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
    if (output_file < 0) {
        printf("Can not open or create file.\n");
        exit(-1);
    }

    printf("Writing results into the file.\n");

    int bytes = read(write_pipe, buf, BUF_SIZE);

    if (index < 0) {
        printf("Can not read from writing pipe.\n");
        exit(-1);
    }

    ssize_t write_ = write(output_file, buf, bytes);
    close(output_file);
    close(write_pipe);

    return 0;
}