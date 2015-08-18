/*
Key point to this question:
PRE_FILE: input from client to server
POST_FILE: output from server to client

fd1: parent to child
fd2: child to parent
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>


#define PRE_FILE "PRE_PIPE"
#define POST_FILE "POST_PIPE"
#define BUFSIZE 80

#define ASCII_A 97
#define ASCII_E 101
#define ASCII_I 105
#define ASCII_O 111
#define ASCII_U 117

int isNotVowel(char c)
{
    /*
    return 0 if the character is a vowel, otherwise return 1

    A vowel is a speech sound made by the vocal cords. It is also a type of letter
    in the alphabet. The letters of the English alphabet are either vowels or
    consonants or both. A vowel sound comes from the lungs, through the vocal cords,
    and is not blocked, so there is no friction.
    */
    if (c != ASCII_A && c != ASCII_E && c != ASCII_I && c != ASCII_O && c != ASCII_U)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void clearBuffer(char *buff)
{
    // I don't think this is a good way to implement this
    int i;
    for (i = 0; i < BUFSIZE; i++)
    {
        buff[i] = '\0';
    }
}

int main(void)
{
    FILE *fp1, *fp2;
    char buffer[BUFSIZE] = {0};
    char modified[BUFSIZE] = {0};

    pid_t childPid;
    int child_status;
    int fd1[2];
    int fd2[2];

    int i, p;

    // create a client to server named pipe (if it doesn't exist)
    umask(0);
    mknod(PRE_FILE, S_IFIFO | 0666, 0);

    while (1)
    {
        clearBuffer(buffer);
        clearBuffer(modified); // dumb way to do this!

        fp1 = fopen(PRE_FILE, "r");

        // read text from client via named pipe, block
        fgets(buffer, BUFSIZE, fp1);
        fclose(fp1);

        // create two unnamed pipe for communication between parent and child
        // note: they use pipe() == -1, i prefer using pipe() < 0 for error testing
        if (pipe(fd1) < 0)
        {
            perror("pipe1");
            exit(1);
        }

        if (pipe(fd2) < 0)
        {
            perror("pipe2");
            exit(1);
        }

        // fork a child
        childPid = fork();
        if (childPid < 0)
        {
            perror("fork");
            exit(2);
        }
        else if (childPid == 0)
        {
            // child code

            // fd1: parent to child, fd2: child to parent
            close(fd1[1]); // don't write to parent via fd1
            if (read(fd1[0], modified, BUFSIZE) < 0)
            {
                perror("read");
                exit(3);
            }

            // compress text
            p = 0;
            for(i = 0; i < strlen(modified); i++)
            {
                modified[i] = tolower(modified[i]);
                char c = modified[i];
                if (isNotVowel(c))
                {
                    modified[p] = c;
                    p++;
                }
            }
            modified[p] = '\0';

            // fd1: parent to child, fd2: child to parent
            close(fd2[0]); // don't read from parent via fd2
            if (write(fd2[1], modified, strlen(modified)) < 0)
            {
                perror("write");
                exit(4);
            }

            exit(0);
            // child finish here
        }
        else
        {
            // parent code


            // send text to child
            // fd1: parent to child, fd2: child to parent
            close(fd1[0]); // don't read to child via fd1
            if (write(fd1[1], buffer, strlen(buffer)) < 0)
            {
                perror("write");
                exit(5);
            }

            // receive compressed message from child
            // fd1: parent to child, fd2: child to parent
            close(fd2[1]); // don't write to child via fd2
            if (read(fd2[0], modified, BUFSIZE) < 0)
            {
                perror("read");
                exit(6);

            }
        }

        // wait child to finish
        wait(&child_status);

        // send compressed message to parent
        fp2 = fopen(POST_FILE, "w");
        fputs(modified, fp2);
        fclose(fp2);
    }
    return 0;
}
