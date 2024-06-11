#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <string>

class VideoProcessor {
public:
    static void processVideo(const std::string& videoPath);
};

#endif // VIDEOPROCESSOR_H