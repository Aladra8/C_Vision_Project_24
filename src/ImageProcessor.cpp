// #include "ImageProcessor.h"
// #include <iostream>

// void ImageProcessor::processFrame(cv::Mat &frame) {
//     cv::Mat gray, blurred, edges;
//     cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
//     cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
//     cv::Canny(blurred, edges, 50, 150);
//     cv::imshow("Edges", edges);
// }

// std::vector<cv::Mat> ImageProcessor::extractFrames(const std::string &videoPath) {
//     cv::VideoCapture cap(videoPath);
//     if (!cap.isOpened()) {
//         std::cerr << "Error: Could not open video." << std::endl;
//         return {};
//     }

//     std::vector<cv::Mat> frames;
//     cv::Mat frame;
//     while (cap.read(frame)) {
//         frames.push_back(frame.clone());
//     }
//     return frames;
// }

#include "ImageProcessor.h"

ImageProcessor::ImageProcessor()
{
    cv::SimpleBlobDetector::Params params;
    params.filterByColor = true;
    params.blobColor = 255;
    blobDetector = cv::SimpleBlobDetector::create(params);
    mlModel = nullptr;
}

void ImageProcessor::setMLModel(MLModel *model)
{
    mlModel = model;
}

void ImageProcessor::processFrame(const cv::Mat &frame, cv::Mat &outputFrame)
{
    cv::cvtColor(frame, outputFrame, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(outputFrame, outputFrame, cv::Size(5, 5), 0);
    cv::Canny(outputFrame, outputFrame, 50, 150);
}

void ImageProcessor::detectBalls(const cv::Mat &frame, std::vector<cv::Rect> &detectedBalls)
{
    if (mlModel)
    {
        detectedBalls = mlModel->detectObjects(frame);
    }
    else
    {
        std::vector<cv::KeyPoint> keypoints;
        blobDetector->detect(frame, keypoints);
        for (const auto &kp : keypoints)
        {
            detectedBalls.push_back(cv::Rect(cv::Point(kp.pt.x - kp.size / 2, kp.pt.y - kp.size / 2), cv::Size(kp.size, kp.size)));
        }
    }
}
