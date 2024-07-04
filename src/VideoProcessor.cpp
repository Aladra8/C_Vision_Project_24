#include "VideoProcessor.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <algorithm>

std::vector<cv::Point2f> VideoProcessor::findCorners(const std::string& videoPath){
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()){
        std::cerr << "Error opening file: " << videoPath << std::endl;
        return {};
    }

    cv::Mat frame, grayFrame, edges;
    cv::Scalar lowerBound(45, 80, 80);   // Lower HSV threshold for color filtering
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
        cv::HoughLines(edges, lines, 1, CV_PI / 180, 145); // Application of Hough transform to find the image lines

        // Remove lines that are close to each other or merge parallel lines
        std::vector<cv::Vec2f> filteredLines; // Contains the lines to keep
        for (size_t i = 0; i < lines.size(); ++i){
            cv::Vec2f currentLine = lines[i];
            bool keepLine = true;
            for (size_t j = 0; j < filteredLines.size(); ++j){
                cv::Vec2f storedLine = filteredLines[j];
                // Calculate differences between current line and stored line
                float rhoDiff = std::abs(currentLine[0] - storedLine[0]);
                float thetaDiff = std::abs(currentLine[1] - storedLine[1]);
                if (rhoDiff < 20 && thetaDiff < CV_PI / 18){
                    keepLine = false;
                    // Calculate average line if lines are close and parallel
                    float avgRho = (currentLine[0] + storedLine[0]) / 2;
                    float avgTheta = (currentLine[1] + storedLine[1]) / 2;
                    filteredLines[j] = cv::Vec2f(avgRho, avgTheta);
                    break;
                }
            }
            if (keepLine){
                filteredLines.push_back(currentLine);
            }
        }

        // Find corners of the board
        std::vector<cv::Point2f> corners; // Contains the corners' coordinates

        // Iterate through filtered lines to find intersections and group nearby parallel lines
        for (size_t i = 0; i < filteredLines.size(); ++i){
            for (size_t j = i + 1; j < filteredLines.size(); ++j){
                cv::Vec2f line1 = filteredLines[i];
                cv::Vec2f line2 = filteredLines[j];

                // Calculate intersection point
                cv::Point2f intersection;
                float rho1 = line1[0], theta1 = line1[1];
                float rho2 = line2[0], theta2 = line2[1];
                intersection.x = (rho2 * sin(theta1) - rho1 * sin(theta2)) / sin(theta1 - theta2);
                intersection.y = (rho1 * cos(theta2) - rho2 * cos(theta1)) / sin(theta1 - theta2);

                // Check if intersection point is within image bounds and not too close to image edges
                if (intersection.x > 0 && intersection.x < frame.cols &&
                    intersection.y > margin && intersection.y < frame.rows - margin) {
                    bool keepCorner = true;

                    // Check distance from existing corners
                    for (const auto& corner : corners){
                        if (cv::norm(intersection - corner) < minCornerDistance){
                            keepCorner = false;
                            break;
                        }
                    }

                    // If intersection point is not too close to existing corners, add it
                    if (keepCorner){
                        corners.push_back(intersection);
                    }
                }
            }
        }

        // Return corners only if exactly 4 corners are found
        if (corners.size() == 4){
            return corners;
        }
    }

    return {};
}

std::vector<cv::Point2f> VideoProcessor::sortCorners(const std::vector<cv::Point2f>& corners) {
    // Calculate centroid of the four corners
    cv::Point2f centroid(0, 0);
    for (const auto& corner : corners) {
        centroid += corner;
    }
    centroid *= (1.0 / corners.size());

    // Calculate angles of corners relative to the centroid
    std::vector<double> angles;
    for (const auto& corner : corners) {
        double angle = std::atan2(corner.y - centroid.y, corner.x - centroid.x);
        angles.push_back(angle);
    }

    // Sort corners in clockwise order based on angles
    std::vector<cv::Point2f> sortedCorners = corners;
    for (size_t i = 0; i < sortedCorners.size(); ++i) {
        for (size_t j = i + 1; j < sortedCorners.size(); ++j) {
            if (angles[i] > angles[j]) {
                std::swap(sortedCorners[i], sortedCorners[j]);
                std::swap(angles[i], angles[j]);
            }
        }
    }

    // Calculate distances between corners to determine if rotation is necessary
    double dist01 = cv::norm(sortedCorners[0] - sortedCorners[1]);
    double dist12 = cv::norm(sortedCorners[1] - sortedCorners[2]);
    double dist23 = cv::norm(sortedCorners[2] - sortedCorners[3]);

    double margin = 0.2; // Margin to check if parallel borders are significantly different

    // Rotate if line 01 is shorter than 12
    if (dist01 < dist12 || dist23 < dist12 || dist01 > dist23 + dist23 * margin || dist23 > dist01 + dist01 * margin) {
        // Reorder corners so that index 0 becomes 1, 1 becomes 2, ..., 3 becomes 0
        std::vector<cv::Point2f> rotatedCorners = {
            sortedCorners[1],
            sortedCorners[2],
            sortedCorners[3],
            sortedCorners[0]
        };
        sortedCorners = rotatedCorners;
    }

    return sortedCorners;
}


void VideoProcessor::drawMinimap(cv::Mat& frame, const std::vector<cv::Point2f>& corners) {
    // Dimensions of the minimap
    const int minimapWidth = 300;
    const int minimapHeight = 150;

    // Define the four corners of the minimap at the bottom-left without margin
    if (dstCorners.empty()) {
        dstCorners = {
            cv::Point2f(0, frame.rows - minimapHeight),
            cv::Point2f(minimapWidth, frame.rows - minimapHeight),
            cv::Point2f(minimapWidth, frame.rows),
            cv::Point2f(0, frame.rows)
        };
    }

    // Calculate the perspective transform from the main table to the minimap
    if (perspectiveTransform.empty()) {
        perspectiveTransform = cv::getPerspectiveTransform(corners, dstCorners);
    }

    // Create an empty minimap with white background
    cv::Mat minimap = cv::Mat::zeros(minimapHeight, minimapWidth, CV_8UC3);
    minimap.setTo(cv::Scalar(255, 255, 255)); // Set white background

    // Position of the minimap on the main frame
    cv::Rect roi(cv::Point(0, frame.rows - minimapHeight), minimap.size());

    // Copy the minimap with white background onto the main frame
    minimap.copyTo(frame(roi));

    // Calculate the position of minimap corners on the main frame
    std::vector<cv::Point> dstCornersInt;
    for (const auto& corner : dstCorners) {
        dstCornersInt.push_back(cv::Point(static_cast<int>(corner.x), static_cast<int>(corner.y)));
    }

    // Draw the borders of the minimap on the main frame with different colors
    cv::line(frame, dstCornersInt[0], dstCornersInt[1], cv::Scalar(0, 0, 255), 2); // Red
    cv::line(frame, dstCornersInt[1], dstCornersInt[2], cv::Scalar(0, 255, 0), 2); // Green
    cv::line(frame, dstCornersInt[2], dstCornersInt[3], cv::Scalar(255, 0, 0), 2); // Blue
    cv::line(frame, dstCornersInt[3], dstCornersInt[0], cv::Scalar(0, 255, 255), 2); // Yellow
}

void VideoProcessor::drawSphere(cv::Mat& frame, const std::vector<std::pair<cv::Point2f, int>>& balls) {
    // Ensure minimap coordinates have been initialized
    if (dstCorners.empty()) {
        std::cerr << "Error: Minimap coordinates are not initialized" << std::endl;
        return;
    }

    // Draw spheres inside the segmented field
    for (const auto& ball : balls) {
        cv::Point2f tableCoordinates = ball.first;
        int id = ball.second;

        // Transform sphere coordinates from the main table to the minimap
        cv::Point2f minimapSphereCoordinates;
        std::vector<cv::Point2f> srcPoint(1, tableCoordinates);  // Insert sphere coordinates as a vector of points
        std::vector<cv::Point2f> dstCornersTransformed(1);  // Vector for transformed coordinates

        // Apply perspective transformation
        cv::perspectiveTransform(srcPoint, dstCornersTransformed, perspectiveTransform);

        // Extract transformed coordinates
        minimapSphereCoordinates = dstCornersTransformed[0];

        // Choose ball color based on id
        cv::Scalar ballColor;
        switch (id) {
            case 0:  // White
                ballColor = cv::Scalar(255, 255, 255);
                break;
            case 1:  // Black
                ballColor = cv::Scalar(0, 0, 0);
                break;
            case 2:  // Blue
                ballColor = cv::Scalar(255, 0, 0);
                break;
            case 3:  // Red
                ballColor = cv::Scalar(0, 0, 255);
                break;
            default:
                std::cerr << "Error: Invalid id for ball color" << std::endl;
                return;
        }

        // Draw filled circle for the ball
        cv::circle(frame, minimapSphereCoordinates, 5, ballColor, -1);

        // Draw black outline around the ball
        cv::circle(frame, minimapSphereCoordinates, 5, cv::Scalar(0, 0, 0), 2);
    }
}

void VideoProcessor::segmentField(cv::Mat &frame, const std::vector<cv::Point2f> &corners, const std::vector<std::pair<cv::Point2f, int>>& balls) {
    // Verify that exactly 4 corners are provided for field segmentation
    if (corners.size() != 4) {
        std::cerr << "Error: Exactly 4 corners are required for field segmentation" << std::endl;
        return;
    }

    // Create a vector of points to represent the polygon of corners
    std::vector<cv::Point> polygon;
    for (const auto& corner : corners) {
        polygon.push_back(corner);
    }

    // Create an empty mask for filling the polygon
    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);

    // Create a vector of vectors of points to represent the polygon in the format required by fillPoly function
    std::vector<std::vector<cv::Point>> pts = { polygon };

    // Fill the polygon in the mask
    cv::fillPoly(mask, pts, cv::Scalar(255));

    // Apply the mask to fill the polygon with green color in the original image
    frame.setTo(cv::Scalar(0, 255, 0), mask);  // Green color

    // Draw the spheres inside the segmented field
    for (const auto& ball : balls) {
        cv::Point2f ballCenter = ball.first;
        int ballId = ball.second;

        // Check if the ball id is valid
        if (ballId < 0 || ballId > 3) {
            std::cerr << "Error: Ball id must be between 0 and 3" << std::endl;
            continue;
        }

        // Draw the ball with the appropriate color inside the segmented polygon
        cv::Scalar ballColor;
        switch (ballId) {
            case 0: ballColor = cv::Scalar(255, 255, 255); break; // White
            case 1: ballColor = cv::Scalar(0, 0, 0); break;       // Black
            case 2: ballColor = cv::Scalar(255, 0, 0); break;     // Blue
            case 3: ballColor = cv::Scalar(0, 0, 255); break;     // Red
        }

        cv::circle(frame, ballCenter, 10, ballColor, -1); // Draw filled circle for the ball
        cv::circle(frame, ballCenter, 10, cv::Scalar(0, 0, 0), 2); // Draw black outline around the ball
    }
}


void VideoProcessor::processVideo(const std::string& videoPath) {
    // Trova i corners nel video
    std::vector<cv::Point2f> corners = findCorners(videoPath);

    // Verifica se sono stati trovati esattamente 4 corners
    if (corners.size() != 4) {
        std::cerr << "Error: Could not find exactly 4 corners" << std::endl;
        return;
    }

    // Ordina i corners in senso orario
    corners = sortCorners(corners);

    // Apre il video
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error opening file: " << videoPath << std::endl;
        return;
    }

    // Esempio di coordinate delle sfere all'interno dell'area segmentata
    std::vector<std::pair<cv::Point2f, int>> balls;
    balls.push_back(std::make_pair(cv::Point2f(450, 300), 0)); // Sfera bianca
    balls.push_back(std::make_pair(cv::Point2f(400, 250), 1)); // Sfera nera

    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::Mat result = frame.clone(); // Crea una copia del frame per risultati

        // Disegna i corners trovati come cerchi rossi
        for (size_t i = 0; i < corners.size(); ++i) {
            cv::circle(result, corners[i], 5, cv::Scalar(0, 0, i * 60), -1); // Colora i corners
        }

        // Segmenta l'area interna ai corners con colore verde e disegna le sfere
        segmentField(result, corners, balls);

        // Disegna i bordi della tavola da biliardo
        cv::line(result, corners[0], corners[1], cv::Scalar(0, 0, 255), 2); // Linea rossa
        cv::line(result, corners[1], corners[2], cv::Scalar(0, 255, 0), 2); // Linea verde
        cv::line(result, corners[2], corners[3], cv::Scalar(255, 0, 0), 2); // Linea blu
        cv::line(result, corners[3], corners[0], cv::Scalar(0, 255, 255), 2); // Linea gialla

        // Disegna la minimappa
        drawMinimap(result, corners);

        // Disegna le sfere sulla tavola da biliardo
        drawSphere(result, balls);

        // Mostra il frame risultante con i corners, i bordi, la minimappa e le sfere
        cv::imshow("Detected Corners, Borders and Minimap", result);

        // Attendi per il tasto premuto o esci se non c'è più input
        if (cv::waitKey(30) >= 0) break;
    }

    // Rilascia il video e chiudi tutte le finestre
    cap.release();
    cv::destroyAllWindows();
}


