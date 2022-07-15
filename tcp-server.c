#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>

#define PORT        8080
#define MAX_POLL    3
#define LOCAL_IP    "192.168.12.19"
#define BUF_LEN     100

char tset_big_arr[1024*1024];

int recv_fun(int fd, char *buffer, int len) {
    printf("%s\n", buffer);
    write(fd, buffer, len);

    return 0;
}

int main()
{
    int                 ser_fd;
    socklen_t           len;
    struct sockaddr_in  servaddr, cli;
    struct pollfd       pfds[MAX_POLL];
    char                recv_buf[BUF_LEN];
    int                 conn_fd     = -1;
    int                 max_fd_idx  = 0;
    int                 max_fd_cnt  = 0;
    int                 i           = 0;
    int                 ready       = 0;
    int                 recv_num    = 0;

    // socket create and verification
    ser_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ser_fd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(LOCAL_IP);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(ser_fd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else {
        printf("Socket successfully binded..\n");
    }

    // Now server is ready to listen and verification
    if ((listen(ser_fd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else {
        printf("Server listening..\n");
    }
    len = sizeof(cli);

    conn_fd = accept(ser_fd, (struct sockaddr *)&cli, &len);

    read(conn_fd, recv_buf, BUF_LEN);


    char ch = '0';

    for(i = 0; i < 1024*1024; i++) {
        if(ch > '9') {
            ch = '0';
        }
        tset_big_arr[i] = ch++;
    }

    write(conn_fd, tset_big_arr, 1024*1024);
//    sleep(5);
    close(conn_fd);
    sleep(5);

}

