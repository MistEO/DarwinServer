#include <iostream>

#include "motion.h"
#include "requestcontrol.h"
#include "resourcecontrol.h"
#include "responsemessage.h"

void path_parse(const RequestMessage& request)
{
    try {
        switch (request.request_method()) {
        case RequestMessage::GET:
            if (request.uri_path() == "/"
                || request.uri_path() == "/help") {
                request_help(request);
            } else if (request.uri_path() == "/image") {
                request_image(request);
            } else if (request.uri_path() == "/audio") {
                request_audio(request);
            } else if (request.uri_path() == "/audio_stop") {
                request_stop_audio(request);
            } else if (request.uri_path().find("/motor") == 0) {
                request_motor(request);
            } else {
                request.reply(404, "404 Not Found");
            }
            break;
        default:
            request.reply(405, "405 Method Not Allowed");
            break;
        }
    } catch (HttpException& exp) {
        std::cerr << exp.what() << std::endl;
        request.reply(400, "400 Bad Request");
    } catch (std::out_of_range& exp) {
        std::cerr << exp.what() << std::endl;
        request.reply(400, "400 Bad Request");
    } catch (std::invalid_argument& exp) {
        std::cerr << exp.what() << std::endl;
        request.reply(400, "400 Bad Request");
    }
}

void request_help(const RequestMessage& request)
{
    std::string data;
    if (resource.get_file("README.html", data)) {
        ResponseMessage response(200);
        response.header_map()["Content-Type"] = "text/html";
        response.set_data(data);
        request.reply(response);
    } else {
        request.reply(500, "500 Internal Server Error");
    }
}

void request_image(const RequestMessage& request)
{
    ResponseMessage response;
    std::string data;
    if (resource.get_image(data,
            response.header_map()["Cols"],
            response.header_map()["Rows"],
            response.header_map()["Step"])) {
        response.header_map()["Content-Type"] = "image";
        response.set_status(200);
        response.set_data(data);
    } else {
        response = ResponseMessage(500, "500 Internal Server Error");
    }
    request.reply(response);
}

void request_audio(const RequestMessage& request)
{
    const std::string file_path = RequestMessage::split_query(request.uri_query()).at("filepath");
    if (resource.play_audio(file_path)) {
        request.reply(200, "200 Ok");
    } else {
        request.reply(500, "500 Internal Server Error");
    }
}

void request_stop_audio(const RequestMessage& request)
{
    if (resource.stop_audio()) {
        request.reply(200, "200 Ok");
    } else {
        request.reply(500, "500 Internal Server Error");
    }
}

void request_motor(const RequestMessage& request)
{
    const auto arg_map = RequestMessage::split_query(request.uri_query());
    if (request.uri_path() == "/motor/walk_start") {
        motion.walk_start();
        request.reply(200, "200 Ok");
    } else if (request.uri_path() == "/motor/walk_stop") {
        motion.walk_stop();
        request.reply(200, "200 Ok");
    } else if (request.uri_path() == "/motor/walk") {
        int x, y, msec;
        if (arg_map.find("x") != arg_map.end()) {
            x = std::stoi(arg_map.at("x"));
        } else {
            x = 5;
        }
        if (arg_map.find("y") != arg_map.end()) {
            y = std::stoi(arg_map.at("y"));
        } else {
            y = 0;
        }
        if (arg_map.find("msec") != arg_map.end()) {
            msec = std::stoi(arg_map.at("msec"));
        } else {
            msec = 2000;
        }
        motion.walk(x, y, msec);
        request.reply(200, "200 Ok");
    } else if (request.uri_path() == "/motor/fall_up") {
        motion.fall_up();
        request.reply(200, "200 Ok");
    } else if (request.uri_path() == "/motor/head") {
        int x, y, home;
        if (arg_map.find("x") != arg_map.end()) {
            x = std::stoi(arg_map.at("x"));
        } else {
            x = 0;
        }
        if (arg_map.find("y") != arg_map.end()) {
            y = std::stoi(arg_map.at("y"));
        } else {
            y = 60;
        }
        if (arg_map.find("home") != arg_map.end()) {
            home = std::stoi(arg_map.at("home"));
        } else {
            home = 1;
        }
        motion.head_move(x, y, home);
        request.reply(200, "200 Ok");
    } else if (request.uri_path().find("/motor/action/") == 0) {
        int index = std::stoi(request.uri_path().substr(std::string("/motor/action/").size(), request.uri_path().size()));
        std::string mp3;
        if (arg_map.find("audio") != arg_map.end()) {
            mp3 = arg_map.at("audio");
        }
        motion.action(index, mp3);
        request.reply(200, "200 Ok");
    } else {
        request.reply(404, "404 Not Found");
    }
}