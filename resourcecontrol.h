#pragma once

#include <cstddef>
#include <mutex>
#include <string>

#include <raspicam/raspicam_cv.h>

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

private:
    ResourceControl() = default;
    raspicam::RaspiCam_Cv camera;
    // cv::VideoCapture capture;
    std::mutex capture_mutex;
};

#define resource ResourceControl::ins()
