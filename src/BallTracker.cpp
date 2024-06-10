#include "BallTracker.h"

void BallTracker::detectAndTrackBalls(cv::Mat &frame) {
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(frame, circles, cv::HOUGH_GRADIENT, 1, frame.rows / 8, 200, 100);

    for (size_t i = 0; i < circles.size(); i++) {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        cv::circle(frame, center, radius, cv::Scalar(0, 255, 0), 3);
    }

    cv::imshow("Detected Balls", frame);
}
