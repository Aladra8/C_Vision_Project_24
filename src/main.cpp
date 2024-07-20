#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

/*
This is the main that i use

#include <opencv2/opencv.hpp>
#include <stdexcept> 
#include <typeinfo>  
#include "VideoProcessor.h"
#include "BallTracking.h"

int main(){
    try{
        // Insert video path here
        std::string videoPath = "";
        VideoProcessor v;
        v.processVideo(videoPath);
    } 
    catch (const std::exception& e){
        std::cerr << "Exception caught: " << e.what() << std::endl;
        std::cerr << "Description: " << typeid(e).name() << std::endl;
    }
    catch (const cv::Exception& e){
        std::cerr << "OpenCV exception: " << e.what() << std::endl;
        std::cerr << "Description: " << typeid(e).name() << std::endl;
    }
    return 0;
}




*/