#include "requestmessage.h"

#include <iostream>
#include <vector>
#include <algorithm>

RequestMessage::RequestMessage(const std::string & source_message)
{
    _unpack(source_message);
}

void RequestMessage::_unpack(const std::string & message)
{
    std::string source(message);
    //转换为小写
    std::transform(source.begin(), source.end(), source.begin(), ::tolower);
    //按行分割
    std::vector<std::string> lines(_split_string(source, "\n"));

    if (lines.empty()) {
        std::cerr << "Request message segmentation error:\n" << message << std::endl;
        return;
    }
    _status_line = lines.front();
    _unpack_request_line(_status_line);
    for (auto i = lines.begin() + 1; i != lines.end() && !i->empty(); ++i) {
        _unpack_header_line(*i);
    }
}

void RequestMessage::_unpack_request_line(const std::string & line)
{
    //按空格分割
    std::vector<std::string> request_line(_split_string(line, " "));

    //处理请求类型
    if (request_line.empty()) {
        _request_type = RequestType::Unknown;
        return;
    }
    if (request_line.front() == "get") {
        _request_type = RequestType::Get;
    }
    else if (request_line.front() == "head") {
        _request_type = RequestType::Head;
    }
    else if (request_line.front() == "post") {
        _request_type = RequestType::Post;
    }
    else {
        _request_type = RequestType::Unknown;
    }

    //处理资源类型
    if (request_line.size() < 2) {
        _resource_type = ResourceType::UnknownResource;
        return;
    }
    if (request_line[1] == "image") {
        _resource_type = ResourceType::Image;
    }
    else if (request_line[1] == "camera") {
        _resource_type = ResourceType::Camera;
    }
    else if (request_line[1] == "motor") {
        _resource_type = ResourceType::Motor;
    }
    else {
        _resource_type = ResourceType::UnknownResource;
    }

    if (request_line.size() < 3) {
        version = "";
    }
    else {
        version = request_line[2];
    }
}

void RequestMessage::_unpack_header_line(const std::string & line)
{
    auto pair = _split_string(line, ":");
    if (pair.size() != 2) {
        std::cerr << "Request header segmentation error: " << line << std::endl;
        return;
    }
    header_map[pair[0]] = pair[1];
}

std::string RequestMessage::status_line() const
{
    return _status_line;
}

RequestMessage::RequestType RequestMessage::request_type() const
{
    return _request_type;
}

RequestMessage::ResourceType RequestMessage::resource_type() const
{
    return _resource_type;
}
