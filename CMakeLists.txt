# cmake_minimum_required(VERSION 3.10)

# project(C_Vision_Project)

# set(CMAKE_CXX_STANDARD 14)

# # Set the OpenCV_DIR to the directory containing OpenCVConfig.cmake
# set(OpenCV_DIR /usr/local/Cellar/opencv/4.9.0_9/lib/cmake/opencv4)

# # Set the Qt6_DIR to the directory containing Qt6WidgetsConfig.cmake
# set(Qt6_DIR /usr/local/Cellar/qt/6.7.0_1/lib/cmake/Qt6)

# find_package(OpenCV REQUIRED)
# find_package(Qt6 COMPONENTS Widgets REQUIRED)

# include_directories(${OpenCV_INCLUDE_DIRS} ${Qt6_INCLUDE_DIRS} include)

# # Ensure that qt_wrap_cpp processes MainWindow.h
# qt_wrap_cpp(MOC_SOURCES include/MainWindow.h)

# add_executable(C_Vision_Project
#     src/main.cpp
#     src/ImageProcessor.cpp
#     src/BallTracker.cpp
#     src/MLModel.cpp
#     src/MainWindow.cpp
#     ${MOC_SOURCES}
# )

# target_link_libraries(C_Vision_Project ${OpenCV_LIBS} Qt6::Widgets)

cmake_minimum_required(VERSION 3.10)

project(C_Vision_Project)

set(CMAKE_CXX_STANDARD 14)

# Set the OpenCV_DIR to the directory containing OpenCVConfig.cmake
set(OpenCV_DIR /usr/local/Cellar/opencv/4.9.0_9/lib/cmake/opencv4)

# Set the Qt6_DIR to the directory containing Qt6WidgetsConfig.cmake
set(Qt6_DIR /usr/local/Cellar/qt/6.7.0_1/lib/cmake/Qt6)

# Set the TensorFlow paths to the directory containing TensorFlow headers and libraries
set(TensorFlow_INCLUDE_DIR /usr/local/Cellar/libtensorflow/2.16.1/include)
set(TensorFlow_LIB_DIR /usr/local/Cellar/libtensorflow/2.16.1/lib)

find_package(OpenCV REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS} 
    ${Qt6_INCLUDE_DIRS} 
    ${TensorFlow_INCLUDE_DIR}
    include
)

# Ensure that qt_wrap_cpp processes MainWindow.h
qt_wrap_cpp(MOC_SOURCES include/MainWindow.h)

add_executable(C_Vision_Project
    src/main.cpp
    src/ImageProcessor.cpp
    src/BallTracker.cpp
    src/MLModel.cpp
    src/MainWindow.cpp
    ${MOC_SOURCES}
)

target_link_libraries(C_Vision_Project 
    ${OpenCV_LIBS} 
    Qt6::Widgets 
    ${TensorFlow_LIB_DIR}/libtensorflow.dylib
    ${TensorFlow_LIB_DIR}/libtensorflow_framework.dylib
)
