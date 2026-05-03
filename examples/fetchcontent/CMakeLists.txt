cmake_minimum_required(VERSION 3.23)
project(fetchcontent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRCS "src/*.cpp")

# get fftune using FetchContent
include(FetchContent)
FetchContent_Declare(fftune GIT_REPOSITORY https://github.com/fftune/fftune.git GIT_TAG origin/master)
FetchContent_MakeAvailable(fftune)

add_executable("${PROJECT_NAME}" ${SRCS})
target_link_libraries("${PROJECT_NAME}" fftune)

# install
include(GNUInstallDirs)
install(TARGETS "${PROJECT_NAME}")
