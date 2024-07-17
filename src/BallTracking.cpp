#include "BallTracking.h"
#include "VideoProcessor.h"
#include <iostream>
#include <opencv2/opencv.hpp>

// Definition of the global variable balls
std::unordered_map<int, BallInfo> balls;

// Function to determine the predominant color within a circle
cv::Scalar determineColor(const cv::Mat& frame, const cv::Vec3f& circle) {
    cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
    int radius = cvRound(circle[2]);
    int radiusSq = radius * radius;
    int countWhite = 0, countBlack = 0, countColored = 0;
    int totalPixels = 0;

    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            int distSq = dx * dx + dy * dy;
            if (distSq <= radiusSq) {
                int x = center.x + dx;
                int y = center.y + dy;
                if (x >= 0 && y >= 0 && x < frame.cols && y < frame.rows) {
                    cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);
                    if (pixel[0] > 170 && pixel[1] > 170 && pixel[2] > 170)
                        countWhite++;
                    else if (pixel[0] < 30 && pixel[1] < 30 && pixel[2] < 30)
                        countBlack++;
                    else if (pixel[0] > 30 && pixel[1] > 30 && pixel[2] > 30)
                        countColored++;
                    totalPixels++;
                }
            }
        }
    }

    double percentWhite = static_cast<double>(countWhite) / totalPixels;
    double percentBlack = static_cast<double>(countBlack) / totalPixels;
    double percentColored = static_cast<double>(countColored) / totalPixels;

    if (percentWhite > 0.15)
        return cv::Scalar(255, 255, 255); // White
    else if (percentBlack > 0.1)
        return cv::Scalar(0, 0, 0); // Black
    else if (percentColored > 0.05 && percentWhite < 0.15 && percentWhite > 0.01)
        return cv::Scalar(0, 0, 255); // Red
    else if (percentColored > 0.05 && percentWhite < 0.01)
        return cv::Scalar(255, 0, 0); // Blue
}

std::unordered_map<int, BallInfo> detectBalls(cv::Mat& frame, const std::vector<cv::Point2f>& corners, int& ballId) {
    std::unordered_map<int, BallInfo> detectedBalls;

    cv::Mat gray, maskedGray;
    maskedGray = cv::Mat::zeros(frame.size(), CV_8UC1);

    std::vector<cv::Point> roiCorners;
    for (const auto& corner : corners) {
        roiCorners.push_back(cv::Point(static_cast<int>(corner.x), static_cast<int>(corner.y)));
    }
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
            BallInfo newBall = { center, radius, determineColor(frame, circle), determineColor(frame, circle) };
            detectedBalls[ballId++] = newBall;
        }
    }

    return detectedBalls;
}

void drawBalls(cv::Mat& frame, const std::unordered_map<int, BallInfo>& balls) {
    for (auto& [id, ball] : balls) {
        cv::circle(frame, ball.center, ball.radius, ball.color, -1);
    }
}

std::unordered_map<int, BallInfo> detectAndDrawBalls(const std::string& videoPath, cv::Mat& frame, cv::Mat& result) {
    VideoProcessor vp;
    std::vector<cv::Point2f> corners;

    try {
        corners = vp.findCorners(videoPath);
        corners = vp.sortCorners(corners);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return {}; // return an empty map in case of an exception
    }
    int ballId = 0;

    std::unordered_map<int, BallInfo> detectedBalls = detectBalls(frame, corners, ballId);
    drawBalls(result, detectedBalls);

    return detectedBalls; // return detectedBalls
}
