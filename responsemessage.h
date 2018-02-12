#pragma once

#include "abstractmessage.h"

class ResponseMessage: public AbstractMessage
{
public:
    std::string first_line() const;
    int status() const;
    void set_status(const int status);
    std::string status_name() const;
    
    std::string header() const;
    std::string to_string() const;

private:
    int _status;
    std::string _status_name;
};