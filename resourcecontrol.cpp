#include "resourcecontrol.h"

#include <algorithm>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <streambuf>
#include <string>
#include <unistd.h>

//cv::VideoCapture ResourceControl::capture(-1);
const std::string ResourceControl::PlayApp("mplayer");

ResourceControl& ResourceControl::ins()
{
    static ResourceControl instance;
    if (!instance.capture.isOpened()) {
        instance.capture.open(-1);
    }
    return instance;
}

ResourceControl::~ResourceControl()
{
    capture.release();
}

bool ResourceControl::get_image(std::string& data, int& cols, int& rows, int& step)
{
    if (!capture.isOpened()) {
        return false;
    }
    cv::Mat frame;
    capture_mutex.lock();
    capture >> frame;
    capture_mutex.unlock();
    if (frame.empty()) {
        return false;
    }
    cv::Mat rgb_frame;
    cv::cvtColor(frame, rgb_frame, CV_BGR2RGB);
    // rgb_frame = rgb_frame.reshape(0,1); // to make it continuous
    data = std::string((char*)rgb_frame.data, frame.total() * frame.elemSize());

    cols = rgb_frame.cols;
    rows = rgb_frame.rows;
    step = rgb_frame.step1();
    return true;
}

bool ResourceControl::get_image(std::string& data, std::string& cols, std::string& rows, std::string& step)
{
    int icols, irows, istep;
    bool ret = get_image(data, icols, irows, istep);
    cols = std::to_string(icols);
    rows = std::to_string(irows);
    step = std::to_string(istep);
    return ret;
}

bool ResourceControl::stop_audio()
{
    const std::string kill_cmd = "killall -9 " + PlayApp;
    std::cout << kill_cmd << std::endl;
    if (system(kill_cmd.c_str())) {
        perror(kill_cmd.c_str());
        return false;
    }
    return true;
}

bool ResourceControl::play_audio(const std::string& file_path)
{
    if (file_path.empty() || access(file_path.c_str(), F_OK | R_OK)) {
        return false;
    }
    const std::string hide_output_cmd = " > /dev/null 2>&1";
    std::string dst_filepath = file_path;
    auto replaceAll = [](std::string& str,
                          const std::string& oldStr,
                          const std::string& newStr) {
        std::string::size_type pos = 0u;
        while ((pos = str.find(oldStr, pos)) != std::string::npos) {
            str.replace(pos, oldStr.length(), newStr);
            pos += newStr.length();
        }
    };
    replaceAll(dst_filepath, " ", "\\ ");
    std::string play_audio_cmd = PlayApp + " " + dst_filepath + hide_output_cmd + " &";
    std::cout << play_audio_cmd << std::endl;
    if (system(play_audio_cmd.c_str())) {
        perror(play_audio_cmd.c_str());
        return false;
    }
    return true;
}

bool ResourceControl::get_file(const std::string& file_path, std::string& data)
{
    if (file_path.empty() || access(file_path.c_str(), F_OK | R_OK)) {
        return false;
    }
    std::ifstream file(file_path);
    std::string str((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    data = str;
    return true;
}

bool ResourceControl::get_mic(int time, std::string& filename)
{
    std::lock_guard<std::mutex> locker(mic_mutex);
    std::string record_cmd = "arecord /tmp/mic.wav -r 16000 -d " + std::to_string(time);
    system(record_cmd.c_str());
    filename = "/tmp/mic.wav";
    if (access(filename.c_str(), F_OK | R_OK)) {
        return false;
    }
    return true;
}

bool ResourceControl::write_file(const std::string& filename, const std::string& data)
{
    std::ofstream file;
    file.open(filename, std::ios::out | std::ios::binary);
    file.write(data.c_str(), data.size());
    file.close();
    return true;
}