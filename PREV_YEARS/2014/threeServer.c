#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>


#define MAXDATASIZE 1024 // max buffer size

void escape(char *out, int bufferSize, char *in) {

  char *end = out + bufferSize - 1;

  while(*in != '\0' && out != end) {

    // escape quote (")
    if(*in == '\"') {
      *out++ = '\\';
    }

    // escape backslash (\)
    if(*in == '\\') {
      *out++ = '\\';
    }

    *out++ = *in++;
  }

  *out = '\0';
}

int listen_on(char *port)
{
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sa;

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(atoi(port));

    // bind address to socket
    int rc = bind(s,(struct sockaddr *)&sa,sizeof(sa));

    // error check
    if(rc == -1)
    {
        perror("bind");
        exit(1);
    }

    // listen for incoming connections
    rc = listen (s, 5);
    if(rc == -1)
    {
        perror("listen");
        exit(1);
    }

    return s;
}

int accept_connection(int s)
{
    struct sockaddr_in caller;
    unsigned int length = sizeof(caller);

    // accept incoming connection
    int msgsock = accept(s, (struct sockaddr *)&caller, &length);

    return msgsock;
}

int handle_request(int msgsock, char *dir)
{
    if (fork() == 0)
    {
        // child code

        // redirect stdout to sock and ls to socket as well
        dup2(msgsock, STDOUT_FILENO);
        dup2(msgsock, STDERR_FILENO);

        execl("/bin/ls", "ls", "-l", dir, NULL);
        // close(msgsock) // I don't this this will work. We will never reach here unless
                            // error occurs
        perror("execl");
        exit(1);
    }
    else
    {
        // parent code: do nothing
        close(msgsock);
    }

    return 0;
}


int main(int argc, char **argv)
{
    char buffer[MAXDATASIZE];
    char dir[MAXDATASIZE];

    if (argc != 2)
    {
        fprintf(stderr,"usage: threeServer port-number\n");
        exit(1);
    }

    printf("\nThis is the server with pid %d listening on port %s\n", getpid(), argv[1]);


    // setup the server to bind and listen on a port
    int s = listen_on(argv[1]);

    while (1)
    {
        int msgsock = accept_connection(s);
        read(msgsock, buffer, MAXDATASIZE - 1);

        printf("\nListing directory request: \"%s\"\n", buffer);

        // escape dangerous characters
        escape(dir, MAXDATASIZE, buffer);

        // handle the new request
        handle_request(msgsock, dir);
    }

    close(s);
    exit(0);
}
