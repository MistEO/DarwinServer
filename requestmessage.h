#pragma once

#include "abstractmessage.h"

class RequestMessage: private AbstractMessage
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
        Audio,
        Camera,
        Motor
    };

    RequestMessage(const std::string & source_message);
    std::string first_line() const;
    RequestType request_type() const;
    ResourceType resource_type() const;

    std::string header() const;
    std::string to_string() const;

    const std::map<std::string, std::string> & get_header_map() const;
    const std::string & get_version() const;
    const std::string & get_data() const;

private:
    void _unpack(const std::string & message);
    void _unpack_request_line(const std::string & line);
    void _unpack_header_line(const std::string & line);

    std::string _status_line;
    RequestType _request_type;
    ResourceType _resource_type;

    std::string _header;
    std::string _source;
};