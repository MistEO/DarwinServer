#pragma once

#include "abstractmessage.h"

class ResponseMessage : public AbstractMessage {
public:
    ResponseMessage() = default;
    ResponseMessage(int status, const std::string& data = std::string());
    std::string first_line() const;
    int status() const;
    void set_status(const int status);
    std::string status_name() const;

    std::string header() const;
    std::string to_string() const;

    std::map<std::string, std::string>& header_map();
    const std::string& version();
    const std::string& data();
    void set_data(const std::string& data);

    int send_to(int connfd) const;

private:
    int _status;
    std::string _status_name;
};