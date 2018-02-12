#pragma once

#include "abstractmessage.h"

class RequestMessage: public AbstractMessage
{
public:
    enum RequestType {
        Unknown,
        Get,
        Head,
        Post
    };
    enum ResourceType {
        UnknownResource,
        Image,
        Camera,
        Motor
    };

    RequestMessage(const std::string & source_message);
    std::string status_line() const;
    RequestType request_type() const;
    ResourceType resource_type() const;

private:
    void _unpack(const std::string & message);
    void _unpack_request_line(const std::string & line);
    void _unpack_header_line(const std::string & line);

    std::string _status_line;
    RequestType _request_type;
    ResourceType _resource_type;
};