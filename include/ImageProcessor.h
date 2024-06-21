// #ifndef IMAGEPROCESSOR_H
// #define IMAGEPROCESSOR_H

// #include <opencv2/opencv.hpp>

// class ImageProcessor {
// public:
//     void processFrame(cv::Mat &frame);
//     std::vector<cv::Mat> extractFrames(const std::string &videoPath);
// };

// #endif // IMAGEPROCESSOR_H


#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class ImageProcessor {
public:
    void processFrame(cv::Mat &frame);
    std::vector<cv::Mat> extractFrames(const std::string &videoPath);
    std::vector<cv::Rect> loadBoundingBoxes(const std::string &bboxFilePath);
};

#endif // IMAGEPROCESSOR_H