#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

void * client_fun(void * arg);

int main(){
    //创建套接字
    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //将套接字和IP、端口绑定
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));  //初始化用NULL填充
    server_address.sin_family = AF_INET;  //使用IPv4地址
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    server_address.sin_port = htons(1240);  //端口
    bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address));

    //进入监听状态，等待用户发起请求
    listen(socket_fd, 20);

    //主循环，接受客户端请求，开启新线程接受和发送消息
    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        //接受客户端请求
        int client_sock = accept(socket_fd, (struct sockaddr*)&client_address, &client_address_size);

        //输出客户端Ip地址及端口号
        char client_ip[INET_ADDRSTRLEN] = "";
        inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Accepted, ip=%s, port=%d\n", client_ip, ntohs(client_address.sin_port));

        //创建并分离线程
        pthread_t tid;
        pthread_create(&tid, NULL, client_fun, &client_sock);
        pthread_detach(tid);
    }
    
    //关闭套接字
    close(socket_fd);
    return 0;
}

void * client_fun(void * arg)
{
    int recv_len = 0;
    char recv_buf[1024] = ""; 
    int connfd = *(int*)arg;
    
    while((recv_len = recv(connfd, recv_buf, sizeof(recv_buf), 0)) > 0) {  
        printf("recv_buf: %s\n", recv_buf); // 打印数据  
        send(connfd, recv_buf, recv_len, 0); // 给客户端回数据
        bzero(recv_buf, sizeof(recv_buf));
    }

    printf("client closed\n");
    //关闭套接字
    close(connfd);

    return NULL;
}