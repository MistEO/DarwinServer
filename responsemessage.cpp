#include "responsemessage.h"

// #include <cstring>
#include <iostream>

std::string ResponseMessage::first_line() const
{
    return version + " " + std::to_string(status()) + " " + status_name() + "\n";
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
    case 403:
        _status_name = "Forbidden";
        break;
    case 404:
        _status_name = "Not Found";
        break;
    case 500:
        _status_name = "Internal Server Error";
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
    for (auto pair : header_map) {
        header_string += pair.first + ":" + pair.second + "\n";
    }
    return header_string;
}

std::string ResponseMessage::to_string() const
{
    return (first_line() + header() + std::string("\n") + data + std::string("\r\n"));
}