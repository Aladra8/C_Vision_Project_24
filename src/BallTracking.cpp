#include "BallTracking.h"
#include "VideoProcessor.h"
#include <iostream>
#include <opencv2/opencv.hpp>

// Function to determine the predominant color within a circle and color it accordingly
void colorCircle(cv::Mat& frame, const cv::Vec3f& circle) {
    // Extract the parameters of the circle
    cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
    int radius = cvRound(circle[2]);

    // Calculate the square of the radius for faster comparisons
    int radiusSq = radius * radius;

    // Counters for the colors
    int countWhite = 0, countBlack = 0, countColored = 0;
    int totalPixels = 0;

    // Iterate over the pixels inside the circle
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            // Calculate the square distance from the center of the circle
            int distSq = dx * dx + dy * dy;

            // Check if the pixel is inside the circle
            if (distSq <= radiusSq) {
                int x = center.x + dx;
                int y = center.y + dy;

                // Ensure the pixel is within the image boundaries
                if (x >= 0 && y >= 0 && x < frame.cols && y < frame.rows) {
                    cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);

                    // Determine the predominant color
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

    // Calculate the color percentages
    double percentWhite = static_cast<double>(countWhite) / totalPixels;
    double percentBlack = static_cast<double>(countBlack) / totalPixels;
    double percentColored = static_cast<double>(countColored) / totalPixels;

    // Determine the color of the circle based on the predominant color found
    if (percentWhite > 0.15) {
        // If white pixels are predominant
        cv::circle(frame, center, radius, cv::Scalar(255, 255, 255), -1); // Color the entire circle white
    } else if (percentBlack > 0.01) {
        // If If black pixels are predominant
        cv::circle(frame, center, radius, cv::Scalar(0, 0, 0), -1); // Color the entire circle black
    } else if (percentColored > 0.01 && percentWhite < 0.15 && percentWhite > 0.01) {
        // If colored is predominant and little white (colored ball + white line)
        cv::circle(frame, center, radius, cv::Scalar(0, 0, 255), -1); // Color the entire circle red
    } else if (percentColored > 0.01 && percentWhite < 0.01) {
        // If colored is predominant (little to no white color)
        cv::circle(frame, center, radius, cv::Scalar(255, 0, 0), -1); // Color the entire circle blue
    }
}

void detectAndDrawBalls(const std::string& videoPath) {
    VideoProcessor vp;
    std::vector<cv::Point2f> corners;
    
    // Finds corners to specify area of search
    try {
        corners = vp.findCorners(videoPath);
        // Sort corners in clockwise order
        corners = vp.sortCorners(corners);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return;
    }

    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        throw std::invalid_argument("Error opening video file: " + videoPath);
    }

    cv::Mat frame, gray, maskedGray;
    while (cap.read(frame)) {
        maskedGray = cv::Mat::zeros(frame.size(), CV_8UC1);

        // Create a mask from the detected corners, the algorithm will only look on the specified area
        std::vector<cv::Point> roiCorners;
        for (const auto& corner : corners) {
            roiCorners.push_back(cv::Point(static_cast<int>(corner.x), static_cast<int>(corner.y)));
        }
        cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);
        cv::fillConvexPoly(mask, roiCorners, cv::Scalar(255));

        // Convert to grayscale
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Apply the mask to the grayscale image
        gray.copyTo(maskedGray, mask);

        // Detect circles using the Hough Transform
        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(maskedGray, circles, cv::HOUGH_GRADIENT, 1, 20, 25, 20, 5, 20);

        // Draw the detected circles and color them internally
        for (const auto& circle : circles) {
            colorCircle(frame, circle);
        }

        // Show the frame with the detected and colored circles
        cv::imshow("Billiard Balls Detection", frame);
        if (cv::waitKey(30) >= 0) break;  // Exit on key press
    }
}