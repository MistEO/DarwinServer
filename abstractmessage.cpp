#include "abstractmessage.h"

#include <iostream>
#include <algorithm>

std::string AbstractMessage::header() const
{
    std::string header_string;
    for (auto pair : header_map) {
        header_string += pair.first + ":" + pair.second + "\n";
    }
    return header_string;
}

std::vector<std::string> AbstractMessage::_split_string(const std::string& source, const std::string& c)
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
