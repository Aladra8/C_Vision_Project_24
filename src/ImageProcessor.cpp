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
#include <iostream>
#include <fstream>
#include <sstream>

void ImageProcessor::processFrame(cv::Mat &frame) {
    cv::Mat gray, blurred, edges;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
    cv::Canny(blurred, edges, 50, 150);
    cv::imshow("Edges", edges);
}

std::vector<cv::Mat> ImageProcessor::extractFrames(const std::string &videoPath) {
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video." << std::endl;
        return {};
    }

    std::vector<cv::Mat> frames;
    cv::Mat frame;
    while (cap.read(frame)) {
        frames.push_back(frame.clone());
    }
    return frames;
}

std::vector<cv::Rect> ImageProcessor::loadBoundingBoxes(const std::string &bboxFilePath) {
    std::vector<cv::Rect> bboxes;
    std::ifstream file(bboxFilePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open bounding box file." << std::endl;
        return bboxes;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int x, y, w, h, label;
        if (!(iss >> x >> y >> w >> h >> label)) {
            break;
        }
        bboxes.emplace_back(x, y, w, h);
    }
    return bboxes;
}
