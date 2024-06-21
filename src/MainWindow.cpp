// #include "MainWindow.h"
// #include <QApplication>
// #include <QPushButton>
// #include <QLabel>
// #include <QVBoxLayout>
// #include <opencv2/opencv.hpp>

// MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
// {
//     QPushButton *btn = new QPushButton("Start", this);
//     QLabel *label = new QLabel(this);
//     QVBoxLayout *layout = new QVBoxLayout();
//     layout->addWidget(btn);
//     layout->addWidget(label);
//     QWidget *widget = new QWidget();
//     widget->setLayout(layout);
//     setCentralWidget(widget);

//     connect(btn, &QPushButton::clicked, [=]()
//             {
//         std::string videoPath = "/Users/babadrammeh/C_Vision_Labs/FinalProject/C_Vision_Project/dataset/game1_clip1/game1_clip1.mp4"; // Use absolute path here
//         cv::VideoCapture cap(videoPath);
//         if (!cap.isOpened()) {
//             label->setText("Error: Could not open video.");
//             return;
//         }
//         cv::Mat frame;
//         while (cap.read(frame)) {
//             // Process frame
//             // Convert to QImage and display
//             cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
//             QImage img((const uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
//             label->setPixmap(QPixmap::fromImage(img));
//             QApplication::processEvents();
//         } });
// }

// MainWindow::~MainWindow()
// {
//     // Destructor implementation (if needed)
// }


#include "MainWindow.h"
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QPushButton *btn = new QPushButton("Start", this);
    QLabel *label = new QLabel(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(btn);
    layout->addWidget(label);
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    setCentralWidget(widget);

    try {
        mlModel.loadModel("/absolute/path/to/model.pb"); // Provide the correct path to your model file
    } catch (const std::runtime_error& e) {
        label->setText("Error: " + QString::fromStdString(e.what()));
        return;
    }

    connect(btn, &QPushButton::clicked, [=]() {
        std::string videoPath = "/Users/babadrammeh/C_Vision_Labs/FinalProject/C_Vision_Project/Dataset/game1_clip1/game1_clip1.mp4"; // Use absolute path here
        std::vector<cv::Mat> frames = imageProcessor.extractFrames(videoPath);
        if (frames.empty()) {
            label->setText("Error: Could not open video.");
            return;
        }

        std::vector<cv::Rect> bboxes = imageProcessor.loadBoundingBoxes("/Users/babadrammeh/C_Vision_Labs/FinalProject/C_Vision_Project/Dataset/game1_clip1/bounding_boxes/frame_first_bbox.txt"); // Provide the correct path to the bounding box file
        for (auto &frame : frames) {
            imageProcessor.processFrame(frame);
            ballTracker.drawBoundingBoxes(frame, bboxes);
            try {
                mlModel.predictOutcome(frame);
            } catch (const std::runtime_error& e) {
                std::cerr << "Prediction error: " << e.what() << std::endl;
                continue;
            }
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
            QImage img((const uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            label->setPixmap(QPixmap::fromImage(img));
            QApplication::processEvents();
        }
    });
}

MainWindow::~MainWindow() {
    // Destructor implementation
}
