#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#define MAXDATASIZE 1024 // max buffer size

int client_socket(char *hostname, char *port)
{
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct addrinfo their_addrinfo;
    struct addrinfo *their_addr = NULL;

    memset(&their_addrinfo, 0, sizeof(struct addrinfo));

    their_addrinfo.ai_family = AF_INET;
    their_addrinfo.ai_socktype = SOCK_STREAM;
    getaddrinfo(hostname, port, &their_addrinfo, &their_addr);

    // connect to server
    int rc = connect(sockfd, their_addr->ai_addr, their_addr->ai_addrlen);

    if(rc == -1)
    {
        perror("connect");
        exit(1);
    }
    return sockfd;
}


int main(int argc, char **argv)
{
    char buffer[MAXDATASIZE];
    if (argc != 4)
    {
        fprintf(stderr,"usage: threeClient hostname port-number directory\n");
        exit(1);
    }

    // create socket
    int sockfd = client_socket(argv[1], argv[2]);

    // write to socket
    write(sockfd, argv[3], strlen(argv[3]) + 1);

    int numbytes = 0;
    do {
        // read from socket
        numbytes = read(sockfd, buffer, MAXDATASIZE - 1);
        buffer[numbytes] = '\0';
        printf("%s", buffer);
    } while(numbytes > 0);

    printf("\n");
    close(sockfd);

    return 0;
}
