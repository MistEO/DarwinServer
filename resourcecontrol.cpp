#include "resourcecontrol.h"

#include <iterator>

cv::VideoCapture ResourceControl::capture(0);

int ResourceControl::get_image(std::vector<uchar> & data, int & cols, int & rows, int & step)
{
    if (!capture.isOpened()) {
        return 404;
    }
    cv::Mat frame;
    capture >> frame;
    if (frame.empty()) {
        return 403;
    }
    data = std::vector<uchar>(frame.data, frame.data + sizeof(frame.data)/sizeof(uchar));
    cols = frame.cols;
    rows = frame.rows;
    step = frame.step;
    return 200;
}