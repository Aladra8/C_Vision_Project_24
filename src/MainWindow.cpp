#include "MainWindow.h"
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QPushButton *btn = new QPushButton("Start", this);
    QLabel *label = new QLabel(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(btn);
    layout->addWidget(label);
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    setCentralWidget(widget);

    connect(btn, &QPushButton::clicked, [=]() {
        cv::VideoCapture cap("billiard_match.mp4");
        if (!cap.isOpened()) {
            label->setText("Error: Could not open video.");
            return;
        }
        cv::Mat frame;
        while (cap.read(frame)) {
            // Process frame
            // Convert to QImage and display
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
            QImage img((const uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            label->setPixmap(QPixmap::fromImage(img));
            QApplication::processEvents();
        }
    });
}
