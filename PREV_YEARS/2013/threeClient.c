#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h> // origin code missed errno for error checking
#include <unistd.h> // origin code missed unistd for read, write, close, open


#define PORT_NUMBER "1234" // we use string this time for port number
#define BUFFER_SIZE 512


int main(void)
{
    struct addrinfo hints;
    struct addrinfo *server_address = NULL;

    char buffer[BUFFER_SIZE] = {0};

    // create a tcp connection
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // error check
    if (socket_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo("localhost", PORT_NUMBER, &hints, &server_address);

    connect(socket_fd, server_address->ai_addr, server_address->ai_addrlen);

    // error check
    if (errno != 0)
    {
        perror("connect");
        exit(2);
    }


    int message_length = sprintf(buffer, "hello server");
    message_length = write(socket_fd, buffer, message_length); /* to server */

    // error check
    if (message_length < 0)
    {
        perror("write");
        exit(3);
    }

    message_length = read(socket_fd, buffer, BUFFER_SIZE-1);

    // error check
    if (message_length < 0)
    {
        perror("read");
        exit(4);
    }

    printf("from server: %s\n", buffer);
    close(socket_fd);
    return 0;
}
