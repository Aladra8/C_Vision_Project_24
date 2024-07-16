# cmake_minimum_required(VERSION 3.10)

# project(C_Vision_Project)

# set(CMAKE_CXX_STANDARD 14)

# find_package(OpenCV REQUIRED)
# find_package(Qt5Widgets REQUIRED)

# include_directories(${OpenCV_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} include)

# add_executable(C_Vision_Project
#     src/main.cpp
#     src/ImageProcessor.cpp
#     src/BallTracker.cpp
#     src/MLModel.cpp
#     src/MainWindow.cpp
# )

# target_link_libraries(C_Vision_Project ${OpenCV_LIBS} Qt5::Widgets)

cmake_minimum_required(VERSION 3.10)

project(C_Vision_Project)

set(CMAKE_CXX_STANDARD 14)

# Set the OpenCV_DIR to the directory containing OpenCVConfig.cmake
set(OpenCV_DIR /usr/local/Cellar/opencv/4.9.0_9/lib/cmake/opencv4)

# Set the Qt5Widgets_DIR to the directory containing Qt5WidgetsConfig.cmake
set(Qt5Widgets_DIR /usr/local/Cellar/qt/6.7.0_1/lib/cmake/Qt6Widgets)

find_package(OpenCV REQUIRED)
find_package(Qt6Widgets REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ${Qt6Widgets_INCLUDE_DIRS} include)

add_executable(C_Vision_Project
    src/main.cpp
    src/BallTracker.cpp
    src/MLModel.cpp
    src/MainWindow.cpp
    src/VideoProcessor.cpp
    src/BallTracking.cpp
)

target_link_libraries(C_Vision_Project ${OpenCV_LIBS} Qt6::Widgets)
