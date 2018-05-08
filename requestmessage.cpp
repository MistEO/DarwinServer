#include "requestmessage.h"

#include <algorithm>
#include <iostream>
#include <vector>

RequestMessage::RequestMessage(int connfd, const std::string& source_message)
    : _connfd(connfd)
    , _source(source_message)
{
    _unpack(_source);
}

void RequestMessage::_unpack(const std::string& message)
{
    std::string source(message);
    // 按行分割
    std::vector<std::string> lines(_split_string(source, "\r\n"));
    if (lines.size() < 2) {
        throw HttpException("the message is invaild: " + source);
    }

    auto iter = lines.begin();

    _request_line = *iter;
    _unpack_request_line(*iter);

    _header.clear();
    _header_map.clear();
    for (++iter; !iter->empty(); ++iter) {
        _header += *iter + "\r\n";
        _unpack_header_line(*iter);
    }
}

void RequestMessage::_unpack_request_line(const std::string& line)
{
    // 按空格分割
    const std::vector<std::string> request_line(_split_string(line, " "));
    if (request_line.size() != 3) {
        throw HttpException("the request line is invaild: " + line);
    }

    // 处理请求类型
    if (request_line.at(0) == "GET") {
        _method = RequestMethod::GET;
    } else {
        throw HttpException("the request method is invaild: " + request_line.at(0));
    }

    // 处理URI
    _uri = request_line.at(1);
    auto uri_splited = _split_string(request_line.at(1), "?");
    if (uri_splited.size() == 2) {
        _uri_path = uri_splited.at(0);
        auto after_path_splited = _split_string(uri_splited.at(1), "#");
        if (after_path_splited.size() == 2) {
            _uri_query = after_path_splited.at(0);
            _uri_fragment = after_path_splited.at(1);
        } else {
            _uri_query = uri_splited.at(1);
            _uri_fragment.clear();
        }
    } else {
        _uri_path = _uri;
        _uri_query.clear();
        _uri_fragment.clear();
    }

    // 处理http版本号
    _version = request_line.at(2);
}

void RequestMessage::_unpack_header_line(const std::string& line)
{
    size_t seg_index = line.find(": ");
    if (seg_index == std::string::npos) {
        throw HttpException("the request header segmentation is invalid: " + line);
    }
    std::string key = line.substr(0, seg_index);
    std::string value = line.substr(seg_index + 2, line.length() - seg_index);

    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    // std::cout << "key:" << pair[0] << " value:" << pair[1] << std::endl;
    _header_map[key] = value;
}

std::string RequestMessage::first_line() const
{
    return _request_line;
}

RequestMessage::RequestMethod RequestMessage::request_method() const
{
    return _method;
}

std::string RequestMessage::uri() const
{
    return _uri;
}

std::string RequestMessage::uri_path() const
{
    return _uri_path;
}

std::string RequestMessage::uri_query() const
{
    return _uri_query;
}

std::string RequestMessage::uri_fragment() const
{
    return _uri_fragment;
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
    return _header_map;
}

const std::string& RequestMessage::get_version() const
{
    return _version;
}

const std::string& RequestMessage::get_data() const
{
    return _data;
}

const int& RequestMessage::get_connfd() const
{
    return _connfd;
}

int RequestMessage::reply(const ResponseMessage& response) const
{
    return response.send_to(_connfd);
}

int RequestMessage::reply(int status, const std::string& text) const
{
    ResponseMessage response(status, text);
    return reply(response);
}

std::map<std::string, std::string> RequestMessage::split_query(const std::string& query)
{
    std::map<std::string, std::string> res;
    auto key_value_vector = _split_string(query, "&");
    for (auto key_value_string : key_value_vector) {
        auto key_value = _split_string(key_value_string, "=");
        if (key_value.size() == 2) {
            res[key_value.at(0)] = key_value.at(1);
        } else {
            throw HttpException("the query is invaild: " + query);
        }
    }
    return res;
}
