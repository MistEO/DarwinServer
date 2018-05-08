#pragma once

#include "abstractmessage.h"
#include "responsemessage.h"

class RequestMessage : private AbstractMessage {
public:
    enum RequestMethod {
        GET,
        Unknown
    };

    RequestMessage(int connfd, const std::string& source_message);
    std::string first_line() const;
    RequestMethod request_method() const;
    std::string uri() const;
    std::string uri_path() const;
    std::string uri_query() const;
    std::string uri_fragment() const;
    static std::map<std::string, std::string> split_query(const std::string& query);

    std::string header() const;
    std::string to_string() const;

    const std::map<std::string, std::string>& get_header_map() const;
    const std::string& get_version() const;
    const std::string& get_data() const;
    const int& get_connfd() const;

    int reply(const ResponseMessage& response) const;
    int reply(int status, const std::string& text = std::string()) const;

private:
    void _unpack(const std::string& message);
    void _unpack_request_line(const std::string& line);
    void _unpack_header_line(const std::string& line);

    int _connfd = -1;

    std::string _request_line;
    RequestMethod _method;
    std::string _uri;
    std::string _uri_path;
    std::string _uri_query;
    std::string _uri_fragment;

    std::string _header;
    std::string _source;
};