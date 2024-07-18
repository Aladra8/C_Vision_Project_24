#include "BallTracking.h"
#include "VideoProcessor.h"
#include <iostream>
#include <opencv2/opencv.hpp>

// Definition of the global variable balls
std::unordered_map<int, BallInfo> balls;

// Function to determine the predominant color within a circle
cv::Scalar determineColor(const cv::Mat& frame, const cv::Vec3f& circle, const cv::Vec3b& avgIntensity, double avgBrightness) {
    cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
    int radius = cvRound(circle[2]);
    int radiusSq = radius * radius;
    int countWhite = 0, countBlack = 0, countColored = 0;
    int totalPixels = 0;

    // Adjust thresholds based on average brightness
    int whiteThreshold = std::min(255, static_cast<int>(180 + avgBrightness)/2);
    int blackThreshold = std::max(0, static_cast<int>(30 - avgBrightness)/2);
    int colorThreshold = std::max(30, static_cast<int>(30 - avgBrightness)/2);

    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            int distSq = dx * dx + dy * dy;
            if (distSq <= radiusSq) {
                int x = center.x + dx;
                int y = center.y + dy;
                if (x >= 0 && y >= 0 && x < frame.cols && y < frame.rows) {
                    cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);

                    // Ignore pixels close to the average intensity
                    if (std::abs(pixel[0] - avgIntensity[0]) > 10 || std::abs(pixel[1] - avgIntensity[1]) > 10 || std::abs(pixel[2] - avgIntensity[2]) > 10) {
                        if (pixel[0] > whiteThreshold && pixel[1] > whiteThreshold && pixel[2] > whiteThreshold)
                            countWhite++;
                        else if (pixel[0] < blackThreshold && pixel[1] < blackThreshold && pixel[2] < blackThreshold)
                            countBlack++;
                        else if (pixel[0] > colorThreshold && pixel[1] > colorThreshold && pixel[2] > colorThreshold)
                            countColored++;
                        totalPixels++;
                    }
                }
            }
        }
    }

    double percentWhite = static_cast<double>(countWhite) / totalPixels;
    double percentBlack = static_cast<double>(countBlack) / totalPixels;
    double percentColored = static_cast<double>(countColored) / totalPixels;

    if (percentWhite > 0.2)
        return cv::Scalar(255, 255, 255); // White
    else if (percentBlack > 0.1)
        return cv::Scalar(0, 0, 0); // Black
    else if (percentColored > 0.1 && percentWhite < 0.15 && percentWhite > 0.01)
        return cv::Scalar(0, 0, 255); // Red
    else if (percentColored > 0.1 && percentWhite < 0.01)
        return cv::Scalar(255, 0, 0); // Blue
}

// Function to calculate the average brightness of an image within the ROI
double calculateAverageBrightness(const cv::Mat& frame, const std::vector<cv::Point>& roiCorners) {
    cv::Mat gray, mask;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY); // Convert to grayscale

    // Create mask from ROI corners
    mask = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::fillConvexPoly(mask, roiCorners, cv::Scalar(255));

    cv::Scalar avgBrightness = cv::mean(gray, mask); // Calculate the mean brightness within the mask
    return avgBrightness[0]; // Return the average brightness
}

// Function to calculate the average intensity of the region of interest (ROI)
cv::Vec3b calculateAverageIntensity(const cv::Mat& frame, const std::vector<cv::Point>& roiCorners) {
    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::fillConvexPoly(mask, roiCorners, cv::Scalar(255));
    cv::Scalar avgIntensity = cv::mean(frame, mask);
    return cv::Vec3b(avgIntensity[0], avgIntensity[1], avgIntensity[2]);
}

// Function to detect balls in the frame
std::unordered_map<int, BallInfo> detectBalls(cv::Mat& frame, const std::vector<cv::Point2f>& corners, int& ballId) {
    std::unordered_map<int, BallInfo> detectedBalls;

    std::vector<cv::Point> roiCorners;
    for (const auto& corner : corners) {
        roiCorners.push_back(cv::Point(static_cast<int>(corner.x), static_cast<int>(corner.y)));
    }

    cv::Vec3b avgIntensity = calculateAverageIntensity(frame, roiCorners);
    double avgBrightness = calculateAverageBrightness(frame, roiCorners);

    cv::Mat gray, maskedGray;
    maskedGray = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);
    cv::fillConvexPoly(mask, roiCorners, cv::Scalar(255));
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    gray.copyTo(maskedGray, mask);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(maskedGray, circles, cv::HOUGH_GRADIENT, 1, 20, 25, 20, 5, 20);

    for (const auto& circle : circles) {
        cv::Point2f center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);

        bool ballFound = false;
        for (auto& [id, ball] : balls) {
            if (cv::norm(ball.center - center) < radius) {
                detectedBalls[id] = { center, radius, ball.initialColor, ball.color };
                ballFound = true;
                break;
            }
        }

        if (!ballFound) {
            BallInfo newBall = { center, radius, determineColor(frame, circle, avgIntensity, avgBrightness), determineColor(frame, circle, avgIntensity, avgBrightness) };
            detectedBalls[ballId++] = newBall;
        }
    }

    return detectedBalls;
}

// Function to draw balls on the frame
void drawBalls(cv::Mat& frame, const std::unordered_map<int, BallInfo>& balls) {
    for (auto& [id, ball] : balls) {
        cv::circle(frame, ball.center, ball.radius, ball.color, -1);
    }
}

// Function to detect and draw balls in a video
std::unordered_map<int, BallInfo> detectAndDrawBalls(const std::string& videoPath, cv::Mat& frame, cv::Mat& result) {
    VideoProcessor vp;
    std::vector<cv::Point2f> corners;

    try {
        corners = vp.findCorners(videoPath);
        corners = vp.sortCorners(corners);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return {}; // Return an empty map in case of an exception
    }
    int ballId = 0;

    std::unordered_map<int, BallInfo> detectedBalls = detectBalls(frame, corners, ballId);
    drawBalls(result, detectedBalls);

    return detectedBalls; // Return detectedBalls
}
