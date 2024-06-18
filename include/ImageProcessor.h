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
#include "MLModel.h"

class ImageProcessor {
public:
    ImageProcessor();
    void processFrame(const cv::Mat &frame, cv::Mat &outputFrame);
    void detectBalls(const cv::Mat &frame, std::vector<cv::Rect> &detectedBalls);
    void setMLModel(MLModel* mlModel);

private:
    cv::Ptr<cv::SimpleBlobDetector> blobDetector;
    MLModel* mlModel;
};

#endif // IMAGEPROCESSOR_H
