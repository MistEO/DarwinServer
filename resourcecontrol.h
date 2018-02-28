#pragma once

#include <string>

#include <opencv2/opencv.hpp>

class ResourceControl {
public:
    static const std::string PlayApp;
    ResourceControl() = delete;
    /// 通过参数传值，返回值为错误码
    static int get_image(std::string& data, int& cols, int& rows, int& step);
    static int get_image(std::string& data, std::string& cols, std::string& rows, std::string& step);
    static int get_stop_audio();
    static int play_audio(const std::string& file_path);

private:
    static cv::VideoCapture capture;
};