#pragma once

#include <string>
#include <vector>

class ProtocolParser
{
public:
    enum RequestType {
        Unknown,
        Get,
        Head,
        Post
    };
    enum ResourceType {
        Image,
        Camera,
        Motor
    };

    ProtocolParser() = default;
    ~ProtocolParser() = default;
    
    static RequestType unpack(
        const char * message, ResourceType & resource_type);
    static std::string pack_status_line(
        int status, const std::string & status_name, const std::string & version = std::string("HTTP/1.1"));
private:
    static std::vector<std::string> _split_string(
        const std::string & source, const std::string & c);

    static RequestType _unpack_request_line(
        const std::string & line, ResourceType & resource_type);
};