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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    videoLabel(new QLabel(this)) {  // Initialize the QLabel
    setCentralWidget(videoLabel);   // Set it as the central widget
}

MainWindow::~MainWindow() {
    delete videoLabel;  // Clean up the QLabel
}

void MainWindow::displayFrame(const cv::Mat &frame) {
    cv::Mat rgbFrame;
    cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
    QImage qimg(rgbFrame.data, rgbFrame.cols, rgbFrame.rows, rgbFrame.step, QImage::Format_RGB888);
    videoLabel->setPixmap(QPixmap::fromImage(qimg));  // Use the QLabel to display the frame
}
