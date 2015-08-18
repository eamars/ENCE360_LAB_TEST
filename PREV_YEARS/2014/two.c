#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h> // they always forget this
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 80
const char *named_pipe_filename = "./named_pipe";

int main(void)
{
    int pid;
    int status;

    // that is better
    char parent_buffer[BUFSIZE] = {0};
    char child_buffer[BUFSIZE] = {0};
    char named_pipe_buffer[BUFSIZE] = {0};

    FILE* fp1; /* file pointer for a named pipe (used in the child process)*/
    FILE* fp2; /* file pointer for a named pipe (used in the parent process) */

    int parent_to_child_pipe[2];

    // create pipes
    umask(0);
    mknod(named_pipe_filename, S_IFIFO | 0666, 0); /* create a named pipe */
    pipe(parent_to_child_pipe); /* create an unnamed pipe */

    // create child process
    pid = fork();

    switch(pid)
    {
        case 0:
        // child code
        // get data from parent using unamed pipe
        close(parent_to_child_pipe[1]);
        read(parent_to_child_pipe[0], child_buffer, BUFSIZE);

        printf("\nChild received %i unnamed-piped characters as: %s\n", (int)strlen(child_buffer),child_buffer);

        // send data back to parent using named pipe
        fp1 = fopen(named_pipe_filename, "w");
        fwrite(child_buffer, 1, strlen(child_buffer), fp1); // fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
        fclose(fp1);

        exit(0);

        default:
        // parent and error code
        printf("\nEnter text to pipe: ");
        scanf("%s", parent_buffer);

        // send data to child using an unamed pipe
        close(parent_to_child_pipe[0]);
        write(parent_to_child_pipe[1], parent_buffer, strlen(parent_buffer));

        // get data back from child using a named pipe
        fp2 = fopen(named_pipe_filename, "r");
        fread(named_pipe_buffer, 1, BUFSIZE, fp2);
        fclose(fp2);
    }
}
