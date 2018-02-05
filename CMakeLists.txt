cmake_minimum_required(VERSION 2.6)
project(openglvideo)

add_executable(openglvideo main.cpp)

install(TARGETS openglvideo RUNTIME DESTINATION bin)
