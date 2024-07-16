#ifndef BALLTRACKING_H
#define BALLTRACKING_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

// Function to detect and draw billiard balls in a video
void detectAndDrawBalls(const std::string& videoPath);
void colorCircle(cv::Mat& frame, const cv::Vec3f& circle);


#endif // BALLTRACKING_H