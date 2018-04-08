#include "requestmessage.h"

#include <algorithm>
#include <iostream>
#include <vector>

RequestMessage::RequestMessage(const std::string& source_message)
{
    _source = source_message;
    _unpack(source_message);
}

void RequestMessage::_unpack(const std::string& message)
{
    std::string source(message);
    //转换为小写
    // std::transform(source.begin(), source.end(), source.begin(), ::tolower);
    //按行分割
    std::vector<std::string> lines(_split_string(source, "\n"));

    if (lines.size() < 2) {
        std::cerr << "Request message segmentation error:\n"
                  << message << std::endl;
        return;
    }
    auto iter = lines.begin();

    _status_line = *iter;
    _unpack_request_line(*iter);

    _header.clear();
    header_map.clear();
    for (++iter; iter != lines.end() && !iter->empty(); ++iter) {
        _header += *iter + "\n";
        _unpack_header_line(*iter);
    }

    data.clear();
    for (++iter; iter != lines.end(); ++iter) {
        if (!data.empty()) {
            data += "\n";
        }
        data += *iter;
    }
}

void RequestMessage::_unpack_request_line(const std::string& line)
{
    //按空格分割
    std::vector<std::string> request_line(_split_string(line, " "));

    //处理请求类型
    if (request_line.empty()) {
        _request_type = RequestType::Unknown;
        return;
    }
    if (request_line.front() == "Get") {
        _request_type = RequestType::Get;
    } else if (request_line.front() == "Head") {
        _request_type = RequestType::Head;
    } else if (request_line.front() == "Post") {
        _request_type = RequestType::Post;
    } else {
        _request_type = RequestType::Unknown;
    }

    //处理资源类型
    if (request_line.size() < 2) {
        _resource_type = ResourceType::UnknownResource;
        return;
    }
    if (request_line[1] == "Image") {
        _resource_type = ResourceType::Image;
    } else if (request_line[1] == "Camera") {
        _resource_type = ResourceType::Camera;
    } else if (request_line[1] == "Audio") {
        _resource_type = ResourceType::Audio;
    } else if (request_line[1] == "StopAudio") {
        _resource_type = ResourceType::StopAudio;
    } else if (request_line[1] == "Motor") {
        _resource_type = ResourceType::Motor;
    } else if (request_line[1] == "StopMotor") {
        _resource_type = ResourceType::StopMotor;
    } else {
        _resource_type = ResourceType::UnknownResource;
    }

    if (request_line.size() < 3) {
        version = "";
    } else {
        version = request_line[2];
    }
}

void RequestMessage::_unpack_header_line(const std::string& line)
{
    size_t seg_index = line.find(":");
    if (seg_index == std::string::npos) {
        std::cerr << "Request header segmentation error: " << line << std::endl;
        return;
    }
    std::string key = line.substr(0, seg_index);
    std::string value = line.substr(seg_index + 1, line.length() - seg_index);

    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    // std::cout << "key:" << pair[0] << " value:" << pair[1] << std::endl;
    header_map[key] = value;
}

std::string RequestMessage::first_line() const
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

std::string RequestMessage::header() const
{
    return _header;
}

std::string RequestMessage::to_string() const
{
    return _source;
}

const std::map<std::string, std::string>& RequestMessage::get_header_map() const
{
    return header_map;
}
const std::string& RequestMessage::get_version() const
{
    return version;
}
const std::string& RequestMessage::get_data() const
{
    return data;
}
