#include "ImageProcessor.h"

void ImageProcessor::processFrame(cv::Mat &frame) {
    cv::Mat gray, blurred, edges;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
    cv::Canny(blurred, edges, 50, 150);
    cv::imshow("Edges", edges);
}
