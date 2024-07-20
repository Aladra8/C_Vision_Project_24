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

#include "VideoProcessor.h"
#include "BallTracking.h"

int main(){
    // Base directory containing all folders
    std::string baseDir = "../Dataset";

    // List of folders to iterate
    std::vector<std::string> folderNames = {
        "game1_clip1", "game1_clip2", "game1_clip3", "game1_clip4",
        "game2_clip1", "game2_clip2",
        "game3_clip1", "game3_clip2",
        "game4_clip1", "game4_clip2"
    };

    // Iterate through each folder and process videos
    for (const auto& folderName : folderNames){
        std::string videoFileName = folderName + ".mp4";
        std::string videoPath = baseDir + "/" + folderName + "/" + videoFileName;
        std::cout << "Processing video: " << videoPath << std::endl;

        try{
            VideoProcessor v;
            v.processVideo(videoPath);
        }
        catch (const std::exception& e){
            std::cerr << "Exception caught while processing video " << videoPath << ": " << e.what() << std::endl;
            std::cerr << "Description: " << typeid(e).name() << std::endl;
        }
        catch (const cv::Exception& e){
            std::cerr << "OpenCV exception while processing video " << videoPath << ": " << e.what() << std::endl;
            std::cerr << "Description: " << typeid(e).name() << std::endl;
        }
    }

    return 0;
}





*/