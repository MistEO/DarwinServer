#include "resourcecontrol.h"

#include <iterator>
#include <algorithm>

cv::VideoCapture ResourceControl::capture(0);

int ResourceControl::get_image(std::string & data, int & cols, int & rows, int & step)
{
    if (!capture.isOpened()) {
        return 404;
    }
    cv::Mat frame;
    capture >> frame;
    if (frame.empty()) {
        return 403;
    }
    data = std::string(frame.data, frame.data + sizeof(frame.data)/sizeof(uchar));
    cols = frame.cols;
    rows = frame.rows;
    step = frame.step;
    return 200;
}


int ResourceControl::get_image(std::string & data, std::string & cols, std::string & rows, std::string & step)
{
    int icols, irows, istep;
    int return_code = get_image(data, icols, irows, istep);
    cols = std::to_string(icols);
    rows = std::to_string(irows);
    step = std::to_string(istep);
    return return_code;
}