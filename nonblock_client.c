#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 设置为非阻塞
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 发起连接（非阻塞时通常立即返回-1，errno为EINPROGRESS）
    int result = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (result < 0 && errno != EINPROGRESS) {
        perror("连接失败");
        close(sockfd);
        return 1;
    }

    // 发送数据
    const char *msg = "hello";
    send(sockfd, msg, strlen(msg), 0);

    // 使用 select 等待服务器回应（最多等待3秒）
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    int ready = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
    if (ready > 0 && FD_ISSET(sockfd, &readfds)) {
        char buffer[1024] = {0};
        int len = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (len > 0) {
            printf("收到服务器回应：%s\n", buffer);
        } else {
            printf("接收失败或服务器关闭连接。\n");
        }
    } else if (ready == 0) {
        printf("等待回应超时。\n");
    } else {
        perror("select错误");
    }

    close(sockfd);
    return 0;
}

