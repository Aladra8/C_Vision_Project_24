#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void displayFrame(const cv::Mat &frame);

private:
    QLabel *videoLabel;  // Add a QLabel for displaying the video
};

#endif // MAINWINDOW_H