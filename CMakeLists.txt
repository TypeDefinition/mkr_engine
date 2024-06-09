# CMake Version
cmake_minimum_required(VERSION 3.23.2)

# Project Name
project(mkr_engine)

# CMake Flags
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
# set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=thread")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Threads
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Set Executable Directory (Ensure that whatever DLLs we need are placed here.)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Definitions
if (WIN32) # WIN32 includes 64-bit Windows.
    add_compile_definitions(SDL_MAIN_HANDLED)
endif ()

# Main Executable
set(SRC_DIR "src")
file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES true CONFIGURE_DEPENDS
        "${SRC_DIR}/*.h"
        "${SRC_DIR}/*.c"
        "${SRC_DIR}/*.hpp"
        "${SRC_DIR}/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_FILES})

# External Dependencies
include(FetchContent)

FetchContent_Declare(mkr_common GIT_REPOSITORY https://github.com/TypeDefinition/mkr_common.git GIT_TAG main)
FetchContent_MakeAvailable(mkr_common)

FetchContent_Declare(mkr_maths GIT_REPOSITORY https://github.com/TypeDefinition/mkr_maths.git GIT_TAG main)
FetchContent_MakeAvailable(mkr_maths)

FetchContent_Declare(mkr_glsl_include GIT_REPOSITORY https://github.com/TypeDefinition/mkr_glsl_include.git GIT_TAG main)
FetchContent_MakeAvailable(mkr_glsl_include)

# Some weird bug is preventing compilation from working if included directly. I think MinGW hates spdlog, so I have to fetch content.
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG da14258533cb951ce85087ceb45556e0b8253660)
FetchContent_MakeAvailable(spdlog)

# Target
target_link_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/dep/sdl2/lib
        ${PROJECT_SOURCE_DIR}/dep/sdl2_image/lib
        ${PROJECT_SOURCE_DIR}/dep/glew/lib/Release/x64)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${SRC_DIR}
        ${PROJECT_SOURCE_DIR}/dep/sdl2/include
        ${PROJECT_SOURCE_DIR}/dep/sdl2/include/SDL2
        ${PROJECT_SOURCE_DIR}/dep/sdl2_image/include
        ${PROJECT_SOURCE_DIR}/dep/glew/include
        ${PROJECT_SOURCE_DIR}/dep/roaring/include
        ${PROJECT_SOURCE_DIR}/dep/stb/include)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads
            mkr_maths mkr_common mkr_glsl_include
            SDL2main SDL2 SDL2_image
            glew32 opengl32 spdlog)
else ()
    target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads
            mkr_maths mkr_common mkr_glsl_include
            SDL2 SDL2_image
            GLEW GL spdlog)
endif ()