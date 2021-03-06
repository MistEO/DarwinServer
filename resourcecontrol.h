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
    bool get_file(const std::string& file_path, std::string& data);
    bool get_mic(int time, std::string& data);
    bool write_file(const std::string& filename, const std::string& data);

private:
    ResourceControl()
        = default;
    cv::VideoCapture capture;
    std::mutex capture_mutex;
    std::mutex mic_mutex;
};

#define resource ResourceControl::ins()
