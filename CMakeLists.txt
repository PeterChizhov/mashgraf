cmake_minimum_required(VERSION 3.8)
project(proj5sem)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(OpenGlLinkers -lGLEW -lglut -lGLU -lGL -lglfw -lm  -lSOIL)

set(SOURCE_FILES main.cpp)
add_executable(proj5sem ${SOURCE_FILES})
target_link_libraries(proj5sem ${OpenGlLinkers})

