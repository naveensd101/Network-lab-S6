#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 80
#define PORT 8080
// #define SA struct sockaddr

// Function designed for chat between client and server.
void func(int connfd)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n')
            ;

        // and send that buffer to client
        write(connfd, buff, sizeof(buff));

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    // AF_INET - IPv4, SOCK_STREAM - TCP, 0 - IPPROTO_TCP
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT to the server
    /* struct socketaddr_in {
     *  short   sin_family;
     *  u_short sin_port;
     *  struct  in_addr sin_addr;
     *  char    sin_zero[8];
     * };
     */
    //here we are assigning the port number and IP address to the server
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // any interface
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    // int bind(int socket, const struct sockaddr *address, socklen_t address_len);
    // socket - socket descriptor, address - pointer to the address, address_len - length of the address
    // returns 0 on success, -1 on error
    // bind() function is used to bind a socket to an address.
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    // listen(socketfd, 5) - socket descriptor, 5 - number of connections
    // number specifies the size of the queue of pending connections
    // returns 0 on success, -1 on error
    // listen() function is used to listen for incoming connections.
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    // accept() - accept the connection request from a client
    // returns socket descriptor, -1 on error
    // accept() function is used to accept the connection request from a client.
    // connfd - socket descriptor, cli - client address
    connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");

    // Function for chatting between client and server
    func(connfd);

    // After chatting close the socket
    close(sockfd);
}
