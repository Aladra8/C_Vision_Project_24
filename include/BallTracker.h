// #ifndef BALLTRACKER_H
// #define BALLTRACKER_H

// #include <opencv2/opencv.hpp>

// class BallTracker {
// public:
//     void detectAndTrackBalls(cv::Mat &frame);
// };

// #endif // BALLTRACKER_H


#ifndef BALLTRACKER_H
#define BALLTRACKER_H

#include <opencv2/opencv.hpp>

class BallTracker {
public:
    void detectAndTrackBalls(cv::Mat &frame);
    void drawBoundingBoxes(cv::Mat &frame, const std::vector<cv::Rect> &bboxes);
};

#endif // BALLTRACKER_H
