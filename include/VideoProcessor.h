#ifndef VIDEO_PROCESSOR_H
#define VIDEO_PROCESSOR_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "BallTracking.h"
#include <vector>
#include <iostream>

class VideoProcessor {
public:
    
    void processVideo(const std::string& videoPath);
    std::vector<cv::Point2f> findCorners(const std::string& videoPath);
    std::vector<cv::Point2f> sortCorners(const std::vector<cv::Point2f>& corners);
    
private:

    void drawBorders(cv::Mat& frame, const std::vector<cv::Point2f>& corners);
    void segmentField(cv::Mat &frame, const std::vector<cv::Point2f> &corners);
    void drawMinimap(cv::Mat& frame, const std::vector<cv::Point2f>& corners);
    void drawSphere(cv::Mat& frame, const std::unordered_map<int, BallInfo>& detectedBalls);
    
    std::vector<cv::Point2f> dstCorners;
    cv::Mat perspectiveTransform;
};

#endif // VIDEO_PROCESSOR_H