#include "VideoProcessor.h"
//the function takes as argument the path of the video
void VideoProcessor::processVideo(const std::string& videoPath) {
    cv::VideoCapture cap(videoPath); // Read the video from the specified path

    if (!cap.isOpened()) {
        std::cerr << "Error while trying to open the file: " << videoPath << std::endl;
        return;
    }

    cv::Mat frame, hsvFrame, maskGreen, maskBlue, maskCombined;
    std::vector<std::vector<cv::Point>> contours; // Contains the contours
    std::vector<cv::Vec4i> hierarchy; // Contains the hierarchy of contours

    std::vector<cv::Point> sumVertices; // Sum of vertices from all frames

    while (true){ // Read frames until interrupted
        cap >> frame; // Read a frame
        if (frame.empty()) break;

        // Convert the frame to HSV
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

        // Define upper and lower bounds for the masks
        // Define the range of green color to segment the field
        cv::Scalar lowerGreen(35, 43, 46);
        cv::Scalar upperGreen(85, 255, 255);
        cv::inRange(hsvFrame, lowerGreen, upperGreen, maskGreen); // Create a mask

        // Define the range of blue color to segment the field
        cv::Scalar lowerBlue(30, 43, 46);
        cv::Scalar upperBlue(255, 255, 255);
        cv::inRange(hsvFrame, lowerBlue, upperBlue, maskBlue);

        // Combine both masks
        cv::bitwise_or(maskGreen, maskBlue, maskCombined); 

        // Find contours
        cv::findContours(maskCombined, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Identify the largest contour
        double maxArea = 0;
        int maxAreaIdx = -1;
        for (size_t i = 0; i < contours.size(); i++){ // For each contour
            double area = cv::contourArea(contours[i]); // Calculate the area
            if (area > maxArea) {
                maxArea = area;
                maxAreaIdx = i;
            }
        }

        // Draw contours on the board
        if (maxAreaIdx != -1){ 
            std::vector<cv::Point> vertices; // Contains the approximated polygonal curve of contours[maxAreaIdx]
            
            // Calculate the approximated vertices
            cv::approxPolyDP(contours[maxAreaIdx], vertices, 0.05 * cv::arcLength(contours[maxAreaIdx], true), true);

            // Add vertices to the sum
            if (vertices.size() == 4){
                for (const auto& vertex : vertices) {
                    sumVertices.push_back(vertex);
                }
            }
        }
    }

    // Calculate average vertices
    cv::Point avgVertex(0, 0);
    for (const auto& vertex : sumVertices) {
        avgVertex += vertex;
    }
    avgVertex /= static_cast<int>(sumVertices.size());

    // Show the video with overlaid average edge position
    cap.release();
    cap.open(videoPath); // Reopen the video from the beginning

    while (true){
        cap >> frame;
        if (frame.empty()) break;

        // Draw lines for the average edge position
        cv::line(frame, sumVertices[0], sumVertices[1], cv::Scalar(0, 0, 255), 2);
        cv::line(frame, sumVertices[1], sumVertices[2], cv::Scalar(0, 0, 255), 2);
        cv::line(frame, sumVertices[2], sumVertices[3], cv::Scalar(0, 0, 255), 2);
        cv::line(frame, sumVertices[3], sumVertices[0], cv::Scalar(0, 0, 255), 2);

        // Show the frame with the overlaid average edge position
        cv::imshow("Average Edge Position", frame);
        if (cv::waitKey(30) >= 0) break;
    }

    cap.release();
    cv::destroyAllWindows();
}