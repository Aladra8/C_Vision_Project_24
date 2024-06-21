#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ImageProcessor.h"
#include "BallTracker.h"
#include "MLModel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    ImageProcessor imageProcessor;
    BallTracker ballTracker;
    MLModel mlModel;
};

#endif // MAINWINDOW_H
