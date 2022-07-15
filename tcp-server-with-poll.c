#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>

#define PORT        8080
#define MAX_POLL    3
#define LOCAL_IP    "192.168.12.19"
#define BUF_LEN     100

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
    struct linger       linger      = {1, 0};
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

    // init pfds
    pfds[0].fd = ser_fd;
	pfds[0].events = POLLIN;
	for (i = 1; i < MAX_POLL; i ++) {
		pfds[i].fd = -1;
		pfds[i].revents = 0;
	}
	max_fd_idx = 1;
	max_fd_cnt = 1;

    while (1) {
        ready = poll(pfds, max_fd_idx, -1);
        if (ready < 0) {
            perror("poll failed");
            continue;
        }

        // if get connection requestion
        if (pfds[0].revents & POLLIN) {
            pfds[0].revents = 0;
            // first check if has free space
            if (max_fd_cnt >= MAX_POLL) {
                conn_fd = accept(ser_fd, (struct sockaddr *)&cli, &len);
                setsockopt(conn_fd, SOL_SOCKET, SO_LINGER, (void*)&linger, sizeof(linger));
                write(conn_fd, "close", 6);
                close(conn_fd);
                perror("too many clients");
                goto __recv;
            }

            conn_fd = accept(ser_fd, (struct sockaddr *)&cli, &len);
            if (conn_fd < 0) {
				perror("accept failed");
				goto __recv;
			}

            printf("ip=%s - port=%d connted\n",inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));

            for (i = 1; i < MAX_POLL; i++) {
				if (pfds[i].fd < 0) {
				    pfds[i].fd = conn_fd;
				    pfds[i].events = POLLIN;

		            max_fd_cnt++;
					break;
				}
			}

            if (i >= max_fd_idx) {
                max_fd_idx = i + 1;
            }
        }

        __recv:
        //receive
        for (i = 1; i < max_fd_idx; i++) {
            if (pfds[i].fd > 0 && (pfds[i].revents & POLLIN)) {
                pfds[i].revents = 0;
                recv_num = read(pfds[i].fd, recv_buf, BUF_LEN);
                if (recv_num <= 0) {
                    getpeername(pfds[i].fd, (struct sockaddr *)&cli, &len);
                    printf("ip=%s - port=%d disconnted\n",inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));

                    close(pfds[i].fd);
                    pfds[i].fd = -1;
                    max_fd_cnt--;
                } else {
                    // processing received tcp message
                    recv_buf[recv_num] = 0;
                    recv_fun(pfds[i].fd, recv_buf, recv_num);
                }
            }
        }
    }
}

