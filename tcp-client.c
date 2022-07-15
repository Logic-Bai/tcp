#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define LOCAL_IP    "10.13.22.102"
#define SERVER_IP   "192.168.12.45"
#define LOCAL_PORT  2233
#define SERVER_PORT 8080

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

#ifdef __BIND_PORT
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(LOCAL_IP);
    servaddr.sin_port = htons(LOCAL_PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else {
        printf("Socket successfully binded..\n");
    }
#endif

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

#ifdef __LINGER
    struct linger ling = {1, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (void*)&ling, sizeof(ling));
#endif

    write(sockfd, "hello\n", 7);

#ifdef __LINGER
    sleep(1);
#endif

    // close the socket
    close(sockfd);

    return 0;
}
