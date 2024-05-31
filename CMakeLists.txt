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
endif()

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

FetchContent_Declare(flecs GIT_REPOSITORY https://github.com/SanderMertens/flecs.git GIT_TAG 37233f127d5006ceb0087fbfcd2f3e36f5b77a23)
FetchContent_MakeAvailable(flecs)

FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG c3aed4b68373955e1cc94307683d44dca1515d2b)
FetchContent_MakeAvailable(spdlog)

# Dependencies
set(SDL2_INC_DIR ${PROJECT_SOURCE_DIR}/dep/sdl2/include)
set(SDL2_LIB_DIR ${PROJECT_SOURCE_DIR}/dep/sdl2/lib)

set(SDL2_IMG_INC_DIR ${PROJECT_SOURCE_DIR}/dep/sdl2_image/include)
set(SDL2_IMG_LIB_DIR ${PROJECT_SOURCE_DIR}/dep/sdl2_image/lib)

set(GLEW_INC_DIR ${PROJECT_SOURCE_DIR}/dep/glew/include)
set(GLEW_LIB_DIR ${PROJECT_SOURCE_DIR}/dep/glew/lib/Release/x64)

# Target
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIR}
                                                  ${SDL2_INC_DIR}
                                                  ${SDL2_INC_DIR}/SDL2 # Needed because SDL2_image does not use the "SDL2/" prefix when including SDL header files.
                                                  ${SDL2_IMG_INC_DIR}
                                                  ${GLEW_INC_DIR})

target_link_directories(${PROJECT_NAME} PUBLIC ${SDL2_LIB_DIR}
                                               ${SDL2_IMG_LIB_DIR}
                                               ${GLEW_LIB_DIR})

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads
                                                 mkr_maths mkr_common mkr_glsl_include
                                                 SDL2main SDL2 SDL2_image
                                                 glew32 opengl32
                                                 spdlog flecs)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads
                                                 mkr_maths mkr_common mkr_glsl_include
                                                 SDL2 SDL2_image
                                                 GLEW GL
                                                 spdlog flecs)
endif()
