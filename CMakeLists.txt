# 1. 声明需要的 CMake 最低版本
cmake_minimum_required(VERSION 3.10)

# 2. 定义项目名称和语言
project(ArmorDetector LANGUAGES CXX)

# 3. 设置 C++ 标准（推荐 C++14 或 C++17）
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. 【非常关键】指定你自定义安装的 OpenCV 路径
# 因为你是装在 /usr/local/opencv4.5.4，不指定这行会找不到库
set(OpenCV_DIR "/usr/local/opencv4.5.4/lib/cmake/opencv4")

# 5. 寻找 OpenCV 库
find_package(OpenCV REQUIRED)

# 6. 打印一下找到的 OpenCV 信息，确认版本正确
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# 7. 包含头文件目录（让编译器能找到 cv::Mat 等）
include_directories(${OpenCV_INCLUDE_DIRS})

# 8. 添加你的源代码，生成可执行文件
# 语法：add_executable(可执行文件名 源文件路径)
add_executable(detect_armor src/main.cpp)

# 9. 链接 OpenCV 库到你的可执行文件上
target_link_libraries(detect_armor ${OpenCV_LIBS})