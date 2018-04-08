#include "resourcecontrol.h"

#include <algorithm>
#include <fcntl.h>
#include <iterator>
#include <unistd.h>

cv::VideoCapture ResourceControl::capture(-1);
const std::string ResourceControl::PlayApp("mplayer");

int ResourceControl::get_image(std::string& data, int& cols, int& rows, int& step)
{
    if (!capture.isOpened()) {
        return 404;
    }
    cv::Mat frame;
    for (int i = 0; i != 5; ++i) {
        capture >> frame;
    }
    if (frame.empty()) {
        return 403;
    }
    cv::Mat rgb_frame;
    cv::cvtColor(frame, rgb_frame, CV_BGR2RGB);
    // rgb_frame = rgb_frame.reshape(0,1); // to make it continuous
    data = std::string((char*)rgb_frame.data, frame.total() * frame.elemSize());

    cols = rgb_frame.cols;
    rows = rgb_frame.rows;
    step = rgb_frame.step1();
    return 200;
}

int ResourceControl::get_image(std::string& data, std::string& cols, std::string& rows, std::string& step)
{
    int icols, irows, istep;
    int return_code = get_image(data, icols, irows, istep);
    cols = std::to_string(icols);
    rows = std::to_string(irows);
    step = std::to_string(istep);
    return return_code;
}

int ResourceControl::get_stop_audio()
{
    const std::string kill_cmd = "killall -9 " + PlayApp;
    std::cout << kill_cmd << std::endl;
    if (system(kill_cmd.c_str())) {
        perror(kill_cmd.c_str());
        return 500;
    }
    return 200;
}

int ResourceControl::play_audio(const std::string& file_path)
{
    if (file_path.empty() || access(file_path.c_str(), F_OK | W_OK)) {
        return 404;
    }
    const std::string hide_output_cmd = " > /dev/null 2>&1";
    // int format_pos = file_path.find_last_of(".");
    // std::string path_without_format = file_path.substr(0, format_pos);
    // std::string format = file_path.substr(format_pos, file_path.length() - format_pos);
    // if (format == ".m4a")
    // {
    //     // commond play not support m4a file
    //     remove((path_without_format + ".mp3").c_str());
    //     std::string cvt_format_cmd = "avconv -i " + file_path + " " + path_without_format + ".mp3" + hide_output_cmd;
    //     std::cout << cvt_format_cmd << std::endl;
    //     // system("avconv -i net_audio.m4a net_audio.mp3 > /dev/null 2>&1");
    //     if (system(cvt_format_cmd.c_str()))
    //     {
    //         remove((path_without_format + ".mp3").c_str());
    //         perror(cvt_format_cmd.c_str());
    //         return 500;
    //     }
    //     format = ".mp3";
    // }
    std::string play_audio_cmd = PlayApp + " " + file_path + hide_output_cmd + " &";
    std::cout << play_audio_cmd << std::endl;
    // system("play net_audio.mp3 > /dev/null 2>&1");
    if (system(play_audio_cmd.c_str())) {
        perror(play_audio_cmd.c_str());
        return 500;
    }
    return 200;
}