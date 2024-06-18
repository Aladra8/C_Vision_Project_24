// #include "BallTracker.h"

// void BallTracker::detectAndTrackBalls(cv::Mat &frame) {
//     std::vector<cv::Vec3f> circles;
//     cv::HoughCircles(frame, circles, cv::HOUGH_GRADIENT, 1, frame.rows / 8, 200, 100);

//     for (size_t i = 0; i < circles.size(); i++) {
//         cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
//         int radius = cvRound(circles[i][2]);
//         cv::circle(frame, center, radius, cv::Scalar(0, 255, 0), 3);
//     }

//     cv::imshow("Detected Balls", frame);
// }

#include "BallTracker.h"

BallTracker::BallTracker()
{
    bgSubtractor = cv::createBackgroundSubtractorMOG2();
    mlModel = nullptr;
}

void BallTracker::setMLModel(MLModel *model)
{
    mlModel = model;
}

void BallTracker::track(const cv::Mat &frame, std::vector<cv::Rect> &trackedBalls)
{
    if (mlModel)
    {
        trackedBalls = mlModel->detectObjects(frame);
    }
    else
    {
        cv::Mat fgMask;
        bgSubtractor->apply(frame, fgMask);
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(fgMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        for (const auto &contour : contours)
        {
            cv::Rect boundingBox = cv::boundingRect(contour);
            if (boundingBox.area() > 500)
            { // Filter small objects
                trackedBalls.push_back(boundingBox);
            }
        }
    }
}
