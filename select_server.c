#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netinet/in.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    fd_set master_set, read_set;
    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    int max_fd = server_fd;

    printf("服务器启动，等待多个客户端连接...\n");

    while (1) {
        read_set = master_set;
        int ready = select(max_fd + 1, &read_set, NULL, NULL, NULL);
        if (ready < 0) {
            perror("select错误");
            break;
        }

        for (int i = 0; i <= max_fd; ++i) {
            if (FD_ISSET(i, &read_set)) {
                if (i == server_fd) {
                    // 新连接
                    struct sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(client_addr);
                    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
                    FD_SET(client_fd, &master_set);
                    if (client_fd > max_fd) max_fd = client_fd;
                    printf("新客户端连接：fd=%d\n", client_fd);
                } else {
                    // 已连接客户端发来消息
                    char buf[1024] = {0};
                    int len = recv(i, buf, sizeof(buf), 0);
                    if (len <= 0) {
                        printf("客户端断开：fd=%d\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                    } else {
                        printf("收到消息(fd=%d)：%s\n", i, buf);
                        send(i, buf, len, 0); // 回显
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}

