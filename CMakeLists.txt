cmake_minimum_required(VERSION 3.10)  # Ensure compatibility with your CMake version
project(UrbanTree)

# Set the C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Do not use compiler-specific extensions

# Include the protozero headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/protozero/include)

# Define the executable
add_executable(UrbanTree main.cpp)
# Optionally add other source files if your project has more than one
# add_executable(UrbanTree main.cpp other_file.cpp)

# Find Protobuf installation
find_package(Protobuf REQUIRED)
if(Protobuf_FOUND)
    message("Protobuf version: ${Protobuf_VERSION}")
    include_directories(${PROTOBUF_INCLUDE_DIRS})
    target_link_libraries(UrbanTree ${PROTOBUF_LIBRARIES})
else()
    message(FATAL_ERROR "Protobuf not found")
endif()

# Check and configure the Abseil package
# Note: Ensure your system has a find_package configuration for Abseil or that it's installed in a known path
find_package(absl REQUIRED COMPONENTS strings base)
if(absl_FOUND)
    target_link_libraries(UrbanTree absl::strings absl::base)
else()
    message(FATAL_ERROR "Abseil not found")
endif()
