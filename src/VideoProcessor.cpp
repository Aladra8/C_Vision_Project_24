#include "VideoProcessor.h"
#include "BallTracking.h"

// Detects the borders given a video path
std::vector<cv::Point2f> VideoProcessor::findCorners(const std::string& videoPath){
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()){
        throw std::invalid_argument("Error opening file: " + videoPath);
    }

    cv::Mat frame, grayFrame, edges;
    cv::Scalar lowerBound(45, 80, 80);     // Lower HSV threshold for color filtering
    cv::Scalar upperBound(140, 255, 255);  // Upper HSV threshold for color filtering
    const float minCornerDistance = 150.0; // Minimum distance between corners
    const int margin = 50;  // Margin of the image to ignore

    while (true){
        cap >> frame;
        if (frame.empty()) break;

        // Mask to exclude outer edges of the image
        cv::Mat mask = cv::Mat::ones(frame.size(), CV_8UC1) * 255;
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
                float denominator = sin(theta1 - theta2);

                // Avoid division by zero
                if (std::abs(denominator) < 1e-10){
                    continue;
                }

                intersection.x = (rho2 * sin(theta1) - rho1 * sin(theta2)) / denominator;
                intersection.y = (rho1 * cos(theta2) - rho2 * cos(theta1)) / denominator;

                // Check if intersection point is within image bounds and not too close to image edges
                if (intersection.x > 0 && intersection.x < frame.cols &&
                    intersection.y > margin && intersection.y < frame.rows - margin){
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

    throw std::runtime_error("Could not find exactly 4 corners in the video.");
}

// Sorts the corners in the same order for every video scenario
std::vector<cv::Point2f> VideoProcessor::sortCorners(const std::vector<cv::Point2f>& corners){
    // Calculate centroid of the four corners
    cv::Point2f centroid(0, 0);
    for (const auto& corner : corners){
        centroid += corner;
    }
    centroid *= (1.0 / corners.size());

    // Calculate angles of corners relative to the centroid
    std::vector<double> angles;
    for (const auto& corner : corners){
        double angle = std::atan2(corner.y - centroid.y, corner.x - centroid.x);
        angles.push_back(angle);
    }

    // Sort corners in clockwise order based on angles
    std::vector<cv::Point2f> sortedCorners = corners;
    for (size_t i = 0; i < sortedCorners.size(); ++i){
        for (size_t j = i + 1; j < sortedCorners.size(); ++j){
            if (angles[i] > angles[j]){
                std::swap(sortedCorners[i], sortedCorners[j]);
                std::swap(angles[i], angles[j]);
            }
        }
    }

    // Calculate distances between corners to determine if rotation is necessary
    double dist01 = cv::norm(sortedCorners[0] - sortedCorners[1]);
    double dist12 = cv::norm(sortedCorners[1] - sortedCorners[2]);
    double dist23 = cv::norm(sortedCorners[2] - sortedCorners[3]);

    double borderMargin = 0.2; // Margin to check if parallel borders are significantly different

    // Rotate if line 01 is shorter than 12
    if (dist01 < dist12 || dist23 < dist12 || dist01 > dist23 + dist23 * borderMargin || dist23 > dist01 + dist01 * borderMargin){
        // Reorder corners so that index 0 becomes 1, 1 becomes 2, 2 becomes 3, 3 becomes 0
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

// Draws a 2D view of the playing field
void VideoProcessor::drawMinimap(cv::Mat& frame, const std::vector<cv::Point2f>& corners){
    // Dimensions of the minimap
    const int minimapWidth = 300;
    const int minimapHeight = 150;
    const int borderThickness = 2;  // Thickness of the borders
    const int pocketRadius = 7;  // Radius of the pockets
    const int padding = 10;  // Padding for the outer border

    // Define the four corners of the minimap at the bottom-left
    if (dstCorners.empty()){
        dstCorners = {
            cv::Point2f(padding, frame.rows - minimapHeight - padding),
            cv::Point2f(minimapWidth + padding, frame.rows - minimapHeight - padding),
            cv::Point2f(minimapWidth + padding, frame.rows - padding),
            cv::Point2f(padding, frame.rows - padding)
        };
    }

    // Calculate the perspective transform from the main table to the minimap
    if (perspectiveTransform.empty()){
        perspectiveTransform = cv::getPerspectiveTransform(corners, dstCorners);
    }

    // Create an empty minimap with white background and padding
    cv::Mat minimap = cv::Mat::zeros(minimapHeight + 2 * padding, minimapWidth + 2 * padding, CV_8UC3);
    minimap.setTo(cv::Scalar(255, 255, 255)); // Set white background

    // Draw the outer border
    cv::rectangle(minimap, cv::Point(0, 0), 
                  cv::Point(minimapWidth + 2 * padding - 1, minimapHeight + 2 * padding - 1), 
                  cv::Scalar(0, 0, 0), borderThickness);

    // Draw the borders of the pool table
    cv::rectangle(minimap, cv::Point(padding, padding), 
                  cv::Point(minimapWidth + padding - 1, minimapHeight + padding - 1), 
                  cv::Scalar(0, 0, 0), borderThickness);

    // Pocket positions
    std::vector<cv::Point> pocketPositions = {
        cv::Point(padding, padding),  // Top-left corner
        cv::Point(minimapWidth / 2 + padding, padding), // Top-center
        cv::Point(minimapWidth + padding - 1, padding),  // Top-right corner
        cv::Point(padding, minimapHeight + padding - 1),  // Bottom-left corner
        cv::Point(minimapWidth / 2 + padding, minimapHeight + padding - 1),  // Bottom-center
        cv::Point(minimapWidth + padding - 1, minimapHeight + padding - 1)  // Bottom-right corner
    };

    for (const auto& pos : pocketPositions){
        cv::circle(minimap, pos, pocketRadius, cv::Scalar(0, 0, 0), -1);  // Draw black filled circle
        cv::circle(minimap, pos, pocketRadius - 2, cv::Scalar(255, 255, 255), -1);  // Draw smaller white filled circle
    }

    // Position of the minimap on the main frame
    cv::Rect roi(cv::Point(0, frame.rows - minimapHeight - 2 * padding), minimap.size());

    // Copy the minimap with white background onto the main frame
    minimap.copyTo(frame(roi));
}

// Draws the detected balls on the 2D view
void VideoProcessor::drawSphere(cv::Mat& frame, const std::unordered_map<int, BallInfo>& detectedBalls){
    // Ensure minimap coordinates have been initialized
    if (dstCorners.empty()){
        throw std::runtime_error("Error: Minimap coordinates are not initialized");
    }

    // Draw spheres inside the segmented field
    for (const auto& ball : detectedBalls){
        const BallInfo& ballInfo = ball.second;
        cv::Point2f tableCoordinates = ballInfo.center;


        // Transform sphere coordinates from the main table to the minimap
        cv::Point2f minimapSphereCoordinates;
        std::vector<cv::Point2f> srcPoint(1, tableCoordinates);  // Insert sphere coordinates as a vector of points
        std::vector<cv::Point2f> dstCornersTransformed(1);  // Vector for transformed coordinates

        // Apply perspective transformation
        cv::perspectiveTransform(srcPoint, dstCornersTransformed, perspectiveTransform);

        // Extract transformed coordinates
        minimapSphereCoordinates = dstCornersTransformed[0];

        // Draw filled circle for the ball
        cv::circle(frame, minimapSphereCoordinates, 5, ballInfo.color, -1);

        // Draw black outline around the ball
        cv::circle(frame, minimapSphereCoordinates, 5, cv::Scalar(0, 0, 0), 2);
    }
}

// Segments the playing field by filling the polygon determined by the corners
void VideoProcessor::segmentField(cv::Mat &frame, const std::vector<cv::Point2f> &corners){

    // Create a vector of points to represent the polygon of corners
    std::vector<cv::Point> polygon;
    for (const auto& corner : corners){
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

}

// Draws the lines connecting the borders
void VideoProcessor::drawBorders(cv::Mat& frame, const std::vector<cv::Point2f>& corners){
    // Draw the borders of the pool table, connects the lines in clockwise order
    cv::line(frame, corners[0], corners[1], cv::Scalar(0, 255, 255), 2); 
    cv::line(frame, corners[1], corners[2], cv::Scalar(0, 255, 255), 2); 
    cv::line(frame, corners[2], corners[3], cv::Scalar(0, 255, 255), 2); 
    cv::line(frame, corners[3], corners[0], cv::Scalar(0, 255, 255), 2); 
}

// Process the whole video, applying table/ball segmentation and 2D view
void VideoProcessor::processVideo(const std::string& videoPath){
    
    // Open the video
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()){
        throw std::invalid_argument("Error opening file: " + videoPath);
    }

    // Find corners in the video
    std::vector<cv::Point2f> corners = findCorners(videoPath);

    // Sort corners clockwise
    corners = sortCorners(corners);

    cv::Mat frame;
    while (true){
        cap >> frame;
        if (frame.empty()) break;

        cv::Mat result = frame.clone(); // Create a copy of the frame for results

/* 
        // Draw found corners as red circles, it can be used to visualize the order in which the are stored
        for (size_t i = 0; i < corners.size(); ++i){
            cv::circle(result, corners[i], 5, cv::Scalar(0, 0, i * 60), -1); // Color the corners
        }
*/
        // Draw the borders of the pool table
        drawBorders(result, corners);

        // Segment the area inside the corners with green color and draw the balls
        segmentField(result, corners);

        std::unordered_map<int, BallInfo> detectedBalls = detectAndDrawBalls(videoPath, frame, result);

        // Draw the minimap
        drawMinimap(result, corners);

        // Draw the balls on the pool table
        drawSphere(result, detectedBalls);

        // Show the resulting frame with corners, borders, minimap, and balls
        cv::imshow("Detected Corners, Borders and Minimap", result);

        // Wait for a key press or exit if there is no more input
        if (cv::waitKey(30) >= 0) break;
    }

    // Release the video and close all windows
    cap.release();
    cv::destroyAllWindows();
}


