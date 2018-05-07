#include "responsemessage.h"

#include <iostream>
#include <sys/socket.h>

ResponseMessage::ResponseMessage(int status, const std::string& data)
{
    set_status(status);
    set_data(data);
}

std::string ResponseMessage::first_line() const
{
    return _version + " " + std::to_string(_status) + " " + _status_name + "\r\n";
}

int ResponseMessage::status() const
{
    return _status;
}

void ResponseMessage::set_status(const int status)
{
    _status = status;
    switch (status) {
    case 200:
        _status_name = "Ok";
        break;
    case 400:
        _status_name = "Bad Request";
        break;
    case 404:
        _status_name = "Not Found";
        break;
    case 500:
        _status_name = "Internal Server Error";
        break;
    case 503:
        _status_name = "Service Unavailable";
        break;
    }
}

std::string ResponseMessage::status_name() const
{
    return _status_name;
}

std::string ResponseMessage::header() const
{
    std::string header_string;
    for (auto pair : _header_map) {
        header_string += pair.first + ": " + pair.second + "\r\n";
    }
    return header_string;
}

std::string ResponseMessage::to_string() const
{
    return first_line() + header() + "\r\n" + _data;
}

std::map<std::string, std::string>& ResponseMessage::header_map()
{
    return _header_map;
}

const std::string& ResponseMessage::version()
{
    return _version;
}

const std::string& ResponseMessage::data()
{
    return _data;
}

void ResponseMessage::set_data(const std::string& data)
{
    if (!data.empty()) {
        _header_map["Content-Length"] = std::to_string(data.length());
    }
    _data = data;
}

int ResponseMessage::send_to(int connfd) const
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
    std::cout << "send: " << *this << std::endl;
    return send(connfd, this->to_string().data(), this->to_string().length(), 0);
}