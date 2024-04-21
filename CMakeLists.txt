cmake_minimum_required(VERSION 3.10)  # Ensure compatibility with your CMake version
project(UrbanTree)

# Set the C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Do not use compiler-specific extensions

# Define the executable
add_executable(UrbanTree main.cpp)
# Optionally add other source files if your project has more than one
# target_sources(UrbanTree PRIVATE other_file.cpp)

# Include the protozero headers using target_include_directories for better target-specific management
target_include_directories(UrbanTree PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/protozero/include)

# Find Protobuf installation and link it
find_package(Protobuf REQUIRED)
if(Protobuf_FOUND)
    message("Protobuf version: ${Protobuf_VERSION}")
    target_include_directories(UrbanTree PRIVATE ${PROTOBUF_INCLUDE_DIRS})
    target_link_libraries(UrbanTree PRIVATE ${PROTOBUF_LIBRARIES})
else()
    message(FATAL_ERROR "Protobuf not found")
endif()

# Check and configure the Abseil package
find_package(absl CONFIG REQUIRED COMPONENTS strings base)
if(absl_FOUND)
    target_link_libraries(UrbanTree PRIVATE absl::strings absl::base)
else()
    message(FATAL_ERROR "Abseil not found")
endif()
