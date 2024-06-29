#include "VideoProcessor.h"
#include <iostream>

std::vector<cv::Point2f> VideoProcessor::findCorners(const std::string& videoPath){
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()){
        std::cerr << "Error opening file: " << videoPath << std::endl;
        return {};
    }

    cv::Mat frame, grayFrame, edges;
    cv::Scalar lowerBound(45, 40, 40);   // Lower HSV threshold for color filtering
    cv::Scalar upperBound(140, 255, 255); // Upper HSV threshold for color filtering
    const float minCornerDistance = 150.0; // Minimum distance between corners

    while (true){
        cap >> frame;
        if (frame.empty()) break;

        // Mask to exclude outer edges of the image
        cv::Mat mask = cv::Mat::ones(frame.size(), CV_8UC1) * 255;
        int margin = 50;  // Margin to ignore
        cv::rectangle(mask, cv::Point(0, 0), cv::Point(frame.cols, margin), cv::Scalar(0), cv::FILLED);
        cv::rectangle(mask, cv::Point(0, frame.rows - margin), cv::Point(frame.cols, frame.rows), cv::Scalar(0), cv::FILLED);

        cv::Mat hsvFrame;
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);
        cv::Mat colorMask;
        cv::inRange(hsvFrame, lowerBound, upperBound, colorMask);
        colorMask &= mask; // Application of the mask

        cv::Mat maskedFrame;
        frame.copyTo(maskedFrame, colorMask);
        cv::cvtColor(maskedFrame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(grayFrame, grayFrame, cv::Size(3, 3), 2); // Small gaussian filter to remove small details
        cv::Canny(grayFrame, edges, 30, 80); // Application of Canny edge detector

        std::vector<cv::Vec2f> lines;
        cv::HoughLines(edges, lines, 1, CV_PI / 180, 180); // Application of Hough transform to find the image lines

        // Removes the lines that are close to each other
        std::vector<cv::Vec2f> filteredLines; // Contains the lines to keep
        for (size_t i = 0; i < lines.size(); ++i){
            cv::Vec2f currentLine = lines[i];
            bool keepLine = true;
            for (size_t j = 0; j < filteredLines.size(); ++j){
                cv::Vec2f storedLine = filteredLines[j];
                // Calculates the distante between the candidate line to keep and the filtered lines 
                float rhoDiff = std::abs(currentLine[0] - storedLine[0]);
                float thetaDiff = std::abs(currentLine[1] - storedLine[1]);
                if (rhoDiff < 20 && thetaDiff < CV_PI / 18){
                    keepLine = false;
                    break;
                }
            }
            if (keepLine){
                filteredLines.push_back(currentLine);
            }
        }

        // Finds corners of the board
        std::vector<cv::Point2f> corners; // Contains the corners' coordinates
        for (size_t i = 0; i < filteredLines.size(); ++i){
            for (size_t j = i + 1; j < filteredLines.size(); ++j){
                // Extracts two lines to confront
                cv::Vec2f line1 = filteredLines[i];
                cv::Vec2f line2 = filteredLines[j];

                // Exctract the lines' parameters
                float rho1 = line1[0], theta1 = line1[1];
                float rho2 = line2[0], theta2 = line2[1];

                cv::Point2f intersection; // Intersection point of the two lines
                intersection.x = (rho2 * sin(theta1) - rho1 * sin(theta2)) / sin(theta1 - theta2);
                intersection.y = (rho1 * cos(theta2) - rho2 * cos(theta1)) / sin(theta1 - theta2);

                if (intersection.x > 0 && intersection.x < frame.cols && intersection.y > margin && intersection.y < frame.rows - margin) {
                    bool keepCorner = true;
                    // Eliminates the corners that are to close to each other
                    for (const auto& corner : corners){
                        if (cv::norm(intersection - corner) < minCornerDistance){
                            keepCorner = false;
                            break;
                        }
                    }
                    if (keepCorner){
                        corners.push_back(intersection);
                    }
                }
            }
        }
        // Returns the corners only if there are 4 of them, this is done because sometimes more than 4
        // corners are found, so instead of removing the outliers it's possible to consider the instances
        // where there are 4 of them. This can require multiple frames of a video
        if (corners.size() == 4){
            return corners;
        }
    }

    return {};
}

void VideoProcessor::processVideo(const std::string& videoPath){
    std::vector<cv::Point2f> corners = findCorners(videoPath);

    if (corners.size() != 4){
        std::cerr << "Error: Could not find exactly 4 corners" << std::endl;
        return;
    }

    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()){
        std::cerr << "Error opening file: " << videoPath << std::endl;
        return;
    }

    cv::Mat frame;
    while (true){
        cap >> frame;
        if (frame.empty()) break;

        cv::Mat result = frame.clone();

        // Draws found points as red circles
        for (size_t i = 0; i < corners.size(); ++i){
            cv::circle(result, corners[i], 5, cv::Scalar(0, 0, 255), -1);
        }

        // Draws rectangle around the found points
        std::vector<cv::Point> rectanglePoints;
        rectanglePoints.push_back(corners[0]);
        rectanglePoints.push_back(corners[1]);
        rectanglePoints.push_back(corners[2]);
        rectanglePoints.push_back(corners[3]);
        rectanglePoints.push_back(corners[0]); // Closes the rectangle

        cv::polylines(result, rectanglePoints, true, cv::Scalar(0, 255, 0), 2);

        // Draws the edges of the billiard table
        // Calculates centroid of the four points
        cv::Point2f centroid(0, 0);
        for (const auto& corner : corners){
            centroid += corner;
        }
        centroid *= (1.0 / corners.size());

        // Calculates angles of the corners relative to the centroid
        std::vector<double> angles;
        for (const auto& corner : corners){
            double angle = std::atan2(corner.y - centroid.y, corner.x - centroid.x);
            angles.push_back(angle);
        }

        // Sorts corners based on angles in clockwise order
        for (size_t i = 0; i < corners.size(); ++i){
            for (size_t j = i + 1; j < corners.size(); ++j){
                if (angles[i] > angles[j]){
                    std::swap(corners[i], corners[j]);
                    std::swap(angles[i], angles[j]);
                }
            }
        }
        // Draws the corner lines
        cv::line(result, corners[0], corners[1], cv::Scalar(100, 255, 255), 2);
        cv::line(result, corners[1], corners[2], cv::Scalar(100, 255, 255), 2);
        cv::line(result, corners[2], corners[3], cv::Scalar(100, 255, 255), 2);
        cv::line(result, corners[3], corners[0], cv::Scalar(100, 255, 255), 2);

        cv::imshow("Detected Corners and Borders", result);

        if (cv::waitKey(30) >= 0) break;
    }

    cap.release();
    cv::destroyAllWindows();
}