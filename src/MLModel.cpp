#include "MLModel.h"
#include <filesystem>

namespace fs = std::filesystem;

void MLModel::trainModel(const std::string& datasetPath) {
    // Load dataset
    std::vector<cv::Mat> images = loadDataset(datasetPath);

    // TODO: Training logic here using the loaded images
}

void MLModel::predictOutcome(cv::Mat &frame) {
    // Prediction logic using the trained model
    // Display the result
}

std::vector<cv::Mat> MLModel::loadDataset(const std::string& datasetPath) {
    std::vector<cv::Mat> images;
    
    for (const auto& entry : fs::directory_iterator(datasetPath)) {
        if (fs::is_directory(entry.status())) {
            for (const auto& file : fs::directory_iterator(entry.path())) {
                if (file.path().extension() == ".png") {
                    images.push_back(cv::imread(file.path().string(), cv::IMREAD_COLOR));
                }
            }
        }
    }
    return images;
}
