cmake_minimum_required(VERSION 3.10)  # Ensure compatibility with CMake version
project(UrbanTree)

set(CMAKE_CXX_STANDARD 14)  # Use C++14 or higher
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Do not use compiler-specific extensions

add_executable(UrbanTree main.cpp)
# Add other necessary components like include directories, link libraries, etc.
