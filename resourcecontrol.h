#pragma once

#include <cstddef>
#include <mutex>
#include <string>

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

class ResourceControl {
public:
    static const std::string PlayApp;
    static ResourceControl& ins();
    ~ResourceControl();
    /// 通过参数传值
    bool get_image(std::string& data, int& cols, int& rows, int& step);
    bool get_image(std::string& data, std::string& cols, std::string& rows, std::string& step);
    bool stop_audio();
    bool play_audio(const std::string& file_path);

private:
    ResourceControl() = default;
    cv::VideoCapture capture;
    std::mutex capture_mutex;
};

#define resource ResourceControl::ins()
