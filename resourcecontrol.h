#pragma once

#include <vector>

#include <opencv2/opencv.hpp>

class ResourceControl
{
public:
    ResourceControl() = delete;
    /// 通过参数传值，返回值为错误码
    static int get_image(std::vector<uchar> & data, int & cols, int & rows, int & step);
private:
    static cv::VideoCapture capture;
};