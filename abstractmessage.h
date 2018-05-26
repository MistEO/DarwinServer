#pragma once

#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class HttpException : public std::exception {
public:
    explicit HttpException(const std::string& message);
    const char* what() const noexcept;

private:
    std::string _message;
};

class AbstractMessage {
public:
    virtual std::string first_line() const = 0;
    virtual std::string header() const = 0;
    virtual std::string to_string() const = 0;

    friend std::ostream& operator<<(std::ostream& out, const AbstractMessage& amsg);

protected:
    static std::vector<std::string> _split_string(
        const std::string& source, const std::string& c);

    std::map<std::string, std::string> _header_map;
    std::string _version = "HTTP/1.1";
    std::string _data;
};