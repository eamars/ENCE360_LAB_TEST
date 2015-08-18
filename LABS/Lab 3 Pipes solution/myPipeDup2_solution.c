/* Title: pipedup2.c
 * Description: ENCE360 Example code - dup2.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
    int fd[2];
    int fd2[2];
    pid_t childpid;
    pid_t childpid2;

    pipe(fd2);

    if((childpid2 = fork() == 0)) { /* Child and Grandchild code */

        pipe(fd);
        if ((childpid = fork()) == 0) { /* Grandchild code: Runs ls -l */
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
            execl("/bin/ls", "ls", "-l", NULL);
            perror("The exec of ls failed");
        } 

        else { /* Child code: Runs sort -k +9 */
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);
            dup2(fd2[1], STDOUT_FILENO);
            close(fd2[0]);
            close(fd2[1]);
            execl("/bin/sort", "sort", "-k", "+9", NULL);
            /* Note: The location of sort depends on your distribution. 
             * Use 'which sort' to find the correct location */
            perror("The exec of sort failed");
        }

    }
    else { /* Parent code: Runs head -5 */
        dup2(fd2[0], STDIN_FILENO);
        close(fd2[0]);
        close(fd2[1]);
        execl("/usr/bin/head", "head", "-5", NULL);
        perror("The exec of head failed");
    } 
   
    exit(0);
}
