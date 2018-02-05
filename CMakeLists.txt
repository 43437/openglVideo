cmake_minimum_required(VERSION 2.6)
project(openglvideo)

set(CMAKE_CXX_FLAGS "-std=c++11")

set(base_dir ${CMAKE_CURRENT_LIST_DIR})

set(src_dir ${base_dir})

file(GLOB src_files ${src_dir}/*.cpp)

include_directories("/home/acmax/app/ffmpeg-dev/include/")

link_directories("/home/acmax/app/ffmpeg-dev/lib")

add_executable(openglvideo ${src_files})

target_link_libraries(openglvideo GL GLEW glfw avformat avcodec avutil swscale)

#install(TARGETS openglvideo RUNTIME DESTINATION bin)
