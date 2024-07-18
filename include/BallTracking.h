#ifndef BALLTRACKING_H
#define BALLTRACKING_H

#include <opencv2/opencv.hpp>
#include <unordered_map>

// Structure to store ball information
struct BallInfo {
    cv::Point2f center;
    int radius;
    cv::Scalar initialColor;
    cv::Scalar color;
};

// Global variable to store detected balls
extern std::unordered_map<int, BallInfo> balls;

// Function declarations
cv::Scalar determineColor(const cv::Mat& frame, const cv::Vec3f& circle, const cv::Vec3b& avgIntensity, double avgBrightness);
std::unordered_map<int, BallInfo> detectBalls(cv::Mat& frame, const std::vector<cv::Point2f>& corners, int& ballId);
void drawBalls(cv::Mat& frame, const std::unordered_map<int, BallInfo>& balls);
std::unordered_map<int, BallInfo> detectAndDrawBalls(const std::string& videoPath, cv::Mat& frame, cv::Mat& result);
double calculateAverageBrightness(const cv::Mat& frame, const std::vector<cv::Point>& roiCorners);
cv::Vec3b calculateAverageIntensity(const cv::Mat& frame, const std::vector<cv::Point>& roiCorners);

#endif // BALLTRACKING_H
