#include "protocolparser.h"

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

ProtocolParser::RequestType ProtocolParser::unpack(const char * message, ProtocolParser::ResourceType & resource_type)
{
    std::string source(message);
    //转换为小写
    std::transform(source.begin(), source.end(), source.begin(), ::tolower);
    //按行分割
    std::vector<std::string> lines(_split_string(source, "\n"));
    ProtocolParser::RequestType request_type = _unpack_request_line(lines.front(), resource_type);
    return request_type;
}

ProtocolParser::RequestType ProtocolParser::_unpack_request_line(const std::string & line, ProtocolParser::ResourceType & resource_type)
{
    //按空格分割
    std::vector<std::string> request_line(_split_string(line, " "));
    if (request_line.size() != 3) {
        return RequestType::Unknown;
    }
    
    RequestType request_type = RequestType::Unknown;
    if (request_line.front() == "get") {
        request_type = RequestType::Get;
    }
    else if (request_line.front() == "head") {
        request_type = RequestType::Head;
    }
    else if (request_line.front() == "post") {
        request_type = RequestType::Post;
    }
    else {
        request_type = RequestType::Unknown;
    }
    return request_type;
}

std::vector<std::string> ProtocolParser::_split_string(const std::string& source, const std::string& c)
{
    std::string::size_type pos1 = 0, pos2 = source.find(c);
    std::vector<std::string> result;
    while (std::string::npos != pos2)
    {
        result.push_back(source.substr(pos1, pos2-pos1));
        pos1 = pos2 + c.size();
        pos2 = source.find(c, pos1);
    }
    if (pos1 != source.length())
        result.push_back(source.substr(pos1));
    return result;
}

std::string ProtocolParser::pack_status_line(int status, const std::string & status_name, const std::string & version)
{
    return version + " " + std::to_string(status) + " " + status_name + "\n";
}