#ifndef MLMODEL_H
#define MLMODEL_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class MLModel {
public:
    void trainModel(const std::string& datasetPath);
    void predictOutcome(cv::Mat &frame);

private:
    std::vector<cv::Mat> loadDataset(const std::string& datasetPath);
};

#endif // MLMODEL_H
