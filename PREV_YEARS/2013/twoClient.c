#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define PRE_FILE "PRE_PIPE"
#define POST_FILE "POST_PIPE"
#define BUFSIZE 80

int main(void)
{
    // named pipe
    FILE *fp1, *fp2;

    char buffer[BUFSIZE] = {0};
    char received[BUFSIZE] = {0}; // spelling mistake in origin text

    // open an existing client to sever named pipe as input
    fp1 = fopen(PRE_FILE, "w");

    // handle input
    printf("Enter a string to compress (no spaces): ");
    scanf("%s", buffer);
    fputs(buffer, fp1); // we can use fwrite or other methods if we use open pipe as stream
    fclose(fp1); // done

    // create a server to client named pipe
    // mknod, mknodat - create a special or ordinary file (just create file)
    //      int mknod(const char *pathname, mode_t mode, dev_t dev);
    mknod(POST_FILE, S_IFIFO | 0666, 0);

    // read and print message
    fp2 = fopen(POST_FILE, "r");
    fgets(received, BUFSIZE, fp2);
    printf("Received from server: %s\n", received);
    fclose(fp2);

    return 0;
}
