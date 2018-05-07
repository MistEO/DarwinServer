#include <iostream>

#include "requestcontrol.h"
#include "resourcecontrol.h"
#include "responsemessage.h"
#ifdef DARWIN
#include "motion.h"
#endif

void path_parse(const RequestMessage& request)
{
    try {
        switch (request.request_method()) {
        case RequestMessage::GET:
            if (request.uri_path() == "/image") {
                request_image(request);
            } else if (request.uri_path() == "/stop_audio") {
                request_stop_audio(request);
            } else if (request.uri_path() == "/audio") {
                request_audio(request);
            } else if (request.uri_path().find("/motor") == 0) {
                request_motor(request);
            } else {
                request.reply(404);
            }
            break;
        default:
            request.reply(400);
            break;
        }
    } catch (HttpException& exp) {
        std::cerr << exp.what() << std::endl;
        request.reply(400);
    } catch (std::out_of_range& exp) {
        std::cerr << exp.what() << std::endl;
        request.reply(400);
    } catch (std::invalid_argument& exp) {
        std::cerr << exp.what() << std::endl;
        request.reply(400);
    }
}

void request_image(const RequestMessage& request)
{
    ResponseMessage response;
    response.header_map()["Content-Type"] = "image";
    std::string data;
    response.set_status(
        resource.get_image(
            data,
            response.header_map()["Cols"],
            response.header_map()["Rows"],
            response.header_map()["Step"]));
    response.set_data(data);
    request.reply(response);
}

void request_stop_audio(const RequestMessage& request)
{
    ResponseMessage response(resource.stop_audio());
    request.reply(response);
}

void request_audio(const RequestMessage& request)
{
    ResponseMessage response;
    const std::string file_path = RequestMessage::split_query(request.uri_query()).at("filepath");
    response.set_status(resource.play_audio(file_path));
    request.reply(response);
}

void request_motor(const RequestMessage& request)
{
    ResponseMessage response;
    const auto arg_map = RequestMessage::split_query(request.uri_query());
    if (request.uri_path() == "/motor/walk_start") {
#ifdef DARWIN
        motion.walk_start();
#else
        std::cout << "walk start" << std::endl;
#endif
        response.set_status(200);
    } else if (request.uri_path() == "/motor/walk_stop") {
#ifdef DARWIN
        motion.walk_stop();
#else
        std::cout << "walk stop" << std::endl;
#endif
        response.set_status(200);
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
#ifdef DARWIN
        motion.walk(x, y, msec);
#else
        std::cout << "walk, x=" << x << " y=" << y << " msec=" << msec << std::endl;
#endif
        response.set_status(200);
    } else if (request.uri_path() == "/motor/fall_up") {
#ifdef DARWIN
        motion.fall_up();
#else
        std::cout << "fall up" << std::endl;
#endif
        response.set_status(200);
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
#ifdef DARWIN
        motion.head_move(x, y, home);
#else
        std::cout << "head, x=" << x << " y=" << y << " home=" << home << std::endl;
#endif
        response.set_status(200);
    } else if (request.uri_path().find("/motor/action/") == 0) {
        int index = std::stoi(request.uri_path().substr(std::string("/motor/action/").size(), request.uri_path().size()));
        std::string mp3;
        if (arg_map.find("audio") != arg_map.end()) {
            mp3 = arg_map.at("audio");
        }
#ifdef DARWIN
        motion.action(index, mp3);
#else
        std::cout << "action:" << index << " , audio:" << mp3 << std::endl;
#endif
        response.set_status(200);
    } else {
        response.set_status(400);
    }
    request.reply(response);
}