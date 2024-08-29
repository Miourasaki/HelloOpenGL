cmake_minimum_required(VERSION 3.28)
project(HelloOpenGL)

set(CMAKE_CXX_STANDARD 17)

link_directories(library)

add_executable(HelloOpenGL main.cpp)
target_include_directories(HelloOpenGL PRIVATE include/)
target_link_libraries(HelloOpenGL glew32s)
target_link_libraries(HelloOpenGL glfw3)

target_link_libraries(HelloOpenGL opengl32)
