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

#include "protocolparser.h"
#include "resourcecontrol.h"

void * client_fun(void * arg);
cv::Mat get_image();
void request_get_response(const int connfd, ProtocolParser::ResourceType resource_type);
void request_image(const int connfd);

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

        ProtocolParser::ResourceType resource_type;
        ProtocolParser::RequestType request_type = ProtocolParser::unpack(recv_buf, resource_type);
        switch (request_type) {
            case ProtocolParser::RequestType::Get:
                request_get_response(connfd, resource_type);
                break;
            case ProtocolParser::RequestType::Post:
                break;
            case ProtocolParser::RequestType::Head:
                break;
            case ProtocolParser::RequestType::Unknown:
                break;
            default:
                break;
        }

        bzero(recv_buf, sizeof(recv_buf));
    }

    printf("client closed\n");
    //关闭套接字
    close(connfd);

    return NULL;
}

void request_get_response(const int connfd, ProtocolParser::ResourceType resource_type)
{
    switch (resource_type) {
        case ProtocolParser::ResourceType::Image:
            request_image(connfd);
            break;
        case ProtocolParser::ResourceType::Camera:
            break;
        case ProtocolParser::ResourceType::Motor:
            break;
    }
}

void request_image(const int connfd)
{
    std::vector<uchar> data;
    int cols = 0, rows = 0, step = 0;
    int status = ResourceControl::get_image(data, cols, rows, step);

    std::string status_line = ProtocolParser::pack_status_line(status);

    std::map<std::string, std::string> header_map;
    header_map["cols"] = std::to_string(cols);
    header_map["rows"] = std::to_string(rows);
    header_map["step"] = std::to_string(step);
    std::string header_line = ProtocolParser::pack_header_line(header_map);

    std::string message = status_line + header_line + "\n";
    std::cout << "send: " << message << std::endl;

    // send(connfd, send_str, send_len, 0); // 给客户端回数据
}