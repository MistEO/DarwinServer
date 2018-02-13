#include "resourcecontrol.h"

#include <iterator>
#include <algorithm>

cv::VideoCapture ResourceControl::capture(0);

int ResourceControl::get_image(std::string &data, int &cols, int &rows, int &step)
{
    if (!capture.isOpened())
    {
        return 404;
    }
    cv::Mat frame;
    for (int i = 0; i != 5; ++i)
    {
        capture >> frame;
    }
    if (frame.empty())
    {
        return 403;
    }
    cv::Mat rgb_frame;
    cv::cvtColor(frame, rgb_frame, CV_BGR2RGB);
    // rgb_frame = rgb_frame.reshape(0,1); // to make it continuous

    cols = rgb_frame.cols;
    rows = rgb_frame.rows;
    step = rgb_frame.step1();
    return 200;
}

int ResourceControl::get_image(std::string &data, std::string &cols, std::string &rows, std::string &step)
{
    int icols, irows, istep;
    int return_code = get_image(data, icols, irows, istep);
    cols = std::to_string(icols);
    rows = std::to_string(irows);
    step = std::to_string(istep);
    return return_code;
}