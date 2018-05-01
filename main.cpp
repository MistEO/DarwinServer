#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "requestmessage.h"
#include "resourcecontrol.h"
#include "responsemessage.h"
#include "motion.h"

void request_image(int connfd);
void request_audio(int connfd, const std::string& file_path);
void request_stop_audio(int connfd);
void request_motor(int connfd, const std::string& url, const std::string& query);
void request_unknown(int connfd);

void send_string(int connfd, const std::string& buff);
void* client_rs_fun(void* arg);
int make_named_socket(const char* filename);
void* lclient_fun(void* arg);

void got_signal(int);

int main()
{
	signal(SIGABRT, got_signal);
	signal(SIGTERM, got_signal);
	signal(SIGQUIT, got_signal);
	signal(SIGINT,  got_signal);

    int aiui_scoket_fd = make_named_socket("/tmp/Aiui.domain");
    pthread_t ltid;
    pthread_create(&ltid, NULL, lclient_fun, &aiui_scoket_fd);
    pthread_detach(ltid);

    //创建套接字
    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //将套接字和IP、端口绑定
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address)); //初始化用NULL填充
    server_address.sin_family = AF_INET; //使用IPv4地址
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); //具体的IP地址
    server_address.sin_port = htons(1680); //端口
    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address))) {
        close(socket_fd);
        perror("bind");
        return 1;
    }

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
        pthread_create(&tid, NULL, client_rs_fun, &client_sock);
        pthread_detach(tid);
    }

    //关闭套接字
    close(socket_fd);
    close(aiui_scoket_fd);
    return 0;
}

void got_signal(int)
{
	exit(0);
}

void* client_rs_fun(void* arg)
{
    int recv_len = 0;
    char recv_buf[1024] = "";
    int connfd = *(int*)arg;

    while ((recv_len = recv(connfd, recv_buf, sizeof(recv_buf), 0)) > 0) {
        printf("recv: %s\n", recv_buf); // 打印数据
        try {
            RequestMessage recv_message(recv_buf);
            switch (recv_message.request_method()) {
            case RequestMessage::GET:
                if (recv_message.uri_path() == "/image") {
                    request_image(connfd);
                } else if (recv_message.uri_path() == "/stop_audio") {
                    request_stop_audio(connfd);
                } else if (recv_message.uri_path() == "/audio") {
                    request_audio(connfd, RequestMessage::split_query(recv_message.uri_query()).at("filepath"));
                } else if (recv_message.uri_path().find("/motor") == 0) {
					request_motor(connfd, recv_message.uri_path(), recv_message.uri_query());
				} else {
                    request_unknown(connfd);
                }
                break;
            default:
                request_unknown(connfd);
                break;
            }
        } catch (HttpException& exp) {
            std::cerr << exp.what() << std::endl;
            request_unknown(connfd);
        } catch (std::out_of_range& exp) {
			std::cerr << exp.what() << std::endl;
			request_unknown(connfd);
		}
    }
    printf("client closed\n");
    //关闭套接字
    close(connfd);

    return NULL;
}

void request_image(int connfd)
{
    ResponseMessage send_message;
    send_message.header_map["Content-Type"] = "image";
    send_message.set_status(
        ResourceControl::get_image(
            send_message.data,
            send_message.header_map["Cols"],
            send_message.header_map["Rows"],
            send_message.header_map["Step"]));
	send_message.header_map["Content-Length"] = std::to_string(send_message.data.length());
	std::cout << "send: " << send_message << std::endl;
    send_string(connfd, send_message.to_string() + "\r\n");
}

void request_stop_audio(int connfd)
{
    ResponseMessage send_message;
    send_message.set_status(ResourceControl::stop_audio());
    std::cout << "send: " << send_message.to_string() << std::endl;
    send_string(connfd, send_message.to_string());
}

void request_audio(int connfd, const std::string& file_path)
{
    ResponseMessage send_message;
    send_message.set_status(ResourceControl::play_audio(file_path));
    std::cout << "send: " << send_message.to_string() << std::endl;
    send_string(connfd, send_message.to_string());
}

void request_unknown(int connfd)
{
    ResponseMessage send_message;
    send_message.set_status(400);
    std::cout << "send: " << send_message.to_string() << std::endl;
    send_string(connfd, send_message.to_string());
}

void request_motor(int connfd, const std::string& url, const std::string& query)
{
	ResponseMessage send_message;
	if (url == "/motor/walk_start") {
		motion.walk_start();
		send_message.set_status(200);
	} else if (url == "/motor/walk_stop") {
		motion.walk_stop();
		send_message.set_status(200);
	} else if (url == "/motor/walk") {
		int x, y, msec;
		const auto arg_map = RequestMessage::split_query(query);
		if (arg_map.find("x") != arg_map.end()) {
			x = std::stoi(arg_map.at("x"));
		} else {
			x = 5;
		}
		if (arg_map.find("y") != arg_map.end()) {
			y = std::stoi(arg_map.at("y"));
		} else {
			y = 0;
		}
		if (arg_map.find("msec") != arg_map.end()) {
			msec = std::stoi(arg_map.at("msec"));
		} else {
			msec = 2000;
		}
		motion.walk(x, y, msec);
		send_message.set_status(200);
	} else {
		send_message.set_status(400);
	}
	std::cout << "send: " << send_message.to_string() << std::endl;
	send_string(connfd, send_message.to_string());
}

void send_string(int connfd, const std::string& buff)
{
    // size_t pos = 0;
    // for (; pos < buff.length(); pos += maxsize) {
    //     //最后一个包
    //     if (pos + maxsize >= buff.length()) {
    //         send(connfd, buff.substr(pos, buff.length() - 1).data(), buff.length() - 1 - pos, flags);
    //         break;
    //     }
    //     send(connfd, buff.substr(pos, pos + maxsize).data(), maxsize, flags);
    // }
    send(connfd, buff.data(), buff.length(), 0);
}

int make_named_socket(const char* filename)
{
    struct sockaddr_un name;
    int sock;
    size_t size;

    /* Create the socket. */
    sock = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Bind a name to the socket. */
    name.sun_family = AF_LOCAL;
    strncpy(name.sun_path, filename, sizeof(name.sun_path));
    name.sun_path[sizeof(name.sun_path) - 1] = '\0';
    unlink(filename);

    /* The size of the address is
     the offset of the start of the filename,
     plus its length (not including the terminating null byte).
     Alternatively you can just do:
     size = SUN_LEN (&name);
 */
    size = (offsetof(struct sockaddr_un, sun_path) + strlen(name.sun_path));

    if (bind(sock, (struct sockaddr*)&name, size) < 0) {
        perror("bind");
        close(sock);
        unlink(filename);
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 1) < 0) {
        perror("listen");
        close(sock);
        unlink(filename);
        exit(EXIT_FAILURE);
    }

    return sock;
}

void* lclient_fun(void* arg)
{
    int socket_fd = *(int*)arg;

    pid_t aiui_pid = fork();
    if (aiui_pid == 0) {
        if (execl("./Aiui", "./Aiui", NULL) == -1) {
            perror("execl ./Aiui failure");
            // close(socket_fd);
            // return NULL;
        }
    }

    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    //接受客户端请求
    int client_sock = accept(socket_fd, (struct sockaddr*)&client_address, &client_address_size);
    if (client_sock < 0) {
        perror("accept");
    } else {
        std::cout << "Accept local socket" << std::endl;
    }

    client_rs_fun(&client_sock);

    return NULL;
}
