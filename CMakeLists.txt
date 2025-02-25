# 设置最低的 CMake 版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(PathPlanning)

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 14)

# 设置构建类型
set(CMAKE_BUILD_TYPE "Debug")

# 查找 OpenCV 包
find_package(OpenCV REQUIRED)
# 查找线程库
find_package(Threads REQUIRED)

# 包含 OpenCV 头文件目录
include_directories(${OpenCV_INCLUDE_DIRS})

# 添加可执行文件
add_executable(PathPlanning main.cpp draw_map.cpp)  # 替换为你的源文件名

# 链接 OpenCV 库和线程库
target_link_libraries(PathPlanning 
    ${OpenCV_LIBS} 
    Threads::Threads
)

# 可选的功能
option(ENABLE_DEBUG "Enable debugging" ON)
if(ENABLE_DEBUG)
    add_definitions(-DDEBUG)
endif()
