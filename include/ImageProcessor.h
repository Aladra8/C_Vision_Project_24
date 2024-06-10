#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <opencv2/opencv.hpp>

class ImageProcessor {
public:
    void processFrame(cv::Mat &frame);
};

#endif // IMAGEPROCESSOR_H
