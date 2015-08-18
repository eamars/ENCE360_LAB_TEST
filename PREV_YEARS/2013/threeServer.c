/*
Key points:
check return code using manual
or use errno <-- this is the global variable or a macro
*/

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h> // origin code missed errno for error checking
#include <unistd.h> // origin code missed unistd for read, write, close, open

#define PORT_NUMBER 1234
#define BUFFER_SIZE 512

int main(void)
{
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in server_address, client_address;

    // create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // error check
    if (server_socket < 0)
    {
        perror("socket");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT_NUMBER);
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(struct sockaddr_in)); /* bind to PORT_NUMBER */

    // error check: if error, the errno is not zero
    if (errno != 0)
    {
        perror("bind");
        exit(2);
    }

    listen(server_socket, 5);

    // error check
    if (errno != 0)
    {
        perror("listen");
        exit(3);
    }

    socklen_t client_address_length = sizeof(struct sockaddr_in);
    int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_length);

    // error check
    if (client_socket < 0)
    {
        perror("accept");
        exit(4);
    }

    int message_length = read(client_socket, buffer, BUFFER_SIZE-1);

    // error check
    if (message_length < 0)
    {
        perror("read");
        exit(5);
    }

    printf("from client: %s\n", buffer);
    message_length = sprintf(buffer, "back to ya client");
    message_length = write(client_socket, buffer, message_length);

    // error check
    if (message_length < 0)
    {
        perror("write");
        exit(6);

    }

    close(client_socket);


    return 0;
}
