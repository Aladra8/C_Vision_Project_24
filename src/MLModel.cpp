// #include "MLModel.h"
// #include <filesystem>

// namespace fs = std::filesystem;

// void MLModel::trainModel(const std::string &datasetPath)
// {
//     std::vector<cv::Mat> images = loadDataset(datasetPath);
//     // TODO: Add training logic here
// }

// void MLModel::predictOutcome(cv::Mat &frame)
// {
//     // TODO: Add prediction logic using the trained model
// }

// std::vector<cv::Mat> MLModel::loadDataset(const std::string &datasetPath)
// {
//     std::vector<cv::Mat> images;

//     for (const auto &entry : fs::directory_iterator(datasetPath))
//     {
//         if (fs::is_directory(entry.status()))
//         {
//             for (const auto &file : fs::directory_iterator(entry.path()))
//             {
//                 if (file.path().extension() == ".png")
//                 {
//                     images.push_back(cv::imread(file.path().string(), cv::IMREAD_COLOR));
//                 }
//             }
//         }
//     }
//     return images;
// }

#include "MLModel.h"
#include <fstream>
#include <iostream>

MLModel::MLModel()
{
    // Initialize TensorFlow session
    graph = TF_NewGraph();
    status = TF_NewStatus();
    TF_SessionOptions *sess_opts = TF_NewSessionOptions();
    session = TF_NewSession(graph, sess_opts, status);
}

MLModel::~MLModel()
{
    TF_CloseSession(session, status);
    TF_DeleteSession(session, status);
    TF_DeleteGraph(graph);
    TF_DeleteStatus(status);
}

void MLModel::loadModel(const std::string &modelPath)
{
    std::ifstream file(modelPath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open model file: " << modelPath << std::endl;
        throw std::runtime_error("Failed to load model file.");
    }

    file.seekg(0, std::ios::end);
    size_t fsize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fsize);
    file.read(buffer.data(), fsize);

    TF_Buffer *tf_buffer = TF_NewBuffer();
    tf_buffer->data = buffer.data();
    tf_buffer->length = buffer.size();
    tf_buffer->data_deallocator = nullptr;

    TF_ImportGraphDefOptions *graph_opts = TF_NewImportGraphDefOptions();
    TF_GraphImportGraphDef(graph, tf_buffer, graph_opts, status);
    TF_DeleteImportGraphDefOptions(graph_opts);
    TF_DeleteBuffer(tf_buffer);

    if (TF_GetCode(status) != TF_OK)
    {
        std::cerr << "Failed to import graph: " << TF_Message(status) << std::endl;
        throw std::runtime_error("Failed to import graph.");
    }
}

std::vector<cv::Rect> MLModel::detectObjects(const cv::Mat &frame)
{
    // Implement the detection logic using TensorFlow here
    // This is a placeholder implementation
    std::vector<cv::Rect> detectedObjects;
    // Placeholder: Return an empty vector
    return detectedObjects;
}
