cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(EasyAudioMonitor VERSION 1.0)

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加可执行文件
add_executable(EasyAudioMonitor main.cpp)

# 链接 Windows Multimedia 库
target_link_libraries(EasyAudioMonitor winmm)
