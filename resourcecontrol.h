#pragma once

#include <string>
#include <cstddef>

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/core.hpp>

class ResourceControl {
public:
    static const std::string PlayApp;
    ResourceControl() = delete;
    /// 通过参数传值，返回值为错误码
    static int get_image(std::string& data, int& cols, int& rows, int& step);
    static int get_image(std::string& data, std::string& cols, std::string& rows, std::string& step);
    static int stop_audio();
    static int play_audio(const std::string& file_path);

private:
    static cv::VideoCapture capture;
};
