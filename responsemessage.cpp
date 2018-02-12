#include "responsemessage.h"

// #include <cstring>
#include <iostream>

std::string ResponseMessage::status_line() const
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
    switch(status) {
        case 200: _status_name = "Ok"; break;
        case 403: _status_name = "Forbidden"; break;
        case 404: _status_name = "Not Found"; break;
    }
}

std::string ResponseMessage::status_name() const
{
    return _status_name;
}

std::string ResponseMessage::message() const
{
    return (status_line() + header() + std::string("\n") + data);
}