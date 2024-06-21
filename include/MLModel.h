// #ifndef MLMODEL_H
// #define MLMODEL_H

// #include <opencv2/opencv.hpp>
// #include <string>
// #include <vector>

// class MLModel {
// public:
//     void trainModel(const std::string &datasetPath);
//     void predictOutcome(cv::Mat &frame);

// private:
//     std::vector<cv::Mat> loadDataset(const std::string &datasetPath);
// };

// #endif // MLMODEL_H

#ifndef MLMODEL_H
#define MLMODEL_H

#include <opencv2/opencv.hpp>
#include <tensorflow/c/c_api.h>
#include <string>
#include <vector>

class MLModel {
public:
    MLModel();
    ~MLModel();
    void loadModel(const std::string &modelPath);
    void trainModel(const std::string &datasetPath);
    void predictOutcome(cv::Mat &frame);

private:
    std::vector<cv::Mat> loadDataset(const std::string &datasetPath);
    TF_Graph* graph;
    TF_Session* session;
    TF_Status* status;
    TF_SessionOptions* sessionOptions;
};

#endif // MLMODEL_H
