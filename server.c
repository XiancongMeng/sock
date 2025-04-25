#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); // 创建TCP套接字

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888); // 监听端口8888
    server_addr.sin_addr.s_addr = INADDR_ANY; // 本机所有IP地址

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)); // 绑定地址
    listen(server_fd, 5); // 监听队列长度为5

    printf("服务器启动，等待连接...\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len); // 阻塞等待连接
        printf("客户端已连接。\n");

        char buffer[1024] = {0};
        int len = read(client_fd, buffer, sizeof(buffer));
        printf("收到客户端消息：%s\n", buffer);

        write(client_fd, buffer, len); // 回显
        close(client_fd);
    }

    close(server_fd);
    return 0;
}

