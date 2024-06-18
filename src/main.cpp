#include <QApplication>
#include "MainWindow.h"
#include "ImageProcessor.h"
#include "BallTracker.h"
#include "MLModel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // Load the machine learning model
    MLModel mlModel;
    mlModel.loadModel("/Users/babadrammeh/C_Vision_Labs/FinalProject/C_Vision_Project/Dataset/game1_clip1/game1_clip1.mp4"); // Update this path if necessary

    // Create instances of ImageProcessor and BallTracker
    ImageProcessor imageProcessor;
    BallTracker ballTracker;

    // Set the ML model for both ImageProcessor and BallTracker
    imageProcessor.setMLModel(&mlModel);
    ballTracker.setMLModel(&mlModel);

    // Capture video
    cv::VideoCapture cap(0); // Open default camera
    if (!cap.isOpened())
    {
        return -1;
    }

    cv::Mat frame;
    while (cap.read(frame))
    {
        cv::Mat processedFrame;
        imageProcessor.processFrame(frame, processedFrame);

        std::vector<cv::Rect> detectedBalls;
        imageProcessor.detectBalls(processedFrame, detectedBalls);

        std::vector<cv::Rect> trackedBalls;
        ballTracker.track(processedFrame, trackedBalls);

        // Display the frame with detected and tracked balls
        for (const auto &rect : detectedBalls)
        {
            cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
        }
        for (const auto &rect : trackedBalls)
        {
            cv::rectangle(frame, rect, cv::Scalar(255, 0, 0), 2);
        }

        w.displayFrame(frame);
        if (cv::waitKey(30) >= 0)
            break;
    }

    return a.exec();
}