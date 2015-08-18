/* Title: pipe.c
 * Description: ENCE360 Example code - Pipes
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

int parent_to_child[2];
int child_to_parent[2];
static char message[BUFSIZ];
int child_status, size;

int main (int argc, char *argv[]) {

    int i = 0;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s message\n", argv[0]);
        exit(1);
    }

    /* Create pipes */
    if (pipe(parent_to_child) == -1) {
        perror("Pipe from");
        exit(2);
    }
    if (pipe(child_to_parent) == -1) {
        perror("Pipe - to");
        exit(2);
    }

    switch (fork()) { /* Fork a new process */

        case -1: /* Error forking */
            perror("Fork");
            exit(3);

        case 0: /* Child code */


            /* Read from parent */
            if (read(parent_to_child[0], message, BUFSIZ) != -1) {
                printf("CHILD: Recieved %s\n",message);
            }
            else {
                perror("Read");
                exit(4);
            }
            /* Capatalize */
            for(i = 0; i < BUFSIZ && message[i] != '\0'; i++) {
                message[i] = toupper(message[i]);
            }

            /* Write to parent */
            write(child_to_parent[1], message, i);

            /* Close pipes */
            close(parent_to_child[0]);
            close(child_to_parent[1]);
            break;

        default: /* Parent code */
            close(parent_to_child[0]);
            close(child_to_parent[1]);
            if (write(parent_to_child[1], argv[1], strlen(argv[1])) != -1) {
                printf("PARENT: Sent %s\n",argv[1]);
            }
            else {
                perror("Write");
                exit(5);
            }

            if(read(child_to_parent[0], message, BUFSIZ) != -1) {
                printf("PARENT: Received %s\n",message);
            }
            else {
                perror("Read");
                exit(5);
            }

            wait(&child_status);

            /* Close pipes */
            close(parent_to_child[1]);
            close(child_to_parent[0]);

    }

    return EXIT_SUCCESS;
}
