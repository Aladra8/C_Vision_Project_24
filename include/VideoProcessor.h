#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

class VideoProcessor{
public:
    void processVideo(const std::string& videoPath);

private:
    std::vector<cv::Point2f> findCorners(const std::string& videoPath);
};

#endif // VIDEOPROCESSOR_H