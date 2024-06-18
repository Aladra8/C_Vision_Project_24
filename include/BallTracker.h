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
#include <vector>
#include "MLModel.h"

class BallTracker {
public:
    BallTracker();
    void track(const cv::Mat &frame, std::vector<cv::Rect> &trackedBalls);
    void setMLModel(MLModel* mlModel);

private:
    cv::Ptr<cv::BackgroundSubtractor> bgSubtractor;
    MLModel* mlModel;
};

#endif // BALLTRACKER_H
