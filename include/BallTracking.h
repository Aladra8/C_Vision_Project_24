#ifndef BALL_TRACKING_H
#define BALL_TRACKING_H

#include <opencv2/opencv.hpp>
#include <unordered_map>

// Structure to hold information about each ball
struct BallInfo {
    cv::Point2f center;
    int radius;
    cv::Scalar initialColor;
    cv::Scalar color;
};

// Global variable to store tracked balls
extern std::unordered_map<int, BallInfo> balls;

// Function to determine the predominant color within a circle
cv::Scalar determineColor(const cv::Mat& frame, const cv::Vec3f& circle);

// Function to detect balls in a frame
std::unordered_map<int, BallInfo> detectBalls(cv::Mat& frame, const std::vector<cv::Point2f>& corners, int& ballId);

// Function to draw detected balls on a frame
void drawBalls(cv::Mat& frame, const std::unordered_map<int, BallInfo>& balls);

// Function to detect and draw balls in a video

std::unordered_map<int, BallInfo> detectAndDrawBalls(const std::string& videoPath, cv::Mat& frame, cv::Mat& result);

#endif // BALL_TRACKING_H