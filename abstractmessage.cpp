#include "abstractmessage.h"

#include <algorithm>

HttpException::HttpException(const std::string& message)
    : std::exception()
    , _message(message)
{
}

const char* HttpException::what() const noexcept
{
    return _message.c_str();
}

std::vector<std::string> AbstractMessage::_split_string(const std::string& source, const std::string& c)
{
    std::string::size_type pos1 = 0, pos2 = source.find(c);
    std::vector<std::string> result;
    while (std::string::npos != pos2) {
        result.push_back(source.substr(pos1, pos2 - pos1));
        pos1 = pos2 + c.size();
        pos2 = source.find(c, pos1);
    }
    if (pos1 != source.length())
        result.push_back(source.substr(pos1));
    return result;
}

std::ostream& operator<<(std::ostream& out, const AbstractMessage& amsg)
{
    std::string data_string;
    if (amsg._header_map.find("Content-Type") != amsg._header_map.end()
        && amsg._header_map.at("Content-Type") == "text") {
        data_string = amsg._data;
    } else if (amsg._data.empty()) {
        ;
    } else {
        data_string = "std::string data, size:" + std::to_string(amsg._data.size());
    }
    out << amsg.first_line()
        << amsg.header()
        << "\r\n"
        << data_string;
    return out;
}