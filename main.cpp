#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "responsemessage.h"
#include "requestmessage.h"
#include "resourcecontrol.h"

void * client_fun(void * arg);
void request_get_image(const int connfd);

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
        printf("recv: %s\n", recv_buf); // 打印数据
        RequestMessage recv_message(recv_buf);
        if (recv_message.request_type() == RequestMessage::Get) {
            if (recv_message.resource_type() == RequestMessage::Image) {
                request_get_image(connfd);
            }
        }
        bzero(recv_buf, sizeof(recv_buf));
    }

    printf("client closed\n");
    //关闭套接字
    close(connfd);

    return NULL;
}

void request_get_image(int connfd)
{
    ResponseMessage send_message;
    send_message.header_map["Content-Type"] = "Image";
    send_message.set_status(
        ResourceControl::get_image(
            send_message.data,
            send_message.header_map["Cols"],
            send_message.header_map["Rows"],
            send_message.header_map["Step"]));
    // std::cout << "send: " << send_message.status_line() << std::endl;
    std::string message_str = send_message.message();
    send(connfd, message_str.c_str(), message_str.length(), 0);
}