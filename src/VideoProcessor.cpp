#include "VideoProcessor.h"
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

void VideoProcessor::processVideo(const std::string& videoPath) {
    cv::VideoCapture cap(videoPath);

    if (!cap.isOpened()) {
        std::cerr << "Error opening file: " << videoPath << std::endl;
        return;
    }

    cv::Mat frame, grayFrame, edges;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // Convert frame to grayscale
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        // Apply Gaussian blur to reduce noise
        cv::GaussianBlur(grayFrame, grayFrame, cv::Size(9, 9), 2);

        // Apply Canny edge detector
        double threshold1 = 30; 
        double threshold2 = 80;
        cv::Canny(grayFrame, edges, threshold1, threshold2);

        // Apply Hough Line Transform to detect lines
        std::vector<cv::Vec2f> lines;
        cv::HoughLines(edges, lines, 1, CV_PI / 180, 100);

        // Draw detected lines on the original frame
        cv::Mat result = frame.clone();
        for (size_t i = 0; i < lines.size(); ++i) {
            float rho = lines[i][0];
            float theta = lines[i][1];
            double a = cos(theta);
            double b = sin(theta);
            double x0 = a * rho;
            double y0 = b * rho;
            cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
            cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
            cv::line(result, pt1, pt2, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
        }

        // Show the edges and detected lines
        cv::imshow("Edges and Lines", result);

        if (cv::waitKey(30) >= 0) break;
    }

    cap.release();
    cv::destroyAllWindows();
}