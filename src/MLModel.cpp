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
#include <tensorflow/c/c_api.h>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

MLModel::MLModel() {
    graph = TF_NewGraph();
    status = TF_NewStatus();
    sessionOptions = TF_NewSessionOptions();
    session = nullptr;
    std::cout << "MLModel constructor: Initialized TensorFlow graph, status, and session options." << std::endl;
}

MLModel::~MLModel() {
    if (session) {
        TF_CloseSession(session, status);
        TF_DeleteSession(session, status);
    }
    TF_DeleteGraph(graph);
    TF_DeleteStatus(status);
    TF_DeleteSessionOptions(sessionOptions);
    std::cout << "MLModel destructor: Cleaned up TensorFlow resources." << std::endl;
}

void MLModel::loadModel(const std::string &modelPath) {
    std::cout << "Loading model from: " << modelPath << std::endl;

    std::ifstream file(modelPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open model file.");
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        throw std::runtime_error("Failed to read model file.");
    }

    TF_Buffer* graph_def = TF_NewBufferFromString(buffer.data(), buffer.size());
    TF_ImportGraphDefOptions* opts = TF_NewImportGraphDefOptions();
    TF_GraphImportGraphDef(graph, graph_def, opts, status);
    TF_DeleteImportGraphDefOptions(opts);
    TF_DeleteBuffer(graph_def);

    if (TF_GetCode(status) != TF_OK) {
        std::cerr << "Failed to import graph: " << TF_Message(status) << std::endl;
        throw std::runtime_error("Failed to import graph.");
    }

    std::cout << "Graph imported successfully." << std::endl;

    session = TF_NewSession(graph, sessionOptions, status);
    if (TF_GetCode(status) != TF_OK) {
        std::cerr << "Failed to create TensorFlow session: " << TF_Message(status) << std::endl;
        throw std::runtime_error("Failed to create TensorFlow session.");
    }

    std::cout << "TensorFlow session created successfully." << std::endl;
}

void MLModel::trainModel(const std::string &datasetPath) {
    std::vector<cv::Mat> images = loadDataset(datasetPath);
    // TODO: Add training logic here
}

void MLModel::predictOutcome(cv::Mat &frame) {
    if (!session) {
        throw std::runtime_error("Session not created.");
    }

    try {
        // Prepare input tensor
        TF_Output input_op = {TF_GraphOperationByName(graph, "input_node_name"), 0}; // Replace with your input node name
        if (input_op.oper == nullptr) {
            throw std::runtime_error("Failed to find input node in graph.");
        }

        const std::vector<int64_t> input_dims = {1, frame.rows, frame.cols, frame.channels()};
        TF_Tensor* input_tensor = TF_NewTensor(TF_UINT8, input_dims.data(), input_dims.size(),
                                               frame.data, frame.total() * frame.elemSize(), nullptr, nullptr);

        if (!input_tensor) {
            throw std::runtime_error("Failed to create input tensor.");
        }

        // Prepare output tensor
        TF_Output output_op = {TF_GraphOperationByName(graph, "output_node_name"), 0}; // Replace with your output node name
        if (output_op.oper == nullptr) {
            TF_DeleteTensor(input_tensor);
            throw std::runtime_error("Failed to find output node in graph.");
        }

        TF_Tensor* output_tensor = nullptr;

        // Run session
        TF_SessionRun(session,
                      nullptr, // Run options
                      &input_op, &input_tensor, 1, // Input tensors
                      &output_op, &output_tensor, 1, // Output tensors
                      nullptr, 0, // Target operations
                      nullptr, // Run metadata
                      status); // Output status

        if (TF_GetCode(status) != TF_OK) {
            TF_DeleteTensor(input_tensor);
            if (output_tensor) TF_DeleteTensor(output_tensor);
            throw std::runtime_error("Error during inference: " + std::string(TF_Message(status)));
        }

        // Process output tensor
        // TODO: Add logic to process the output_tensor and display results
        cv::imshow("Prediction", frame);

        // Cleanup
        TF_DeleteTensor(input_tensor);
        TF_DeleteTensor(output_tensor);
    } catch (const std::runtime_error& e) {
        std::cerr << "Prediction failed: " << e.what() << std::endl;
    }
}

std::vector<cv::Mat> MLModel::loadDataset(const std::string &datasetPath) {
    std::vector<cv::Mat> images;

    for (const auto &entry : fs::directory_iterator(datasetPath)) {
        if (fs::is_directory(entry.status())) {
            for (const auto &file : fs::directory_iterator(entry.path())) {
                if (file.path().extension() == ".png") {
                    images.push_back(cv::imread(file.path().string(), cv::IMREAD_COLOR));
                }
            }
        }
    }
    return images;
}
