# CMake Version
cmake_minimum_required(VERSION 3.23.2)
# Project Name
project(mkr_engine)

# Includes
include(FetchContent)

# C++ Version
set(CMAKE_CXX_STANDARD 20)

# CMake Flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_BUILD_TYPE Debug)

# Compiler Flags
# set(CMAKE_CXX_FLAGS "-std=c++20 -O3")
set(CMAKE_CXX_FLAGS "-std=c++20 -g -fsanitize=thread")

set(SRC_DIR "src/")
file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES true CONFIGURE_DEPENDS
        "${SRC_DIR}/*.h"
        "${SRC_DIR}/*.c"
        "${SRC_DIR}/*.hpp"
        "${SRC_DIR}/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_FILES})

# External Dependencies
FetchContent_Declare(mkr_common_lib GIT_REPOSITORY https://github.com/TypeDefinition/mkr_common_lib.git GIT_TAG main)
FetchContent_MakeAvailable(mkr_common_lib)

FetchContent_Declare(mkr_maths_lib GIT_REPOSITORY https://github.com/TypeDefinition/mkr_maths_lib.git GIT_TAG main)
FetchContent_MakeAvailable(mkr_maths_lib)

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

# Target
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR})
target_link_libraries(${PROJECT_NAME} mkr_maths_lib mkr_common_lib GL GLEW SDL2 SDL2_image spdlog::spdlog)